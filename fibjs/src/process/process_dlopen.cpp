/*
 * process_dlopen.cpp
 *
 *  Created on: Oct 17, 2023
 *      Author: lion
 */

#include "object.h"
#include "ifs/process.h"
#include <exlib/include/dl_func.h>
#include "addons/js_native_api_internal.h"

namespace node {

static exlib::fiber_local<node_module*> thread_local_modpending;

void node_module_register(void* m)
{
    thread_local_modpending = reinterpret_cast<struct node_module*>(m);
}

} // namespace node

namespace fibjs {

result_t process_base::dlopen(v8::Local<v8::Object> module, exlib::string filename, int32_t flags)
{
    if (node::thread_local_modpending != NULL)
        return CALL_E_INVALID_CALL;

    void* handle_ = ::dlopen(filename.c_str(), flags);
    if (handle_ == nullptr)
#ifdef WIN32
        return CHECK_ERROR(LastError());
#else
        return CHECK_ERROR(Runtime::setError(dlerror()));
#endif

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> exports;
    v8::Local<v8::Value> exports_v = module->Get(context, isolate->NewString("exports")).FromMaybe(v8::Local<v8::Value>());
    if (!exports_v.IsEmpty())
        exports = exports_v->ToObject(context).FromMaybe(v8::Local<v8::Object>());

    node::node_module* mp = node::thread_local_modpending;
    node::thread_local_modpending = NULL;

    if (mp != NULL) {
        if (mp->nm_context_register_func != nullptr) {
            mp->nm_context_register_func(exports, module, context, mp->nm_priv);
        } else if (mp->nm_register_func != nullptr) {
            mp->nm_register_func(exports, module, mp->nm_priv);
        } else {
            ::dlclose(handle_);
            return CHECK_ERROR(Runtime::setError("process: invalid module."));
        }
    } else {
        napi_addon_register_func register_func;
        register_func = (napi_addon_register_func)::dlsym(handle_, "napi_register_module_v1");
        if (register_func == nullptr) {
            ::dlclose(handle_);
            return CHECK_ERROR(Runtime::setError("process: invalid module."));
        }

        int32_t module_api_version = NODE_API_DEFAULT_MODULE_API_VERSION;
        node_api_addon_get_api_version_func get_api_version_func;
        get_api_version_func = (node_api_addon_get_api_version_func)::dlsym(handle_, "node_api_module_get_api_version_v1");
        if (get_api_version_func)
            module_api_version = get_api_version_func();

        napi_module_register_by_symbol(exports, module, context, register_func, module_api_version);
    }

    return 0;
}

}
