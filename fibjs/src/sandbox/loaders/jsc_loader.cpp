/*
 * jsc_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "path.h"
#include "SandBox.h"
#include "Buffer.h"
#include "ifs/zlib.h"
#include "loaders.h"
#include "version.h"

namespace fibjs {

result_t jsc_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    result_t hr;

    obj_ptr<Buffer_base> unz;
    hr = zlib_base::cc_gunzip(src, -1, unz);
    if (hr < 0)
        return hr;

    Isolate* isolate = ctx->m_sb->holder();

    v8::Local<v8::String> soname = isolate->NewString(name);
    exlib::string pname;
    path_base::dirname(name, pname);

    Buffer* code = Buffer::Cast(unz);

    int32_t code_len = (int32_t)code->length() - sizeof(int32_t);

    if (*(int32_t*)(code->data() + code_len) != jsc_version)
        return CHECK_ERROR(Runtime::setError("SandBox: bad jsc version."));

    code_len -= sizeof(int32_t);

    exlib::string s_temp_source;
    int32_t src_len = *(int32_t*)(code->data() + code_len);
    int32_t i;

    s_temp_source.resize(src_len);
    char* _temp_source = s_temp_source.data();

    for (i = 0; i < src_len; i++)
        _temp_source[i] = '.';

    code_len -= sizeof(int32_t);
    int32_t line_count = *(int32_t*)(code->data() + code_len);

    code_len -= sizeof(int32_t) * line_count;

    int32_t pos = 0;
    int32_t* p = (int32_t*)(code->data() + code_len);

    for (i = 0; i < line_count; i++) {
        pos += p[i];
        if (pos >= src_len)
            break;
        _temp_source[pos] = '\n';
        pos++;
    }

    {
        TryCatch try_catch;

        v8::ScriptCompiler::CachedData* cache;
        cache = new v8::ScriptCompiler::CachedData(code->data(), code_len);

        v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
        pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, ctx->m_sb->m_id));
        v8::ScriptOrigin so_origin(soname, -1, 0, false,
            -1, v8::Local<v8::Value>(), false, false, false, pargs);

        v8::ScriptCompiler::Source source(isolate->NewString(s_temp_source),
            so_origin, cache);

        script = v8::ScriptCompiler::Compile(isolate->context(), &source,
            v8::ScriptCompiler::kConsumeCodeCache)
                     .FromMaybe(v8::Local<v8::Script>());

        if (script.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    return 0;
}
}