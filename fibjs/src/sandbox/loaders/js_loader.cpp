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
    exlib::string arg_names, std::vector<v8::Local<v8::Value>>& args, bool in_cjs)
{
    result_t hr;
    SandBox::ModuleType type;

    hr = ctx->m_sb->resolveModuleType(name, type);
    if (hr)
        return hr;

    if (type == SandBox::kCommonJS)
        return m_cjs.run(ctx, src, name, arg_names, args, in_cjs);
    else if (type == SandBox::kESModule)
        return m_mjs.run(ctx, src, name, arg_names, args, in_cjs);

    return CHECK_ERROR(Runtime::setError("SandBox: Invalid file format."));
}
}