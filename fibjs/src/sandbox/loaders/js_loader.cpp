/*
 * js_loader.cpp
 *
 *  Created on: Aug 19, 2024
 *      Author: lion
 */

#include "object.h"
#include "loaders.h"

namespace fibjs {

result_t js_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    return m_cjs.run(ctx, src, name, arg_names, args);
}
}