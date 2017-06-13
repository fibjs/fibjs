/*
 * js_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "loaders.h"

namespace fibjs {

result_t JsLoader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Value>* args, int32_t args_count)
{
    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> soname = isolate->NewFromUtf8(name);
    exlib::string pname;
    path_base::dirname(name, pname);

    v8::Local<v8::Script> script;
    {
        TryCatch try_catch;

        exlib::string strScript;

        if (src)
            src->toString(strScript);

        const char* c_str = strScript.c_str();

        if (strScript.length() > 2 && c_str[0] == '#' && c_str[1] == '!') {
            strScript[0] = '/';
            strScript[1] = '/';
        }

        exlib::string src1 = arg_names + strScript + "\n});";

        script = v8::Script::Compile(
            isolate->NewFromUtf8(src1), soname);
        if (script.IsEmpty()) {
            TryCatch try_catch1;

            v8::ScriptCompiler::Source script_source(
                isolate->NewFromUtf8(strScript),
                v8::ScriptOrigin(soname));

            if (v8::ScriptCompiler::CompileUnbound(
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
    args[1] = isolate->NewFromUtf8(pname);
    v8::Local<v8::Object> glob = isolate->context()->Global();
    v = v8::Local<v8::Function>::Cast(v)->Call(glob, args_count, args);
    if (v.IsEmpty())
        return CALL_E_JAVASCRIPT;

    return 0;
}
}