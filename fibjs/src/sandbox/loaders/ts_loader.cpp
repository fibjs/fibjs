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
        TryCatch try_catch;

        exlib::string strScript;

        src->toString(strScript);
        const char* c_str = strScript.c_str();

        if (strScript.length() > 2 && c_str[0] == '#' && c_str[1] == '!') {
            strScript[0] = '/';
            strScript[1] = '/';
        }

        /* transpile with typescript first :start */
        v8::Local<v8::Value> m;
        ctx->m_sb->require("internal/typescript", "/", m);
        if (m.IsEmpty())
            return CALL_E_JAVASCRIPT;

        v8::Local<v8::Object> object = m->ToObject();

        v8::Local<v8::Value> transFunc = object->Get(isolate->NewString("transpile", 9));
        v8::Local<v8::Value> pscript = isolate->NewString(strScript);
        v8::Local<v8::Value> compiledScript = v8::Local<v8::Function>::Cast(transFunc)->Call(object, 1, &pscript);

        if (compiledScript.IsEmpty())
            return CALL_E_JAVASCRIPT;

        result_t hr = GetArgumentValue(compiledScript, strScript, true);
        if (hr < 0)
            return hr;
        /* transpile with typescript first :end */
        exlib::string src1 = arg_names + strScript + "\n});";

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
