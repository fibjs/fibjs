/*
 * json_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/encoding.h"
#include "loaders.h"

namespace fibjs {

void _json_loader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    obj_ptr<Buffer_base> src = Buffer_base::getInstance(args.Data());

    exlib::string strScript;
    src->toString(strScript);
    v8::Local<v8::Value> v;

    result_t hr = json_base::decode(strScript, v);
    if (hr < 0) {
        ThrowResult(hr);
        return;
    }

    v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(args[5]);
    module->Set(context, NewString(isolate, "exports"), v);
}

result_t JsonLoader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Function> func = isolate->NewFunction("json_loader", _json_loader, src->wrap());
    if (func.IsEmpty())
        return CHECK_ERROR(Runtime::setError("function alloc error."));

    v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(args[5]);
    module->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("entry")),
        func);

    v8::Local<v8::Object> glob = context->Global();
    v8::Local<v8::Value> v;

    func->Call(context, glob, (int32_t)args.size(), args.data()).ToLocal(&v);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}
}