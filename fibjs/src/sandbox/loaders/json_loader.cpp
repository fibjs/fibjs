/*
 * json_loader.cpp
 *
 *  Created on: Jun 3, 2017
 *      Author: lion
 */

#include "object.h"
#include "SandBox.h"
#include "ifs/encoding.h"
#include "loaders.h"

namespace fibjs {
result_t JsonLoader::run_module(SandBox::Context* ctx, Buffer_base* src,
    exlib::string name, v8::Local<v8::Object> module, v8::Local<v8::Object> exports)
{
    exlib::string strScript;
    v8::Local<v8::Value> v;

    src->toString(strScript);
    result_t hr = json_base::decode(strScript, v);
    if (hr < 0)
        return hr;

    Isolate* isolate = ctx->m_sb->holder();
    module->Set(isolate->NewString("exports"), v);

    return 0;
}
}