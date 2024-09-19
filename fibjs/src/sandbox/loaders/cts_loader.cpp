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
#include "ifs/util.h"

namespace fibjs {

result_t cts_Loader::ts_compile(Isolate* isolate, Buffer_base* src, obj_ptr<Buffer_base>& retVal)
{
    v8::Local<v8::Function> _aramo;

    if (!isolate->m_amaro.IsEmpty())
        _aramo = isolate->m_amaro.Get(isolate->m_isolate);
    else {
        v8::Local<v8::Value> _amaro_module;
        isolate->m_topSandbox->require("amaro", "/builtin", _amaro_module);

        _aramo = _amaro_module.As<v8::Object>()->Get(isolate->context(), isolate->NewString("transform")).FromMaybe(v8::Local<v8::Value>()).As<v8::Function>();
        isolate->m_amaro.Reset(isolate->m_isolate, _aramo);
    }

    exlib::string _src;
    src->toString(_src);

    v8::Local<v8::Value> __src = isolate->NewString(_src);
    v8::Local<v8::Value> _dest = _aramo->Call(isolate->context(), isolate->context()->Global(), 1, &__src).FromMaybe(v8::Local<v8::Value>());
    if (_dest.IsEmpty())
        return CALL_E_JAVASCRIPT;

    v8::Local<v8::Value> _code = _dest.As<v8::Object>()->Get(isolate->context(), isolate->NewString("code")).FromMaybe(v8::Local<v8::Value>());

    v8::String::Utf8Value _dest_str = v8::String::Utf8Value(isolate->m_isolate, _code);
    retVal = new Buffer(*_dest_str, _dest_str.length());

    return 0;
}

result_t cts_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();

    obj_ptr<Buffer_base> _src;
    result_t hr = ts_compile(isolate, src, _src);
    if (hr < 0)
        return hr;

    return cjs_Loader::compile(ctx, _src, name, arg_names, script);
}
}