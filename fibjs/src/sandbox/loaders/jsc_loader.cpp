/*
 * jsc_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "ifs/zlib.h"
#include "loaders.h"

namespace fibjs {

result_t JscLoader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Value>* args, int32_t args_count)
{
    result_t hr;

    obj_ptr<Buffer_base> unz;
    hr = zlib_base::cc_gunzip(src, unz);
    if (hr < 0)
        return hr;

    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> soname = isolate->NewFromUtf8(name);
    exlib::string pname;
    path_base::dirname(name, pname);

    v8::Local<v8::Script> script;
    {
        TryCatch try_catch;

        exlib::string code;
        unz->toString(code);

        v8::ScriptCompiler::CachedData* cache;
        cache = new v8::ScriptCompiler::CachedData((const uint8_t*)code.c_str(),
            (int32_t)(code.length() - sizeof(int32_t)));

        exlib::string s_temp_source;
        int32_t len = *(int32_t*)(code.c_str() + code.length() - sizeof(int32_t));
        int32_t i;

        s_temp_source.resize(len);

        for (i = 0; i < len; i++)
            s_temp_source[i] = '.';

        v8::ScriptCompiler::Source source(isolate->NewFromUtf8(s_temp_source),
            v8::ScriptOrigin(soname), cache);

        script = v8::ScriptCompiler::Compile(isolate->m_isolate, &source,
            v8::ScriptCompiler::kConsumeCodeCache);

        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
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