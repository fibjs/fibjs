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

result_t json_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    Isolate* isolate = ctx->m_sb->holder();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string strScript;
    src->toString(strScript);
    v8::Local<v8::Value> v;

    result_t hr = json_base::decode(strScript, v);
    if (hr < 0)
        return hr;

    v8::Local<v8::Object> module = args[2].As<v8::Object>();
    module->Set(context, isolate->NewString("exports"), v).IsJust();

    return 0;
}
}