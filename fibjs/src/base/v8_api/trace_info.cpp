/*
 * trace_info.cpp
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

exlib::string traceInfo(Isolate* isolate, int32_t deep, void* entry_fp, void* handle)
{
    i::Isolate* v8_isolate = (i::Isolate*)isolate;
    i::ThreadLocalTop tt;

    tt.c_entry_fp_ = (i::Address)entry_fp;
    tt.handler_ = (i::Address)handle;

    i::JavaScriptStackFrameIterator it(v8_isolate, &tt);

    exlib::string strBuffer;
    bool bFirst = true;

    for (; !it.done() && deep-- > 0; it.Advance()) {
        i::JavaScriptFrame* frame = it.frame();
        std::vector<i::FrameSummary> frames;

        frame->Summarize(&frames);

        const i::FrameSummary::JavaScriptFrameSummary& summ = frames[0].AsJavaScript();
        i::Handle<i::Script> script = i::Cast<i::Script>(summ.script());
        if (script->type() == i::Script::Type::kNormal) {
            strBuffer.append(bFirst ? "    at " : "\n    at ");
            bFirst = false;

            String::Utf8Value funcname(isolate, Utils::ToLocal(i::JSFunction::GetName(v8_isolate, summ.function())));
            if (*funcname) {
                strBuffer.append(*funcname);
                strBuffer.append(" (", 2);
            }

            int32_t line_number = 0;
            int32_t column_number = 0;
            i::Script::PositionInfo info;
            bool valid_pos = i::Script::GetPositionInfo(script, summ.SourcePosition(),
                &info, i::Script::OffsetFlag::kWithOffset);

            if (valid_pos) {
                line_number = info.line + 1;
                column_number = info.column + 1;
            }

            i::Handle<i::Object> name(script->name(), v8_isolate);
            String::Utf8Value filename(isolate, Utils::ToLocal(name));

            char numStr[32];

            if (*filename) {
                strBuffer.append(*filename);
                strBuffer.append(1, ':');
            } else
                strBuffer.append("[eval]:", 7);

            snprintf(numStr, sizeof(numStr), "%d", line_number);
            strBuffer.append(numStr);
            strBuffer.append(1, ':');
            snprintf(numStr, sizeof(numStr), "%d", column_number);
            strBuffer.append(numStr);

            if (**funcname)
                strBuffer.append(1, ')');
        }
    }
    return strBuffer;
}

exlib::string traceInfo(Isolate* isolate, int32_t deep)
{
    i::Isolate* v8_isolate = (i::Isolate*)isolate;
    return traceInfo(isolate, deep, (void*)*v8_isolate->c_entry_fp_address(), (void*)*v8_isolate->handler_address());
}

}