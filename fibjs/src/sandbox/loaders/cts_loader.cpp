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
#include "ts_cache.h"
#include "../../util/ts_strip/ts_strip.h"

namespace fibjs {

result_t cts_Loader::ts_compile(Isolate* isolate, Buffer_base* src, obj_ptr<Buffer_base>& retVal)
{
    Buffer* buf = Buffer::Cast(src);
    
    // Content key: identical sources share one entry, wherever they live
    size_t key = ts_cache_key(buf->data(), buf->length());
    
    // Try to get cached JS first (skips small files automatically)
    if (ts_cache_get(key, buf->length(), retVal))
        return 0;

    // Cache miss: strip TypeScript types. The source buffer belongs to the isolate's
    // file cache (every sandbox in this process shares it), so the stripper works on
    // a copy of its own and hands back the result.
    try {
        retVal = ts_strip::stripToBuffer(buf->data(), buf->length());
    } catch (const std::exception& e) {
        return CHECK_ERROR(Runtime::setError(e.what()));
    }

    // Async save to cache (fire and forget, zero copy with ref counting)
    ts_cache_set(key, retVal);

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