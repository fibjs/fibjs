/*
 * cjs_loader.cpp
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

result_t cjs_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::String> soname = isolate->NewString(name);

    exlib::string strScript;
    src->toString(strScript);
    const char* c_str = strScript.c_str();

    if (strScript.length() > 2 && c_str[0] == '#' && c_str[1] == '!') {
        char* _strScript = strScript.data();
        _strScript[0] = '/';
        _strScript[1] = '/';
    }

    exlib::string src1 = arg_names + "\n" + strScript + "\n});";

    TryCatch try_catch;

    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
    pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, ctx->m_sb->m_id));
    v8::ScriptOrigin so_origin(isolate->m_isolate, soname, -1, 0, false,
        -1, v8::Local<v8::Value>(), false, false, false, pargs);

    script = v8::Script::Compile(isolate->m_isolate->GetCurrentContext(),
        isolate->NewString(src1), &so_origin)
                 .FromMaybe(v8::Local<v8::Script>());

    if (script.IsEmpty())
        return throwSyntaxError(try_catch);

    return 0;
}
}