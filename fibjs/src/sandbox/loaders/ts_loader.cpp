/*
 * ts_loader.cpp
 *
 *  Created on: May 27, 2018
 *      Author: Richard
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "Buffer.h"
#include "loaders.h"

namespace fibjs {

extern const char* opt_tools[];

result_t TsLoader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Value>* args, int32_t args_count)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> soname = isolate->NewString(name);
    exlib::string pname;
    path_base::dirname(name, pname);

    v8::Local<v8::Script> script;
    {
        result_t hr;
        exlib::string strScript;

        src->toString(strScript);
        const char* c_str = strScript.c_str();

        if (strScript.length() > 2 && c_str[0] == '#' && c_str[1] == '!') {
            strScript[0] = '/';
            strScript[1] = '/';
        }

        /* transpile with typescript first :start */
        v8::Local<v8::Value> m;
        hr = isolate->m_topSandbox->require("internal/typescript", "/", m);
        if (hr < 0)
            return hr;

        v8::Local<v8::Object> object = m->ToObject();
        v8::Local<v8::Value> transFunc = object->Get(isolate->NewString("transpile", 9));

        v8::Local<v8::Value> scriptContent = isolate->NewString(strScript);
        v8::Local<v8::Object> compilerOptions = v8::Object::New(isolate->m_isolate);

        // v8::Local<v8::Array> tsLibs = v8::Array::New(isolate->m_isolate);
        // int32_t tsLibCount = 0;
        // tsLibs->Set(tsLibCount++, isolate->NewString("ES6"));
        // tsLibs->Set(tsLibCount++, isolate->NewString("Iterable"));
        // tsLibs->Set(tsLibCount++, isolate->NewString("ESNext"));
        // compilerOptions->Set(isolate->NewString("lib"), tsLibs);

        compilerOptions->Set(isolate->NewString("target"), isolate->NewString("ES6"));
        compilerOptions->Set(isolate->NewString("module"), isolate->NewString("CommonJS"));
        compilerOptions->Set(isolate->NewString("strict"), v8::BooleanObject::New(isolate->m_isolate, true));
        compilerOptions->Set(isolate->NewString("diagnostics"), v8::BooleanObject::New(isolate->m_isolate, true));
        compilerOptions->Set(isolate->NewString("allowJs"), v8::BooleanObject::New(isolate->m_isolate, true));

        // compilerOptions->Set(isolate->NewString("inlineSourceMap"), v8::BooleanObject::New(isolate->m_isolate, true));
        // compilerOptions->Set(isolate->NewString("inlineSources"), v8::BooleanObject::New(isolate->m_isolate, true));
        // compilerOptions->Set(isolate->NewString("sourceMap"), v8::BooleanObject::New(isolate->m_isolate, true));

        std::vector<v8::Local<v8::Value>> tranpileArgs;
        tranpileArgs.resize(2);
        tranpileArgs[0] = scriptContent;
        tranpileArgs[1] = compilerOptions;

        v8::Local<v8::Value> compiledScript = v8::Local<v8::Function>::Cast(transFunc)->Call(object, (int32_t)tranpileArgs.size(), tranpileArgs.data());
        if (compiledScript.IsEmpty())
            return CALL_E_JAVASCRIPT;

        hr = GetArgumentValue(compiledScript, strScript, true);
        if (hr < 0)
            return hr;
        /* transpile with typescript first :end */
        exlib::string src1 = arg_names + strScript + "\n});";

        TryCatch try_catch;
        script = v8::Script::Compile(
            isolate->NewString(src1), soname);
        if (script.IsEmpty()) {
            TryCatch try_catch1;

            v8::ScriptCompiler::Source script_source(
                isolate->NewString(strScript),
                v8::ScriptOrigin(soname));

            if (v8::ScriptCompiler::CompileUnboundScript(
                    isolate->m_isolate, &script_source)
                    .IsEmpty()) {
                try_catch.Reset();
                return throwSyntaxError(try_catch1);
            }

            throwSyntaxError(try_catch);
        }
    }

    v8::Local<v8::Value> v = script->Run();
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    args[0] = soname;
    args[1] = isolate->NewString(pname);
    v8::Local<v8::Object> glob = isolate->context()->Global();
    v = v8::Local<v8::Function>::Cast(v)->Call(glob, args_count, args);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}
}
