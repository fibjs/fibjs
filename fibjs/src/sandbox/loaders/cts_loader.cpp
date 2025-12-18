/*
 * cts_loader.cpp
 *
 *  Created on: Sep 19, 2024
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "loaders.h"
#include "../../util/ts_strip/ts_strip.h"

namespace fibjs {

result_t cts_Loader::ts_compile(Isolate* isolate, Buffer_base* src, obj_ptr<Buffer_base>& retVal)
{
    Buffer* buf = Buffer::Cast(src);
    
    // Strip TypeScript types in-place on the buffer data
    try {
        ts_strip::stripInPlace(buf->data(), buf->length());
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    }

    retVal = src;

    return 0;
}

result_t cts_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();

    // Strip TypeScript types and compile as JS directly (fast path)
    obj_ptr<Buffer_base> js_src;
    result_t hr = ts_compile(isolate, src, js_src);
    if (hr < 0)
        return hr;

    return cjs_Loader::compile(ctx, js_src, name, arg_names, script);
}
}