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

static v8::MaybeLocal<v8::Module> _resolveModule(
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

class promise_data : public object_base {
public:
    static promise_data* getInstance(v8::Local<v8::Value> o)
    {
        return (promise_data*)object_base::getInstance(o);
    }

public:
    exlib::string id;
    v8::Global<v8::Object> mods;
    v8::Global<v8::Object> mod;
    v8::Global<v8::Module> module;
    v8::Global<v8::Promise::Resolver> m_resolver;
};

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    obj_ptr<promise_data> datas = promise_data::getInstance(args.Data());

    v8::Local<v8::Object> mod = datas->mod.Get(isolate->m_isolate);
    v8::Local<v8::Module> module = datas->module.Get(isolate->m_isolate);
    v8::Local<v8::Promise::Resolver> resolver = datas->m_resolver.Get(isolate->m_isolate);

    v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
    mod->DeletePrivate(context, strPendding).IsJust();

    v8::Local<v8::Value> result = module->GetModuleNamespace();
    mod->Set(context, isolate->NewString("exports"), result).IsJust();
    resolver->Resolve(context, result).IsJust();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Isolate* isolate = Isolate::current(args);
    v8::Local<v8::Context> context = isolate->context();
    obj_ptr<promise_data> datas = promise_data::getInstance(args.Data());

    v8::Local<v8::Object> mods = datas->mods.Get(isolate->m_isolate);
    v8::Local<v8::Object> mod = datas->mod.Get(isolate->m_isolate);
    v8::Local<v8::Module> module = datas->module.Get(isolate->m_isolate);
    v8::Local<v8::Promise::Resolver> resolver = datas->m_resolver.Get(isolate->m_isolate);

    mod->Delete(context, isolate->NewString("exports")).IsJust();
    mods->Delete(context, isolate->NewString(datas->id)).IsJust();

    resolver->Reject(context, args[0]).IsJust();
}

v8::MaybeLocal<v8::Promise> SandBox::async_import(exlib::string id, exlib::string base)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> _context = isolate->context();

    result_t hr;
    obj_ptr<Buffer_base> data;
    v8::Local<v8::Object> mod;

    exlib::string pname;
    path_base::dirname(base, pname);

    hr = resolve(pname, id, data, mod);
    if (hr < 0) {
        ThrowResult(hr);
        return v8::MaybeLocal<v8::Promise>();
    } else if (!IsEmpty(mod)) {
        v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());
        v8::Local<v8::Value> v;

        hr = wait_module(mod, v);
        resolver->Resolve(_context, v).IsJust();

        return resolver->GetPromise();
    }

    Context context(this, id);

    v8::Local<v8::String> strExports = isolate->NewString("exports");
    v8::Local<v8::String> strModule = isolate->NewString(id);
    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);

    mod = v8::Object::New(isolate->m_isolate);
    mod->Set(_context, isolate->NewString("id"), strModule).IsJust();
    mod->Set(_context, strExports, exports).IsJust();
    mod->Set(_context, isolate->NewString("filename"), strModule).IsJust();
    mod->Set(_context, isolate->NewString("require"), context.m_fnRequest).IsJust();
    mod->Set(_context, isolate->NewString("run"), context.m_fnRun).IsJust();

    v8::Local<v8::Object> _mods = mods();
    _mods->Set(_context, strModule, mod).IsJust();

    exlib::string strScript;
    data->toString(strScript);

    v8::Local<v8::Module> module;
    exlib::string exception;

    {
        TryCatch try_catch;

        v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
        pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, m_id));
        v8::ScriptOrigin so_origin(isolate->m_isolate, strModule, 0, 0, false,
            -1, v8::Local<v8::Value>(), false, false, true, pargs);

        v8::ScriptCompiler::Source source(isolate->NewString(strScript), so_origin);
        module = v8::ScriptCompiler::CompileModule(isolate->m_isolate, &source).FromMaybe(v8::Local<v8::Module>());
        if (module.IsEmpty())
            exception = GetException(try_catch, 0, false, false);
    }

    if (module.IsEmpty()) {
        ThrowError(exception);
        return v8::MaybeLocal<v8::Promise>();
    }

    module->InstantiateModule(_context, _resolveModule).IsJust();

    TryCatch try_catch;
    v8::Local<v8::Value> result = module->Evaluate(_context).FromMaybe(v8::Local<v8::Value>());
    if (result.IsEmpty()) {
        try_catch.ReThrow();
        return v8::MaybeLocal<v8::Promise>();
    }

    v8::Local<v8::Promise::Resolver> resolver = v8::Promise::Resolver::New(_context).FromMaybe(v8::Local<v8::Promise::Resolver>());

    obj_ptr<promise_data> datas = new promise_data();

    datas->id = id;
    datas->mods.Reset(isolate->m_isolate, mods());
    datas->mod.Reset(isolate->m_isolate, mod);
    datas->module.Reset(isolate->m_isolate, module);
    datas->m_resolver.Reset(isolate->m_isolate, resolver);

    v8::Local<v8::Promise> promise = v8::Local<v8::Promise>::Cast(result);

    v8::Local<v8::Private> strPendding = v8::Private::ForApi(isolate->m_isolate, isolate->NewString("pendding"));
    mod->SetPrivate(_context, strPendding, resolver->GetPromise()).IsJust();

    promise->Then(_context,
               v8::Function::New(_context, promise_then, datas->wrap(isolate)).ToLocalChecked(),
               v8::Function::New(_context, promise_catch, datas->wrap(isolate)).ToLocalChecked())
        .FromMaybe(v8::Local<v8::Promise>());

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

    module->InstantiateModule(context, _resolveModule).IsJust();

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