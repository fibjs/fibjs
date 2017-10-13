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
#include "exlib/include/qstring.h"

namespace fibjs {

void InvokeApiInterruptCallbacks(v8::Isolate* isolate)
{
    v8::internal::Isolate* v8_isolate = (v8::internal::Isolate*)isolate;
    v8_isolate->InvokeApiInterruptCallbacks();
}

struct V8FrameInfo {
    void* entry_fp;
    void* handle;
};

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
        v8::String::Utf8Value funcname(v8::Utils::ToLocal(summ.FunctionName()));

        strBuffer.append(bFirst ? "    at " : "\n    at ");
        bFirst = false;
        if (**funcname) {
            strBuffer.append(*funcname);
            strBuffer.append(" (", 2);
        }

        v8::internal::Handle<v8::internal::Script> script = v8::internal::Handle<v8::internal::Script>::cast(summ.script());

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
    return strBuffer;
}

exlib::string traceInfo(v8::Isolate* isolate, int32_t deep)
{
    v8::internal::Isolate* v8_isolate = (v8::internal::Isolate*)isolate;
    return traceInfo(isolate, deep, *v8_isolate->c_entry_fp_address(), *v8_isolate->handler_address());
}
}