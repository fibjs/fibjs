/*
 * js_loader.cpp
 *
 *  Created on: Jun 10, 2023
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "Fiber.h"
#include "loaders.h"
#include "ifs/util.h"
namespace fibjs {

static v8::MaybeLocal<v8::Module> resolveModule(
    v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
    v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer)
{
    Isolate* isolate = Isolate::current(context);

    for (int i = 0; i < import_assertions->Length(); i++) {
        v8::Local<v8::String> v8_assertion_key = import_assertions->Get(context, i).As<v8::String>();
        puts(isolate->toString(v8_assertion_key).c_str());
    }

    ThrowError(isolate->NewString("import is not supported in sandbox"));
    return v8::MaybeLocal<v8::Module>();
}

class async_import_data {
public:
    obj_ptr<SandBox> m_sb;
    exlib::string id;
    exlib::string base;
    v8::Global<v8::Promise::Resolver> m_resolver;
};

static result_t sync_import(async_import_data* data)
{
    JSFiber::EnterJsScope s;
    Isolate* isolate = data->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Value> retVal;
    puts(data->id.c_str());
    puts(data->base.c_str());
    result_t hr = data->m_sb->run_module(data->id, data->base, retVal);
    if (hr < 0)
        data->m_resolver.Get(isolate->m_isolate)->Reject(context, FillError(hr));
    else
        data->m_resolver.Get(isolate->m_isolate)->Resolve(context, retVal);

    delete data;
    return 0;
}

v8::MaybeLocal<v8::Promise> SandBox::async_import(exlib::string id, exlib::string base)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    async_import_data* data = new async_import_data();
    data->m_sb = this;
    data->id = id;
    os_dirname(base, data->base);

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(context).FromMaybe(v8::Local<v8::Promise::Resolver>());
    data->m_resolver.Reset(isolate->m_isolate, resolver);

    syncCall(isolate, sync_import, data);

    return resolver->GetPromise();
}

v8::MaybeLocal<v8::Promise> SandBox::ImportModuleDynamically(v8::Local<v8::Context> context,
    v8::Local<v8::Data> host_defined_options, v8::Local<v8::Value> resource_name,
    v8::Local<v8::String> specifier, v8::Local<v8::FixedArray> import_assertions)
{
    Isolate* isolate = Isolate::current(context);
    v8::Local<v8::PrimitiveArray> _host_options = host_defined_options.As<v8::PrimitiveArray>();
    uint32_t id = _host_options->Get(isolate->m_isolate, 0)->Uint32Value(context).FromMaybe(0);

    v8::String::Utf8Value _resource_name(isolate->m_isolate, resource_name);
    v8::String::Utf8Value _specifier(isolate->m_isolate, specifier);

    return isolate->m_sandboxes.find(id)->second->async_import(*_specifier, *_resource_name);
}

result_t mjs_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::String> soname = isolate->NewString(name);

    exlib::string strScript;
    src->toString(strScript);

    TryCatch try_catch;

    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
    pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, ctx->m_sb->m_id));
    v8::ScriptOrigin so_origin(isolate->m_isolate, soname, 0, 0, false,
        -1, v8::Local<v8::Value>(), false, false, true, pargs);

    v8::ScriptCompiler::Source source(isolate->NewString(strScript), so_origin);
    v8::Local<v8::Module> module = v8::ScriptCompiler::CompileModule(isolate->m_isolate, &source)
                                       .FromMaybe(v8::Local<v8::Module>());

    if (module.IsEmpty())
        return throwSyntaxError(try_catch);

    module->InstantiateModule(context, resolveModule).IsJust();

    v8::Local<v8::Value> promise = module->Evaluate(context).FromMaybe(v8::Local<v8::Value>());
    if (promise.IsEmpty()) {
        try_catch.ReThrow();
        return CALL_E_JAVASCRIPT;
    }

    v8::Local<v8::Value> result = isolate->WaitPromise(promise);
    if (result.IsEmpty()) {
        try_catch.ReThrow();
        return CALL_E_JAVASCRIPT;
    }

    v8::Local<v8::Object>::Cast(args[2])->Set(context, isolate->NewString("exports"), module->GetModuleNamespace()).IsJust();

    return 0;
}
}