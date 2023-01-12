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
#include "ifs/util.h"

namespace fibjs {

result_t JsLoader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::String> soname = isolate->NewString(name);

    exlib::string strScript;
    src->toString(strScript);
    const char* c_str = strScript.c_str();

    if (strScript.length() > 2 && c_str[0] == '#' && c_str[1] == '!') {
        char* _strScript = strScript.c_buffer();
        _strScript[0] = '/';
        _strScript[1] = '/';
    }

    exlib::string src1 = arg_names + strScript + "\n});";

    TryCatch try_catch;
    v8::ScriptOrigin so_origin(isolate->m_isolate, soname);
    script = v8::Script::Compile(
        isolate->m_isolate->GetCurrentContext(),
        isolate->NewString(src1), &so_origin).FromMaybe(v8::Local<v8::Script>());

    if (script.IsEmpty()) {
        TryCatch try_catch1;

        v8::ScriptCompiler::Source script_source(
            isolate->NewString(strScript),
            so_origin);

        if (v8::ScriptCompiler::CompileUnboundScript(
                isolate->m_isolate, &script_source)
                .IsEmpty()) {
            try_catch.Reset();
            return throwSyntaxError(try_catch1);
        }

        return throwSyntaxError(try_catch);
    }

    return 0;
}
}