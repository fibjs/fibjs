/*
 * traceInfo.cpp
 *
 *  Created on: Oct 1, 2017
 *      Author: lion
 */

#ifdef _WIN32
#pragma warning(disable : 4800)
#endif

#include "v8.h"
#include "v8/src/utils.h"
#include "v8/src/api.h"
#include "v8/src/isolate.h"
#include "v8/src/frames.h"
#include "v8/src/frames-inl.h"
#include "src/debug/debug-interface.h"

#include "exlib/include/qstring.h"
#include "v8_api.h"
#include <fstream>

namespace fibjs {

V8FrameInfo save_fi(v8::Isolate* isolate)
{
    v8::internal::Isolate* v8_isolate = (v8::internal::Isolate*)isolate;
    V8FrameInfo fi;

    fi.entry_fp = *v8_isolate->c_entry_fp_address();
    fi.handle = *v8_isolate->handler_address();

    return fi;
}

exlib::string traceInfo(v8::Isolate* isolate, int32_t deep, void* entry_fp, void* handle)
{
    v8::internal::Isolate* v8_isolate = (v8::internal::Isolate*)isolate;
    v8::internal::ThreadLocalTop tt;

    tt.c_entry_fp_ = (v8::internal::Address)entry_fp;
    tt.handler_ = (v8::internal::Address)handle;

    v8::internal::JavaScriptFrameIterator it(v8_isolate, &tt);

    exlib::string strBuffer;
    bool bFirst = true;

    for (; !it.done() && deep-- > 0; it.Advance()) {
        v8::internal::JavaScriptFrame* frame = it.frame();
        std::vector<v8::internal::FrameSummary> frames;

        frame->Summarize(&frames);

        const v8::internal::FrameSummary::JavaScriptFrameSummary& summ = frames[0].AsJavaScript();
        v8::internal::Handle<v8::internal::Script> script = v8::internal::Handle<v8::internal::Script>::cast(summ.script());
        if (script->type() == v8::internal::Script::TYPE_NORMAL) {
            strBuffer.append(bFirst ? "    at " : "\n    at ");
            bFirst = false;

            v8::String::Utf8Value funcname(v8::Utils::ToLocal(summ.FunctionName()));
            if (**funcname) {
                strBuffer.append(*funcname);
                strBuffer.append(" (", 2);
            }

            int32_t line_number = 0;
            int32_t column_number = 0;
            v8::internal::Script::PositionInfo info;
            bool valid_pos = v8::internal::Script::GetPositionInfo(script, summ.SourcePosition(),
                &info, v8::internal::Script::WITH_OFFSET);

            if (valid_pos) {
                line_number = info.line + 1;
                column_number = info.column + 1;
            }

            v8::internal::Handle<v8::internal::Object> name(script->name(), v8_isolate);
            v8::String::Utf8Value filename(v8::Utils::ToLocal(name));

            char numStr[32];

            if (*filename) {
                strBuffer.append(*filename);
                strBuffer.append(1, ':');
            } else
                strBuffer.append("[eval]:", 7);

            sprintf(numStr, "%d", line_number);
            strBuffer.append(numStr);
            strBuffer.append(1, ':');
            sprintf(numStr, "%d", column_number);
            strBuffer.append(numStr);

            if (**funcname)
                strBuffer.append(1, ')');
        }
    }
    return strBuffer;
}

exlib::string traceInfo(v8::Isolate* isolate, int32_t deep)
{
    v8::internal::Isolate* v8_isolate = (v8::internal::Isolate*)isolate;
    return traceInfo(isolate, deep, *v8_isolate->c_entry_fp_address(), *v8_isolate->handler_address());
}

void beginCoverage(v8::Isolate* isolate)
{
    v8::debug::Coverage::SelectMode(isolate, v8::debug::Coverage::kBlockCount);
}

inline std::string ToSTLString(v8::Isolate* isolate, v8::Local<v8::String> v8_str)
{
    v8::String::Utf8Value utf8(isolate, v8_str);
    return *utf8;
}

inline int LineFromOffset(v8::Local<v8::debug::Script> script, int offset)
{
    v8::debug::Location location = script->GetSourceLocation(offset);
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

inline void WriteLcovDataForNamedRange(std::ostream& sink,
    std::vector<uint32_t>& lines, std::string name,
    int start_line, int end_line, uint32_t count)
{
    WriteLcovDataForRange(lines, start_line, end_line, count);
    sink << "FN:" << start_line + 1 << "," << name << std::endl;
    sink << "FNDA:" << count << "," << name << std::endl;
}

void WriteLcovData(v8::Isolate* isolate, exlib::string file)
{
    v8::HandleScope handle_scope(isolate);

    v8::debug::Coverage::SelectMode(isolate, v8::debug::Coverage::kBestEffort);

    v8::debug::Coverage coverage = v8::debug::Coverage::CollectPrecise(isolate);
    std::ofstream sink(file.c_str(), std::ofstream::app);
    for (size_t i = 0; i < coverage.ScriptCount(); i++) {
        v8::debug::Coverage::ScriptData script_data = coverage.GetScriptData(i);
        v8::Local<v8::debug::Script> script = script_data.GetScript();
        // Skip unnamed scripts.
        v8::Local<v8::String> name;
        if (!script->Name().ToLocal(&name))
            continue;
        std::string file_name = ToSTLString(isolate, name);
        // Skip scripts not backed by a file.
        if (!std::ifstream(file_name).good())
            continue;
        sink << "SF:";
        sink << file_name << std::endl;
        std::vector<uint32_t> lines;
        for (size_t j = 0; j < script_data.FunctionCount(); j++) {
            v8::debug::Coverage::FunctionData function_data = script_data.GetFunctionData(j);

            // Write function stats.
            {
                v8::debug::Location start = script->GetSourceLocation(function_data.StartOffset());
                v8::debug::Location end = script->GetSourceLocation(function_data.EndOffset());
                int start_line = start.GetLineNumber();
                int end_line = end.GetLineNumber();
                uint32_t count = function_data.Count();

                v8::Local<v8::String> name;
                std::stringstream name_stream;
                if (function_data.Name().ToLocal(&name)) {
                    name_stream << ToSTLString(isolate, name);
                } else {
                    name_stream << "<" << start_line + 1 << "-";
                    name_stream << start.GetColumnNumber() << ">";
                }

                WriteLcovDataForNamedRange(sink, lines, name_stream.str(), start_line,
                    end_line, count);
            }

            // Process inner blocks.
            for (size_t k = 0; k < function_data.BlockCount(); k++) {
                v8::debug::Coverage::BlockData block_data = function_data.GetBlockData(k);
                int start_line = LineFromOffset(script, block_data.StartOffset());
                int end_line = LineFromOffset(script, block_data.EndOffset() - 1);
                uint32_t count = block_data.Count();
                WriteLcovDataForRange(lines, start_line, end_line, count);
            }
        }
        // Write per-line coverage. LCOV uses 1-based line numbers.
        for (size_t i = 0; i < lines.size(); i++) {
            sink << "DA:" << (i + 1) << "," << lines[i] << std::endl;
        }
        sink << "end_of_record" << std::endl;
    }
}
}