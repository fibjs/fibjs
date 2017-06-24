/*
 * util_compile.cpp
 *
 *  Created on: May 8, 2017
 *      Author: lion
 */

#include "object.h"
#include "ifs/util.h"
#include "ifs/zlib.h"
#include "Buffer.h"
#include "SandBox.h"

namespace fibjs {

result_t util_base::compile(exlib::string srcname, exlib::string script,
    int32_t mode, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    exlib::string oname = srcname;

    v8::Local<v8::String> soname = isolate->NewFromUtf8(oname);

    if (script.length() > 2 && script[0] == '#' && script[1] == '!') {
        script[0] = '/';
        script[1] = '/';
    }

    v8::Local<v8::Script> code;
    {
        TryCatch try_catch;

        {
            v8::ScriptCompiler::Source script_source(
                isolate->NewFromUtf8(script));

            if (v8::ScriptCompiler::CompileUnbound(
                    isolate->m_isolate, &script_source)
                    .IsEmpty())
                return throwSyntaxError(try_catch);
        }

        const char* args;

        switch (mode) {
        case 1:
            args = SandBox::script_args;
            break;
        case 2:
            args = SandBox::worker_args;
            break;
        default:
            args = SandBox::module_args;
            break;
        }
        script = args + script + "\n});";

        v8::ScriptCompiler::Source script_source(
            isolate->NewFromUtf8(script), v8::ScriptOrigin(soname));

        if (v8::ScriptCompiler::CompileUnbound(
                isolate->m_isolate, &script_source,
                v8::ScriptCompiler::kProduceCodeCache)
                .IsEmpty())
            return throwSyntaxError(try_catch);

        const v8::ScriptCompiler::CachedData* cache = script_source.GetCachedData();

        exlib::string buf((const char*)cache->data, cache->length);

        int32_t len = (int32_t)script.length();
        buf.append((const char*)&len, sizeof(len));

        obj_ptr<Buffer_base> unz = new Buffer(buf);
        return zlib_base::cc_gzip(unz, retVal);
    }

    return 0;
}
}
