/*
 * module.cpp
 *
 *  Created on: Jan 28, 2026
 *      Author: lion
 */

#include "object.h"
#include "ifs/module.h"
#include "options.h"

namespace fibjs {

result_t module_base::get_builtinModules(std::vector<exlib::string>& retVal)
{
    RootModule* pModule = RootModule::g_root;

    // Add "buffer" module
    retVal.push_back("buffer");
    retVal.push_back("node:buffer");

    // Add all other native builtin modules
    while (pModule) {
        const char* name = pModule->name();
        retVal.push_back(name);

        // Add node: prefixed version
        exlib::string node_name = "node:";
        node_name.append(name);
        retVal.push_back(node_name);

        pModule = pModule->m_next;
    }

    // "module" is created by SandBox::initModule instead of a native module
    retVal.push_back("module");
    retVal.push_back("node:module");

    // Add the embedded JS builtin modules, such as "stream", "readline" or
    // "timers/promises"
    for (intptr_t i = 0; opt_tools[i].name; i++) {
        const char* name = opt_tools[i].name;

        if (!is_user_opt_tool(name))
            continue;

        retVal.push_back(name);

        exlib::string node_name = "node:";
        node_name.append(name);
        retVal.push_back(node_name);
    }

    // Add sub-path builtin modules (Node.js compatibility)
    static const char* s_subpaths[] = {
        "assert/strict", "util/types",
        "path/posix", "path/win32",
        "fs/promises", "dns/promises"
    };

    for (size_t i = 0; i < sizeof(s_subpaths) / sizeof(s_subpaths[0]); i++) {
        retVal.push_back(s_subpaths[i]);

        exlib::string node_name = "node:";
        node_name.append(s_subpaths[i]);
        retVal.push_back(node_name);
    }

    return result_t();
}

result_t module_base::createRequire(exlib::string base, v8::Local<v8::Function>& retVal)
{
    // This is handled by SandBox::initModule in SandBox_context.cpp
    return result_t::invalid_call();
}

result_t module_base::isBuiltin(v8::Local<v8::Value> id, bool& retVal)
{
    retVal = false;

    // Only a primitive string names a module.  A String object or anything with a
    // toString() is deliberately not coerced: Node returns false for those, and the
    // call has to stay safe inside feature detection.
    if (!id->IsString())
        return result_t();

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    exlib::string name;
    result_t hr = GetArgumentValue(isolate, id, name);
    if (hr.failed())
        return hr;

    // Accept both forms.  builtinModules holds every module twice (bare and
    // node:-prefixed), so matching the given spelling directly would reject the
    // form the caller did not use; strip one prefix and probe both spellings.
    // "node:node:fs" strips to "node:fs", and the table holds that entry, so the
    // second pass has to reject a name that still starts with the prefix.
    static const char kNodePrefix[] = "node:";
    const size_t kPrefixLen = sizeof(kNodePrefix) - 1;

    if (name.length() >= kPrefixLen && !qstrcmp(name.c_str(), kNodePrefix, kPrefixLen))
        name = name.substr(kPrefixLen);

    if (name.length() >= kPrefixLen && !qstrcmp(name.c_str(), kNodePrefix, kPrefixLen))
        return result_t();

    std::vector<exlib::string> modules;
    module_base::get_builtinModules(modules);

    for (size_t i = 0; i < modules.size(); i++)
        if (!qstrcmp(modules[i].c_str(), name.c_str())) {
            retVal = true;
            break;
        }

    return result_t();
}

} /* namespace fibjs */
