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

using namespace v8;

namespace fibjs {

bool path_isAbsolute(exlib::string path);

void beginCoverage(Isolate* isolate)
{
    debug::Coverage::SelectMode(isolate, debug::CoverageMode::kBlockCount);
}

void pauseCoverage(Isolate* isolate)
{
    debug::Coverage::SelectMode(isolate, debug::CoverageMode::kBestEffort);
}

inline std::string ToSTLString(Isolate* isolate, Local<String> v8_str)
{
    String::Utf8Value utf8(isolate, v8_str);
    return *utf8;
}

inline int LineFromOffset(Local<debug::Script> script, int offset)
{
    debug::Location location = script->GetSourceLocation(offset);
    return location.GetLineNumber();
}

inline void WriteLcovDataForRange(std::vector<uint32_t>& lines, int start_line,
    int end_line, uint32_t count)
{
    // Ensure space in the array.
    lines.resize(std::max(static_cast<size_t>(end_line + 1), lines.size()), 0);
    // Boundary lines could be shared between two functions with different
    // invocation counts. Take the maximum.
    lines[start_line] = std::max(lines[start_line], count);
    lines[end_line] = std::max(lines[end_line], count);
    // Invocation counts for non-boundary lines are overwritten.
    for (int k = start_line + 1; k < end_line; k++)
        lines[k] = count;
}

inline void WriteLcovDataForNamedRange(FILE* sink,
    std::vector<uint32_t>& lines, std::string name, int start_line, int end_line, uint32_t count)
{
    WriteLcovDataForRange(lines, start_line, end_line, count);
    fprintf(sink, "FN:%d,%s\n", start_line + 1, name.c_str());
    fprintf(sink, "FNDA:%d,%s\n", count, name.c_str());
}

void WriteLcovData(Isolate* isolate, FILE* file)
{
    HandleScope handle_scope(isolate);
    debug::Coverage coverage = debug::Coverage::CollectPrecise(isolate);

    for (size_t i = 0; i < coverage.ScriptCount(); i++) {
        debug::Coverage::ScriptData script_data = coverage.GetScriptData(i);
        Local<debug::Script> script = script_data.GetScript();
        // Skip unnamed scripts.
        Local<String> name;
        if (!script->Name().ToLocal(&name))
            continue;
        std::string file_name = ToSTLString(isolate, name);
        if (!path_isAbsolute(file_name))
            continue;

        fprintf(file, "SF:%s\n", file_name.c_str());
        std::vector<uint32_t> lines;
        for (size_t j = 0; j < script_data.FunctionCount(); j++) {
            debug::Coverage::FunctionData function_data = script_data.GetFunctionData(j);

            // Write function stats.
            {
                debug::Location start = script->GetSourceLocation(function_data.StartOffset());
                debug::Location end = script->GetSourceLocation(function_data.EndOffset());
                int start_line = start.GetLineNumber();
                int end_line = end.GetLineNumber();
                uint32_t count = function_data.Count();

                Local<String> name;
                std::stringstream name_stream;
                if (function_data.Name().ToLocal(&name)) {
                    name_stream << ToSTLString(isolate, name);
                } else {
                    name_stream << "<" << start_line + 1 << "-";
                    name_stream << start.GetColumnNumber() << ">";
                }

                WriteLcovDataForNamedRange(file, lines, name_stream.str(), start_line,
                    end_line, count);
            }

            // Process inner blocks.
            for (size_t k = 0; k < function_data.BlockCount(); k++) {
                debug::Coverage::BlockData block_data = function_data.GetBlockData(k);
                int start_line = LineFromOffset(script, block_data.StartOffset());
                int end_line = LineFromOffset(script, block_data.EndOffset() - 1);
                uint32_t count = block_data.Count();
                WriteLcovDataForRange(lines, start_line, end_line, count);
            }
        }
        // Write per-line coverage. LCOV uses 1-based line numbers.
        for (size_t i = 0; i < lines.size(); i++)
            fprintf(file, "DA:%d,%d\n", (int32_t)(i + 1), lines[i]);

        fprintf(file, "end_of_record\n");
    }

    fclose(file);
}
}