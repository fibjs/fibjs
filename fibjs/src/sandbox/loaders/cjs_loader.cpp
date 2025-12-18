/*
 * cjs_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "Buffer.h"
#include "loaders.h"

namespace fibjs {

result_t cjs_Loader::compile(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, v8::Local<v8::Script>& script)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::String> soname = isolate->NewString(name);

    Buffer* buf = Buffer::Cast(src);
    const char* data = (const char*)buf->data();
    size_t length = buf->length();

    // Construct script: arg_names + "\n" + source + "\n});" 
    exlib::string src1;
    src1.reserve(arg_names.length() + 1 + length + 4); // pre-allocate
    src1.append(arg_names);
    src1.append("\n", 1);
    
    size_t code_start = src1.length();
    src1.append(data, length);
    
    // Handle shebang (#!) by replacing with // comment in src1
    if (length > 2 && src1[code_start] == '#' && src1[code_start + 1] == '!') {
        src1[code_start] = '/';
        src1[code_start + 1] = '/';
    }
    
    src1.append("\n});", 4);

    TryCatch try_catch;

    v8::Local<v8::PrimitiveArray> pargs = v8::PrimitiveArray::New(isolate->m_isolate, 1);
    pargs->Set(isolate->m_isolate, 0, v8::Number::New(isolate->m_isolate, ctx->m_sb->m_id));
    v8::ScriptOrigin so_origin(soname, -1, 0, false,
        -1, v8::Local<v8::Value>(), false, false, false, pargs);

    script = v8::Script::Compile(isolate->m_isolate->GetCurrentContext(),
        isolate->NewString(src1), &so_origin)
                 .FromMaybe(v8::Local<v8::Script>());

    if (script.IsEmpty())
        return throwSyntaxError(try_catch);

    return 0;
}
}