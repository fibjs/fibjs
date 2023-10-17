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

namespace fibjs {

result_t process_base::dlopen(v8::Local<v8::Object> module, exlib::string filename, int32_t flags)
{
    void* handle_ = ::dlopen(filename.c_str(), flags);
    if (handle_ == nullptr)
        return CHECK_ERROR(LastError());

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

    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();

    v8::Local<v8::Object> exports;
    v8::Local<v8::Value> exports_v = module->Get(context, isolate->NewString("exports")).FromMaybe(v8::Local<v8::Value>());
    if (!exports_v.IsEmpty())
        exports = exports_v->ToObject(context).FromMaybe(v8::Local<v8::Object>());

    napi_env env = new napi_env__(context, module_api_version);

    napi_value _exports = nullptr;
    env->CallIntoModule([&](napi_env env) {
        _exports = register_func(env, v8impl::JsValueFromV8LocalValue(exports));
    });

    if (_exports != nullptr && _exports != v8impl::JsValueFromV8LocalValue(exports))
        module->Set(context, isolate->NewString("exports"), v8impl::V8LocalValueFromJsValue(_exports)).FromMaybe(false);

    return 0;
}

}
