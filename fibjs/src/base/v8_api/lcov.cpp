/*
 * lcov.cpp
 *
 *  Created on: Sep 16, 2024
 *      Author: lion
 */

#include "v8.h"
#include "exlib/include/qstring.h"
#include "v8/src/api/api-inl.h"
#include "v8_api.h"
#include "Isolate.h"
#include "ifs/process.h"
#include "../../fs/match/path_match.h"

#include <set>

using namespace v8;

namespace fibjs {

bool path_isAbsolute(exlib::string path);

void beginCoverage(v8::Isolate* isolate)
{
    debug::Coverage::SelectMode(isolate, debug::CoverageMode::kBlockCount);
}

void pauseCoverage(v8::Isolate* isolate)
{
    debug::Coverage::SelectMode(isolate, debug::CoverageMode::kBestEffort);
}

inline std::string ToSTLString(v8::Isolate* isolate, Local<String> v8_str)
{
    String::Utf8Value utf8(isolate, v8_str);
    return *utf8;
}

// One line of the script source: [start, end) in UTF-16 code units with the
// line ending excluded, plus the execution count of that line.
struct CovLine {
    uint32_t start;
    uint32_t end;
    uint32_t count;
};

// One executed range of a function: the function itself or one of its blocks.
struct CovRange {
    uint32_t start;
    uint32_t end;
    uint32_t count;
};

// One function of the report.
struct CovFunction {
    std::string name;
    int32_t line;
    uint32_t count;
};

// V8 counts positions in UTF-16 code units, so the source is kept in that unit
// and the lines of the file are derived from it: whether a range covers a line
// completely can only be told from the real start and end of that line.
inline bool ReadScriptSource(v8::Isolate* isolate, Local<debug::Script> script, std::vector<uint16_t>& source)
{
    Local<debug::ScriptSource> src = script->Source();
    if (src.IsEmpty())
        return false;

    Local<String> code;
    if (!src->JavaScriptCode().ToLocal(&code))
        return false;

    int32_t len = code->Length();
    if (len <= 0)
        return false;

    source.resize(len);
    code->Write(isolate, source.data(), 0, len);

    return true;
}

// Split the source the way the lcov readers do: a line ends at `\n` (a `\r` in
// front of it belongs to the line ending and not to the line), the file's last
// line needs no ending, and a line without a character counts as covered:
// there is no code in it that could have been missed.
inline void BuildLines(const std::vector<uint16_t>& source, std::vector<CovLine>& lines)
{
    uint32_t start = 0;

    for (uint32_t i = 0; i < source.size(); i++) {
        if (source[i] != '\n')
            continue;

        uint32_t end = i;
        if (end > start && source[end - 1] == '\r')
            end--;

        lines.push_back({ start, end, start == end ? 1u : 0u });
        start = i + 1;
    }

    if (start < source.size())
        lines.push_back({ start, (uint32_t)source.size(), 0 });
}

// The number of lines the file has, by the same rule BuildLines applies. The
// compiled script carries the file with a wrapper around it (see cjs_Loader:
// `arg_names + "\n" + source + "\n});"`), so its own line count is not the
// file's, and the lines of the file are the ones the report is about. A file
// that is gone meanwhile is left out, like node's test coverage leaves it out.
inline int32_t ReadFileLineCount(const std::string& file_name)
{
    FILE* fp = fopen(file_name.c_str(), "rb");

    if (fp == nullptr)
        return -1;

    int32_t count = 0;
    bool last_is_eol = false;
    int ch;

    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') {
            count++;
            last_is_eol = true;
        } else
            last_is_eol = false;
    }

    fclose(fp);

    if (!last_is_eol)
        count++;

    return count;
}

// The line holding `offset`, or -1 for an offset inside a line ending.
inline int32_t FindLine(const std::vector<CovLine>& lines, uint32_t offset)
{
    int32_t lo = 0, hi = (int32_t)lines.size() - 1;

    while (lo <= hi) {
        int32_t mid = (lo + hi) / 2;

        if (offset < lines[mid].start)
            hi = mid - 1;
        else if (offset > lines[mid].end)
            lo = mid + 1;
        else
            return mid;
    }

    return -1;
}

// A range counts for each line it covers completely, and the count is assigned
// rather than merged: V8 hands the ranges over from the outer function to the
// inner ones, so the innermost range that covers a line decides it. That is the
// rule the node test runner applies, and it is what keeps the `}` of a branch
// that never ran uncovered. The old code took the maximum over the ranges, so
// that line was reported as covered, and the ranges of the wrapper a script is
// compiled with reached past the end of the file and added lines that do not
// exist.
inline void ApplyRange(std::vector<CovLine>& lines, const CovRange& range)
{
    int32_t first = FindLine(lines, range.start);
    if (first < 0)
        return;

    for (size_t i = first; i < lines.size() && lines[i].start < range.end; i++) {
        CovLine& line = lines[i];

        if (range.start <= line.start && range.end >= line.end)
            line.count = range.count;
    }
}

// The report is read next to the project it describes, so a file inside the
// working directory is named relative to it. A file outside keeps its absolute
// path: there is nothing sensible to be relative to.
inline std::string LcovPath(const std::string& file_name)
{
    exlib::string cwd;

    if (process_base::cwd(cwd) < 0)
        return file_name;

    std::string dir(cwd.c_str());

    for (size_t i = 0; i < dir.length(); i++) {
        if (dir[i] == '\\')
            dir[i] = '/';
    }

    if (dir.empty() || dir[dir.length() - 1] != '/')
        dir += '/';

    if (file_name.compare(0, dir.size(), dir) == 0)
        return file_name.substr(dir.size());

    return file_name;
}

// What a `--test` run must not count (the runner publishes it in
// opt_tools/test.js): the files the runner was handed, and the patterns a test
// file is recognised by. node leaves out every file matching those patterns,
// whether the runner was handed it or a test imported it, because a report
// about the code under test should not count the tests themselves. Any other
// run has no such list and reports every script it loaded.
struct CovExcludes {
    std::set<std::string> files;
    std::vector<std::string> patterns;

    bool isTest(const std::string& file_name) const
    {
        if (files.count(file_name) != 0)
            return true;

        // the path a pattern is matched against is the one the report names,
        // but a file outside the working directory stays absolute and has to be
        // matched as such
        std::string relative = LcovPath(file_name);
        std::string flat = file_name;

        for (size_t i = 0; i < flat.length(); i++) {
            if (flat[i] == '\\')
                flat[i] = '/';
        }

        for (size_t i = 0; i < patterns.size(); i++) {
            if (matchesGlob(relative, patterns[i], false) || matchesGlob(flat, patterns[i], false))
                return true;
        }

        return false;
    }
};

inline void CollectCovExcludes(v8::Isolate* isolate, CovExcludes& excludes)
{
    fibjs::Isolate* current = fibjs::Isolate::current();

    if (current == nullptr)
        return;

    v8::HandleScope handle_scope(isolate);
    Local<Context> context = current->context();

    if (context.IsEmpty())
        return;

    Local<Value> v = context->Global()
                         ->Get(context, String::NewFromUtf8(isolate, "__fibjs_test_cov_exclude",
                                             NewStringType::kNormal)
                                             .ToLocalChecked())
                         .FromMaybe(Local<Value>());

    if (v.IsEmpty() || !v->IsObject())
        return;

    Local<Object> options = v.As<Object>();
    Local<Value> files = options->Get(context, String::NewFromUtf8(isolate, "files",
                                                     NewStringType::kNormal)
                                                     .ToLocalChecked())
                             .FromMaybe(Local<Value>());

    if (!files.IsEmpty() && files->IsArray()) {
        Local<Array> list = files.As<Array>();

        for (uint32_t i = 0; i < list->Length(); i++) {
            Local<Value> e = list->Get(context, i).FromMaybe(Local<Value>());

            if (!e.IsEmpty() && e->IsString())
                excludes.files.insert(ToSTLString(isolate, e.As<String>()));
        }
    }

    Local<Value> patterns = options->Get(context, String::NewFromUtf8(isolate, "patterns",
                                                         NewStringType::kNormal)
                                                         .ToLocalChecked())
                                .FromMaybe(Local<Value>());

    if (!patterns.IsEmpty() && patterns->IsArray()) {
        Local<Array> list = patterns.As<Array>();

        for (uint32_t i = 0; i < list->Length(); i++) {
            Local<Value> e = list->Get(context, i).FromMaybe(Local<Value>());

            if (!e.IsEmpty() && e->IsString())
                excludes.patterns.push_back(ToSTLString(isolate, e.As<String>()));
        }
    }
}

void WriteLcovData(v8::Isolate* isolate, FILE* file)
{
    HandleScope handle_scope(isolate);
    debug::Coverage coverage = debug::Coverage::CollectPrecise(isolate);

    CovExcludes excludes;
    CollectCovExcludes(isolate, excludes);

    for (size_t i = 0; i < coverage.ScriptCount(); i++) {
        debug::Coverage::ScriptData script_data = coverage.GetScriptData(i);
        Local<debug::Script> script = script_data.GetScript();

        // Skip unnamed scripts.
        Local<String> name;
        if (!script->Name().ToLocal(&name))
            continue;

        std::string file_name = ToSTLString(isolate, name);

        // embedded modules carry a pseudo path ("internal/..."), and the tests
        // of a `--test` run are not what the report is about
        if (!path_isAbsolute(file_name) || excludes.isTest(file_name))
            continue;

        // a script whose source is gone (or never was there) cannot be mapped
        // to lines, so it is left out of the report
        std::vector<uint16_t> source;
        if (!ReadScriptSource(isolate, script, source))
            continue;

        std::vector<CovLine> lines;
        BuildLines(source, lines);

        // The compiled script carries the file one wrapper line further down,
        // and fibjs says so itself: the cjs loader hands V8 a line offset of -1
        // for the wrapper it writes in front of the source (an ES module gets
        // none), and the debug API applies that offset. It is exactly the
        // number of script lines before the file's first line.
        int32_t line_shift = -script->GetSourceLocation(0).GetLineNumber();
        if (line_shift < 0)
            line_shift = 0;

        // the lines of the file are the ones the report is about: neither the
        // wrapper in front of them nor the `\n});` behind them belongs to it,
        // and the count from the file keeps a file that shrank meanwhile in
        // bounds
        int32_t file_lines = ReadFileLineCount(file_name);
        int32_t known_lines = (int32_t)lines.size() - line_shift;

        if (file_lines > known_lines)
            file_lines = known_lines;

        if (file_lines <= 0)
            continue;

        std::vector<CovFunction> functions;
        std::vector<int32_t> branch_lines;
        std::vector<uint32_t> branch_counts;
        uint32_t wrapper_count = 0;
        bool has_wrapper = false;

        for (size_t j = 0; j < script_data.FunctionCount(); j++) {
            debug::Coverage::FunctionData function_data = script_data.GetFunctionData(j);

            // the function itself first, its blocks after it: the inner ranges
            // are applied last and decide the lines they cover completely
            std::vector<CovRange> ranges;

            ranges.push_back({ (uint32_t)function_data.StartOffset(),
                (uint32_t)function_data.EndOffset(), function_data.Count() });

            for (size_t k = 0; k < function_data.BlockCount(); k++) {
                debug::Coverage::BlockData block_data = function_data.GetBlockData(k);

                ranges.push_back({ (uint32_t)block_data.StartOffset(),
                    (uint32_t)block_data.EndOffset(), block_data.Count() });
            }

            for (size_t k = 0; k < ranges.size(); k++) {
                const CovRange& range = ranges[k];

                // the wrapper a script is compiled with has no position of its
                // own; the inner blocks of it may still have one
                if ((int32_t)range.start < 0 || range.end <= range.start)
                    continue;

                ApplyRange(lines, range);

                int32_t line = script->GetSourceLocation((int32_t)range.start).GetLineNumber();

                if (line < 0) {
                    // a range on the wrapper line says how often the module body
                    // ran, which node reports as a branch of the first line;
                    // fibjs compiles a module more than once on the way in (a
                    // probe for ESM syntax, the byte code cache), and those
                    // copies of the wrapper are one branch, taken from the run
                    // that executed it
                    if (wrapper_count < range.count) {
                        wrapper_count = range.count;
                        has_wrapper = true;
                    }

                    continue;
                }

                if (line >= file_lines)
                    continue;

                branch_lines.push_back(line + 1);
                branch_counts.push_back(range.count);
            }

            // a function without a position in the source, or without a name,
            // is one of those wrappers: a reader of the report cannot point at
            // it, and an empty name merges every anonymous function of the file
            // into a single entry once the logs are merged
            if (function_data.StartOffset() < 0)
                continue;

            Local<String> func_name;
            if (!function_data.Name().ToLocal(&func_name))
                continue;

            std::string func_str = ToSTLString(isolate, func_name);
            if (func_str.empty())
                continue;

            int32_t line = script->GetSourceLocation(function_data.StartOffset()).GetLineNumber();
            if (line < 0 || line >= file_lines)
                continue;

            functions.push_back({ func_str, line + 1, function_data.Count() });
        }

        fprintf(file, "SF:%s\n", LcovPath(file_name).c_str());

        uint32_t func_hit = 0;
        for (size_t j = 0; j < functions.size(); j++) {
            fprintf(file, "FN:%d,%s\n", functions[j].line, functions[j].name.c_str());

            if (functions[j].count > 0)
                func_hit++;
        }

        for (size_t j = 0; j < functions.size(); j++)
            fprintf(file, "FNDA:%d,%s\n", functions[j].count, functions[j].name.c_str());

        fprintf(file, "FNF:%d\n", (int32_t)functions.size());
        fprintf(file, "FNH:%d\n", (int32_t)func_hit);

        uint32_t branch_hit = 0;

        // the module body comes first, like node reports it
        if (has_wrapper) {
            fprintf(file, "BRDA:1,0,0,%d\n", (int32_t)wrapper_count);

            if (wrapper_count > 0)
                branch_hit++;
        }

        for (size_t j = 0; j < branch_lines.size(); j++) {
            fprintf(file, "BRDA:%d,%d,0,%d\n", branch_lines[j],
                (int32_t)(j + (has_wrapper ? 1 : 0)), branch_counts[j]);

            if (branch_counts[j] > 0)
                branch_hit++;
        }

        fprintf(file, "BRF:%d\n", (int32_t)branch_lines.size() + (has_wrapper ? 1 : 0));
        fprintf(file, "BRH:%d\n", (int32_t)branch_hit);

        uint32_t line_hit = 0;
        for (int32_t j = 0; j < file_lines; j++) {
            uint32_t count = lines[line_shift + j].count;

            fprintf(file, "DA:%d,%d\n", j + 1, count);

            if (count > 0)
                line_hit++;
        }

        fprintf(file, "LH:%d\n", (int32_t)line_hit);
        fprintf(file, "LF:%d\n", file_lines);

        fprintf(file, "end_of_record\n");
    }

    fclose(file);
}
}