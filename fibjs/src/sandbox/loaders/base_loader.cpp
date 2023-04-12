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

const char* SandBox::worker_args = "(async function(exports,require,module,__filename,__dirname,run,Master){";
const char* SandBox::module_args = "(async function(exports,require,module,__filename,__dirname,run){";
const char* SandBox::base_args = "(async function(exports,require,module,__filename,__dirname,run";

result_t SandBox::ExtLoader::run_script(Context* ctx, Buffer_base* src, exlib::string name,
    std::vector<arg>& extarg, bool is_main)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::String> strRequire = isolate->NewString("require");
    v8::Local<v8::String> strExports = isolate->NewString("exports");

    v8::Local<v8::Object> module = v8::Object::New(isolate->m_isolate);
    v8::Local<v8::Object> exports = v8::Object::New(isolate->m_isolate);

    module->Set(context, strExports, exports).IsJust();
    module->Set(context, strRequire, ctx->m_fnRequest).IsJust();

    if (is_main)
        ctx->m_fnRequest->Set(context, isolate->NewString("main"), module).IsJust();

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

    args[0] = exports;
    args[1] = ctx->m_fnRequest;
    args[2] = module;
    args[3] = isolate->NewString(name);
    args[4] = isolate->NewString(pname);
    args[5] = ctx->m_fnRun;

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
    v8::Local<v8::Value> v = script->Run(context).FromMaybe(v8::Local<v8::Value>());
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    JSFunction func = v8::Local<v8::Function>::Cast(v);

    v8::Local<v8::Object> glob = context->Global();

    v = func.Call(context, glob, (int32_t)args.size(), args.data());
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}
}
