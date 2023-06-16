/*
 * js_loader.cpp
 *
 *  Created on: Jun 10, 2023
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "loaders.h"
#include "ifs/util.h"

namespace fibjs {

static v8::MaybeLocal<v8::Module> resolveModule(
    v8::Local<v8::Context> context, v8::Local<v8::String> specifier,
    v8::Local<v8::FixedArray> import_assertions, v8::Local<v8::Module> referrer)
{
    return v8::MaybeLocal<v8::Module>();
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
    if (promise.IsEmpty())
        return throwSyntaxError(try_catch);

    v8::Local<v8::Value> result = isolate->WaitPromise(promise);
    if (result.IsEmpty())
        return throwSyntaxError(try_catch);

    v8::Local<v8::Object>::Cast(args[2])->Set(context, isolate->NewString("exports"), module->GetModuleNamespace()).IsJust();

    return 0;
}
}