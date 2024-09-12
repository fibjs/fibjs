/*
 * node_loader.cpp
 *
 *  Created on: Oct 18, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include "SandBox.h"
#include "loaders.h"

namespace fibjs {

result_t node_Loader::run(SandBox::Context* ctx, Buffer_base* src, exlib::string name,
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args)
{
    return process_base::dlopen(args[2].As<v8::Object>(), name, 1);
}
}