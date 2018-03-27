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
#include "utf8.h"
#include "QuickArray.h"
#include "version.h"

namespace fibjs {

result_t util_base::compile(exlib::string srcname, exlib::string script,
    int32_t mode, obj_ptr<Buffer_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    exlib::string oname = srcname;

    v8::Local<v8::String> soname = isolate->NewString(oname);

    if (script.length() > 2 && script[0] == '#' && script[1] == '!') {
        script[0] = '/';
        script[1] = '/';
    }

    v8::Local<v8::Script> code;
    {
        TryCatch try_catch;

        {
            v8::ScriptCompiler::Source script_source(
                isolate->NewString(script));

            if (v8::ScriptCompiler::CompileUnboundScript(
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

        exlib::wstring wscript(utf8to16String(script));

        v8::Local<v8::String> v8src = v8::String::NewFromTwoByte(isolate->m_isolate, (const uint16_t*)wscript.c_str(),
            v8::String::kNormalString, (int32_t)wscript.length());
        v8::ScriptCompiler::Source script_source(v8src, v8::ScriptOrigin(soname));

        v8::MaybeLocal<v8::UnboundScript> ubs = v8::ScriptCompiler::CompileUnboundScript(
            isolate->m_isolate, &script_source,
            v8::ScriptCompiler::kEagerCompile);
        if (ubs.IsEmpty())
            return throwSyntaxError(try_catch);

        const v8::ScriptCompiler::CachedData* cache;
        cache = v8::ScriptCompiler::CreateCodeCache(ubs.ToLocalChecked(), v8src);

        exlib::string buf((const char*)cache->data, cache->length);

        int32_t len = (int32_t)wscript.length();
        QuickArray<int32_t> lines;
        int32_t pos, n;
        const exlib::wchar* c_str = wscript.c_str();

        pos = 0;
        while (pos < len) {
            n = 0;
            while (pos < len && c_str[pos] != '\n') {
                pos++;
                n++;
            }

            if (pos < len) {
                pos++;
                lines.append(n);
                n = 0;
            }
        }

        n = (int32_t)lines.size();
        pos = (int32_t)buf.length();

        buf.resize(pos + (n + 3) * sizeof(int32_t));
        int32_t* p = (int32_t*)&buf[pos];
        int32_t i;

        for (i = 0; i < n; i++) {
            *p = lines[i];
            p++;
        }

        p[0] = n;
        p[1] = len;
        p[2] = jsc_version;

        obj_ptr<Buffer_base> unz = new Buffer(buf);
        return zlib_base::cc_gzip(unz, retVal);
    }

    return 0;
}
}
