/*
 * base_loader.cpp
 *
 *  Created on: Jun 4, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/global.h"
#include "ifs/util.h"
#include "path.h"
#include "Event.h"

namespace fibjs {

const char* SandBox::script_args = "(async function(__filename,__dirname,require,run,exports,module,__argv){";
const char* SandBox::worker_args = "(async function(__filename,__dirname,require,run,exports,module,Master){";
const char* SandBox::module_args = "(async function(__filename,__dirname,require,run,exports,module){";
const char* SandBox::base_args = "(async function(__filename,__dirname,require,run,exports,module";

result_t SandBox::ExtLoader::run_script(Context* ctx, Buffer_base* src, exlib::string name,
    std::vector<arg>& extarg, bool is_main)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::String> strRequire = isolate->NewString("require");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Object> module = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);

    module->Set(context, strExports, exports);
    module->Set(context, strRequire, ctx->m_fnRequest);

    if (is_main)
        ctx->m_fnRequest->Set(context, isolate->NewString("main"), module);

    return run_module(ctx, src, name, module, exports, extarg);
}

result_t SandBox::ExtLoader::run_module(Context* ctx, Buffer_base* src, exlib::string name,
    v8::Local<v8::Object> module, v8::Local<v8::Object> exports,
    std::vector<arg>& extarg)
{
    Isolate* isolate = ctx->m_sb->holder();

    std::vector<v8::Local<v8::Value>> args;
    args.resize(extarg.size() + 6);

    exlib::string pname;
    path_base::dirname(name, pname);

    args[0] = isolate->NewString(name);
    args[1] = isolate->NewString(pname);
    args[2] = ctx->m_fnRequest;
    args[3] = ctx->m_fnRun;
    args[4] = exports;
    args[5] = module;

    int32_t i;
    exlib::string arg_names(base_args);

    for (i = 0; i < (int32_t)extarg.size(); i++) {
        arg& a = extarg[i];
        arg_names.append(1, ',');
        arg_names.append(a.first);
        args[i + 6] = a.second;
    }

    arg_names.append("){", 2);

    return run(ctx, src, name, arg_names, args);
}

static void promise_then(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    _data->Set(context, NewString(isolate, "_result"), args[0]);

    obj_ptr<Event_base> ev = Event_base::getInstance(JSValue(_data->Get(context, NewString(isolate, "_ev"))));
    ev->set();
}

static void promise_catch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Local<v8::Object> _data = v8::Local<v8::Object>::Cast(args.Data());

    _data->Set(context, NewString(isolate, "_error"), args[0]);

    obj_ptr<Event_base> ev = Event_base::getInstance(JSValue(_data->Get(context, NewString(isolate, "_ev"))));
    ev->set();
}

result_t SandBox::ExtLoader::run(Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    result_t hr;
    v8::Local<v8::Script> script;

    hr = compile(ctx, src, name, arg_names, script);
    if (hr < 0)
        return hr;

    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Value> v = script->Run(context).ToLocalChecked();
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(v);

    v8::Local<v8::Object> module = v8::Local<v8::Object>::Cast(args[5]);
    module->SetPrivate(context, v8::Private::ForApi(isolate->m_isolate, isolate->NewString("entry")),
        func);

    v8::Local<v8::Object> glob = context->Global();

    func->Call(context, glob, (int32_t)args.size(), args.data()).ToLocal(&v);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    if (func->IsAsyncFunction()) {
        v8::Local<v8::Promise> _promise = v8::Local<v8::Promise>::Cast(v);
        v8::Local<v8::Function> _then_func;
        v8::Local<v8::Function> _catch_func;

        obj_ptr<Event_base> ev = new Event();
        v8::Local<v8::Object> _data = v8::Object::New(isolate->m_isolate);

        _data->Set(context, isolate->NewString("_ev"), ev->wrap());

        _then_func = isolate->NewFunction("promise_then", promise_then, _data);
        if (_then_func.IsEmpty()) {
            ThrowError("function alloc error.");
            return CALL_E_JAVASCRIPT;
        }
        _catch_func = isolate->NewFunction("promise_catch", promise_catch, _data);
        if (_catch_func.IsEmpty()) {
            ThrowError("function alloc error.");
            return CALL_E_JAVASCRIPT;
        }

        _promise->Then(isolate->context(), _then_func);
        _promise->Catch(isolate->context(), _catch_func);

        ev->wait();

        JSValue error = _data->Get(context, isolate->NewString("_error"));

        if (!error.IsEmpty() && !error->IsUndefined() && !error->IsNull()) {
            isolate->m_isolate->ThrowException(error);
            return CALL_E_JAVASCRIPT;
        }
    }
    return 0;
}
}
