/*
 * module.cpp
 *
 *  Created on: Jan 28, 2026
 *      Author: lion
 */

#include "object.h"
#include "ifs/module.h"

namespace fibjs {

result_t module_base::get_builtinModules(v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Array> builtinModules = v8::Array::New(isolate->m_isolate);
    RootModule* pModule = RootModule::g_root;
    intptr_t idx = 0;

    // Add "buffer" module
    builtinModules->Set(context, (uint32_t)(idx++), isolate->NewString("buffer")).IsJust();
    builtinModules->Set(context, (uint32_t)(idx++), isolate->NewString("node:buffer")).IsJust();

    // Add all other builtin modules
    while (pModule) {
        const char* name = pModule->name();
        builtinModules->Set(context, (uint32_t)(idx++), isolate->NewString(name)).IsJust();

        // Add node: prefixed version
        exlib::string node_name = "node:";
        node_name.append(name);
        builtinModules->Set(context, (uint32_t)(idx++), isolate->NewString(node_name)).IsJust();

        pModule = pModule->m_next;
    }

    // Add sub-path builtin modules (Node.js compatibility)
    static const char* s_subpaths[] = {
        "assert/strict", "util/types",
        "path/posix", "path/win32",
        "fs/promises", "dns/promises",
        "stream/promises", "timers/promises", "readline/promises",
        "stream/web"
    };

    for (size_t i = 0; i < sizeof(s_subpaths) / sizeof(s_subpaths[0]); i++) {
        builtinModules->Set(context, (uint32_t)(idx++), isolate->NewString(s_subpaths[i])).IsJust();

        exlib::string node_name = "node:";
        node_name.append(s_subpaths[i]);
        builtinModules->Set(context, (uint32_t)(idx++), isolate->NewString(node_name)).IsJust();
    }

    retVal = builtinModules;
    return 0;
}

result_t module_base::createRequire(exlib::string base, v8::Local<v8::Function>& retVal)
{
    // This is handled by SandBox::initModule in SandBox_context.cpp
    return CALL_E_INVALID_CALL;
}

} /* namespace fibjs */
