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
#include "options.h"
#include "ifs/process.h"
#include "../../fs/match/path_match.h"

#include <set>

#ifdef _WIN32
#include <io.h>

static inline intptr_t cov_write(intptr_t fd, const char* data, size_t len)
{
    return _write((int32_t)fd, data, (unsigned int)len);
}

static inline intptr_t cov_fileno(FILE* file)
{
    return _fileno(file);
}
#else
#include <unistd.h>

static inline intptr_t cov_write(intptr_t fd, const char* data, size_t len)
{
    return ::write((int32_t)fd, data, len);
}

static inline intptr_t cov_fileno(FILE* file)
{
    return fileno(file);
}
#endif

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
// file's, and the lines of the file are the ones the report is about.
//
// This is the fallback of DeriveFileLineCount below: the rule there tells the
// file's lines from the source the script was compiled with, and this one is
// only needed when the rule cannot (a file whose text the script does not carry,
// e.g. the placeholder of the byte code cache). A file that is gone meanwhile
// is left out, like node's test coverage leaves it out.
inline int32_t ReadFileLineCount(const std::string& file_name)
{
    FILE* fp = fopen(file_name.c_str(), "rb");

    if (fp == nullptr)
        return -1;

    int32_t count = 0;
    bool last_is_eol = false;
    char buf[65536];
    size_t len;

    while ((len = fread(buf, 1, sizeof(buf), fp)) > 0) {
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == '\n') {
                count++;
                last_is_eol = true;
            } else
                last_is_eol = false;
        }
    }

    fclose(fp);

    if (!last_is_eol)
        count++;

    return count;
}

// The lines of the file the report is about, told from the source the script
// was compiled with instead of by reading the file again: every file of the
// graph has been read once to compile it, and reading all of them a second time
// to count their lines is the single most expensive part of a report of a large
// graph (it was ~40% of it). The compiled source describes the same file, and
// it is the one that ran, which is what the report is about.
//
// The loaders that wrap a file (see cjs_Loader::compile) put it between
// `arg_names + "\n"` and `"\n});"`, and tell V8 about that one line in front of
// it with a `-1` source offset. So the file's lines are the script's lines
// without the wrapper's two parts, minus one more when the file ends with a line
// ending: the wrapper's `\n` puts an empty line there, and an empty line is the
// line ending and not a line of the file. A script without that offset carries
// no wrapper and its lines are the file's.
//
// -1 marks the cases the rule cannot tell, and the caller reads the file then.
inline int32_t DeriveFileLineCount(const std::vector<uint16_t>& source, size_t line_count, int32_t line_shift)
{
    if (line_shift <= 0)
        return (int32_t)line_count;

    size_t n = source.size();

    // the wrapper's tail `"\n});"`, and the line ending of the file in front of
    // it when there is one
    if (n < 5 || source[n - 1] != ';' || source[n - 2] != ')' || source[n - 3] != '}' || source[n - 4] != '\n')
        return -1;

    int32_t count = (int32_t)line_count - line_shift - 1;

    if (source[n - 5] == '\n')
        count--;

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

// The working directory a report is named relative to, read once: a per file
// lookup would be a system call for every script of the graph.
struct CovDir {
    std::string dir;

    CovDir()
    {
        exlib::string cwd;

        if (process_base::cwd(cwd) < 0)
            return;

        dir.assign(cwd.c_str());

        for (size_t i = 0; i < dir.length(); i++) {
            if (dir[i] == '\\')
                dir[i] = '/';
        }

        if (dir.empty() || dir[dir.length() - 1] != '/')
            dir += '/';
    }

    // Whether the file is inside the working directory. Windows names a script
    // with the separators of the platform and does not tell apart the case of a
    // path, while the directory was folded above: both are folded here, and
    // here only, because a `\` is a character of a file name like any other
    // where it is not a separator.
    static bool isInside(const std::string& file_name, const std::string& dir)
    {
        size_t n = dir.length();

        if (file_name.length() < n)
            return false;

        for (size_t i = 0; i < n; i++) {
            char a = file_name[i];
            char b = dir[i];

#ifdef _WIN32
            if (a == '\\')
                a = '/';

            if (a >= 'A' && a <= 'Z')
                a += 'a' - 'A';

            if (b >= 'A' && b <= 'Z')
                b += 'a' - 'A';
#endif

            if (a != b)
                return false;
        }

        return true;
    }

    // The report is read next to the project it describes, so a file inside the
    // working directory is named relative to it. A file outside keeps its
    // absolute path: there is nothing sensible to be relative to.
    std::string relative(const std::string& file_name) const
    {
        if (dir.empty() || !isInside(file_name, dir))
            return file_name;

        std::string relative = file_name.substr(dir.length());

#ifdef _WIN32
        for (size_t i = 0; i < relative.length(); i++) {
            if (relative[i] == '\\')
                relative[i] = '/';
        }
#endif

        return relative;
    }
};

// A glob of `--cov-exclude` or one a `--test` run publishes, compiled once:
// matching a name against a string would compile the pattern again for every
// file of the graph, which costs more than the report itself.
struct CovPatterns {
    std::vector<std::unique_ptr<MinimatchPattern>> patterns;

    void add(const std::string& pattern)
    {
        patterns.push_back(std::make_unique<MinimatchPattern>(pattern, false));
    }

    bool empty() const
    {
        return patterns.empty();
    }

    // a file inside the working directory is matched under the name the report
    // carries, a file outside of it under its absolute path - the same string
    // for a file outside, and then it is matched once
    bool matches(const std::string& relative, const std::string& flat) const
    {
        bool same = (flat == relative);

        for (size_t i = 0; i < patterns.size(); i++) {
            if (patterns[i]->match(relative) || (!same && patterns[i]->match(flat)))
                return true;
        }

        return false;
    }
};

// The files the report leaves out: the ones a `--test` run must not count, and
// the ones the caller asked to drop.
struct CovExcludes {
    std::set<std::string> files;
    CovPatterns tests;
    CovPatterns excluded;

    bool hasPatterns() const
    {
        return !tests.empty() || !excluded.empty();
    }

    bool isExcluded(const std::string& relative, const std::string& flat) const
    {
        return excluded.matches(relative, flat);
    }

    bool isTest(const std::string& file_name, const std::string& relative, const std::string& flat) const
    {
        if (files.count(file_name) != 0)
            return true;

        return tests.matches(relative, flat);
    }

    // the patterns of the caller, in the `;` separated form options.cpp keeps
    void addExcluded(const exlib::string& list)
    {
        size_t start = 0;

        while (start < list.length()) {
            size_t end = list.find(';', start);

            if (end == exlib::string::npos)
                end = list.length();

            excluded.add(std::string(list.c_str() + start, end - start));
            start = end + 1;
        }
    }
};

// the absolute path with the separators a pattern is written with
inline std::string FlattenPath(const std::string& file_name)
{
    std::string flat = file_name;

    for (size_t i = 0; i < flat.length(); i++) {
        if (flat[i] == '\\')
            flat[i] = '/';
    }

    return flat;
}

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
                excludes.tests.add(ToSTLString(isolate, e.As<String>()));
        }
    }
}

// The report of one file is built in memory and handed to the log in one piece:
// every process of a run appends to the same file, and a line at a time lets the
// lines of two processes interleave and tear (the log is line oriented, so a
// torn line is a lost line). Formatting into a buffer also takes the per line
// cost of the report, which for a large graph is what the coverage switch costs
// at exit, from a `fprintf` call down to an `append`.
class CovLogWriter {
public:
    CovLogWriter(FILE* file)
        : m_fd(cov_fileno(file))
    {
        m_buffer.reserve(1024 * 1024);
    }

    // The report is line oriented and a line is a tag and a few numbers, so the
    // lines are appended directly: a `printf` per line costs more than the write
    // of a whole report of a large graph, and each call here is a plain copy.
    CovLogWriter& put(const char* text)
    {
        m_buffer.append(text);

        return *this;
    }

    CovLogWriter& comma()
    {
        m_buffer.push_back(',');

        return *this;
    }

    CovLogWriter& endLine()
    {
        m_buffer.push_back('\n');

        return *this;
    }

    CovLogWriter& number(uint32_t value)
    {
        char digits[10];
        int32_t len = 0;

        do {
            digits[len++] = (char)('0' + value % 10);
            value /= 10;
        } while (value != 0);

        while (len > 0)
            m_buffer.push_back(digits[--len]);

        return *this;
    }

    // called when a record is complete: the buffer holds no more than the
    // largest file of the graph. The record goes out in one write, because the
    // log is opened in append mode and the lines of two processes that append
    // at the same time must not interleave inside a record. stdio itself is
    // never used on the log, so the descriptor carries the whole record.
    void flush()
    {
        if (m_buffer.empty())
            return;

        const char* data = m_buffer.data();
        size_t left = m_buffer.size();

        while (left > 0) {
            intptr_t written = (intptr_t)cov_write(m_fd, data, left);

            if (written <= 0)
                break;

            data += written;
            left -= (size_t)written;
        }

        m_buffer.clear();
    }

private:
    intptr_t m_fd;
    std::string m_buffer;
};

void WriteLcovData(v8::Isolate* isolate, FILE* file)
{
    HandleScope handle_scope(isolate);
    debug::Coverage coverage = debug::Coverage::CollectPrecise(isolate);

    CovExcludes excludes;
    CollectCovExcludes(isolate, excludes);
    excludes.addExcluded(g_cov_exclude);

    CovDir dir;
    CovLogWriter out(file);

    // the report of a file is built in these, and they are kept across the files
    // of the graph: a graph of thousands of files is otherwise thousands of
    // allocations of the same buffers
    std::vector<uint16_t> source;
    std::vector<CovLine> lines;
    std::vector<CovFunction> functions;
    std::vector<int32_t> branch_lines;
    std::vector<uint32_t> branch_counts;

    for (size_t i = 0; i < coverage.ScriptCount(); i++) {
        debug::Coverage::ScriptData script_data = coverage.GetScriptData(i);
        Local<debug::Script> script = script_data.GetScript();

        // Skip unnamed scripts.
        Local<String> name;
        if (!script->Name().ToLocal(&name))
            continue;

        std::string file_name = ToSTLString(isolate, name);

        // embedded modules carry a pseudo path ("internal/...")
        if (!path_isAbsolute(file_name))
            continue;

        // the name the report carries, and - only when a pattern has to be
        // matched - the absolute path with the separators of the platform
        // flattened: the tests of a `--test` run are not what the report is
        // about, and the files of `--cov-exclude` are not wanted in it
        std::string relative = dir.relative(file_name);

        if (excludes.hasPatterns()) {
            std::string flat = FlattenPath(file_name);

            if (excludes.isTest(file_name, relative, flat) || excludes.isExcluded(relative, flat))
                continue;
        } else if (excludes.isTest(file_name, relative, relative))
            continue;

        // a script whose source is gone (or never was there) cannot be mapped
        // to lines, so it is left out of the report
        bool has_source = ReadScriptSource(isolate, script, source);
        if (!has_source)
            continue;

        lines.clear();
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
        // wrapper in front of them nor the `\n});` behind them belongs to it
        int32_t file_lines = DeriveFileLineCount(source, lines.size(), line_shift);

        // the rule cannot tell (a source that is not the file's text, or a file
        // with nothing in it): the file itself settles it, and a file that is
        // gone is left out here
        if (file_lines <= 0)
            file_lines = ReadFileLineCount(file_name);

        int32_t known_lines = (int32_t)lines.size() - line_shift;

        if (file_lines > known_lines)
            file_lines = known_lines;

        if (file_lines <= 0)
            continue;

        functions.clear();
        branch_lines.clear();
        branch_counts.clear();
        uint32_t wrapper_count = 0;
        bool has_wrapper = false;

        for (size_t j = 0; j < script_data.FunctionCount(); j++) {
            debug::Coverage::FunctionData function_data = script_data.GetFunctionData(j);

            // the function itself first, its blocks after it: the inner ranges
            // are applied last and decide the lines they cover completely
            size_t block_count = function_data.BlockCount();

            for (size_t k = 0; k <= block_count; k++) {
                uint32_t start, end, count;

                if (k == 0) {
                    start = (uint32_t)function_data.StartOffset();
                    end = (uint32_t)function_data.EndOffset();
                    count = function_data.Count();
                } else {
                    debug::Coverage::BlockData block_data = function_data.GetBlockData(k - 1);

                    start = (uint32_t)block_data.StartOffset();
                    end = (uint32_t)block_data.EndOffset();
                    count = block_data.Count();
                }

                // the wrapper a script is compiled with has no position of its
                // own; the inner blocks of it may still have one
                if ((int32_t)start < 0 || end <= start)
                    continue;

                ApplyRange(lines, { start, end, count });

                int32_t line = script->GetSourceLocation((int32_t)start).GetLineNumber();

                if (line < 0) {
                    // a range on the wrapper line says how often the module body
                    // ran, which node reports as a branch of the first line;
                    // fibjs compiles a module more than once on the way in (a
                    // probe for ESM syntax, the byte code cache), and those
                    // copies of the wrapper are one branch, taken from the run
                    // that executed it
                    if (wrapper_count < count) {
                        wrapper_count = count;
                        has_wrapper = true;
                    }

                    continue;
                }

                if (line >= file_lines)
                    continue;

                branch_lines.push_back(line + 1);
                branch_counts.push_back(count);
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

        out.put("SF:").put(relative.c_str()).endLine();

        uint32_t func_hit = 0;
        for (size_t j = 0; j < functions.size(); j++) {
            out.put("FN:").number(functions[j].line).comma().put(functions[j].name.c_str()).endLine();

            if (functions[j].count > 0)
                func_hit++;
        }

        for (size_t j = 0; j < functions.size(); j++)
            out.put("FNDA:").number(functions[j].count).comma().put(functions[j].name.c_str()).endLine();

        out.put("FNF:").number((uint32_t)functions.size()).endLine();
        out.put("FNH:").number(func_hit).endLine();

        uint32_t branch_hit = 0;

        // the module body comes first, like node reports it
        if (has_wrapper) {
            out.put("BRDA:1,0,0,").number(wrapper_count).endLine();

            if (wrapper_count > 0)
                branch_hit++;
        }

        for (size_t j = 0; j < branch_lines.size(); j++) {
            out.put("BRDA:").number(branch_lines[j]).comma()
                .number((uint32_t)(j + (has_wrapper ? 1 : 0)))
                .put(",0,")
                .number(branch_counts[j])
                .endLine();

            if (branch_counts[j] > 0)
                branch_hit++;
        }

        out.put("BRF:").number((uint32_t)branch_lines.size() + (has_wrapper ? 1 : 0)).endLine();
        out.put("BRH:").number(branch_hit).endLine();

        uint32_t line_hit = 0;
        for (int32_t j = 0; j < file_lines; j++) {
            uint32_t count = lines[line_shift + j].count;

            out.put("DA:").number((uint32_t)(j + 1)).comma().number(count).endLine();

            if (count > 0)
                line_hit++;
        }

        out.put("LH:").number(line_hit).endLine();
        out.put("LF:").number((uint32_t)file_lines).endLine();

        out.put("end_of_record").endLine();

        // one record, one write: a reader of a log that several processes
        // appended to can never see half a record
        out.flush();
    }

    fclose(file);
}
}