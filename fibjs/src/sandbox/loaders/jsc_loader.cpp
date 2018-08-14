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
#include "ifs/util.h"
#include "loaders.h"
#include "version.h"

namespace fibjs {

result_t JscLoader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
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

    exlib::string code;
    unz->toString(code);

    int32_t code_len = (int32_t)code.length() - sizeof(int32_t);

    if (*(int32_t*)(code.c_str() + code_len) != jsc_version)
        return CHECK_ERROR(Runtime::setError("SandBox: bad jsc version."));

    code_len -= sizeof(int32_t);

    exlib::string s_temp_source;
    int32_t src_len = *(int32_t*)(code.c_str() + code_len);
    int32_t i;

    s_temp_source.resize(src_len);

    for (i = 0; i < src_len; i++)
        s_temp_source[i] = '.';

    code_len -= sizeof(int32_t);
    int32_t line_count = *(int32_t*)(code.c_str() + code_len);

    code_len -= sizeof(int32_t) * line_count;

    int32_t pos = 0;
    int32_t* p = (int32_t*)(code.c_str() + code_len);

    for (i = 0; i < line_count; i++) {
        pos += p[i];
        if (pos >= src_len)
            break;
        s_temp_source[pos] = '\n';
        pos++;
    }

    v8::MaybeLocal<v8::Script> mayscript;
    {
        TryCatch try_catch;

        v8::ScriptCompiler::CachedData* cache;
        cache = new v8::ScriptCompiler::CachedData((const uint8_t*)code.c_str(), code_len);

        v8::ScriptCompiler::Source source(isolate->NewString(s_temp_source),
            v8::ScriptOrigin(soname), cache);

        mayscript = v8::ScriptCompiler::Compile(isolate->context(), &source,
            v8::ScriptCompiler::kConsumeCodeCache);

        if (mayscript.IsEmpty())
            return throwSyntaxError(try_catch);
    }

    script = mayscript.ToLocalChecked();

    return 0;
}
}