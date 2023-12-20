/*
 * node_api.cpp
 *
 *  Created on: Oct 17, 2023
 *      Author: lion
 */

#include "addons/js_native_api_internal.h"

namespace v8impl {

namespace {

    class BufferFinalizer : private Finalizer {
    public:
        static BufferFinalizer* New(napi_env env,
            napi_finalize finalize_callback = nullptr,
            void* finalize_data = nullptr,
            void* finalize_hint = nullptr)
        {
            return new BufferFinalizer(
                env, finalize_callback, finalize_data, finalize_hint);
        }
        // node::Buffer::FreeCallback
        static void FinalizeBufferCallback(char* data, void* hint)
        {
            std::unique_ptr<BufferFinalizer, Deleter> finalizer {
                static_cast<BufferFinalizer*>(hint)
            };
            finalizer->finalize_data_ = data;

            // It is safe to call into JavaScript at this point.
            if (finalizer->finalize_callback_ == nullptr)
                return;
            finalizer->env_->CallFinalizer(finalizer->finalize_callback_,
                finalizer->finalize_data_,
                finalizer->finalize_hint_);
        }

        struct Deleter {
            void operator()(BufferFinalizer* finalizer) { delete finalizer; }
        };

    private:
        BufferFinalizer(napi_env env,
            napi_finalize finalize_callback,
            void* finalize_data,
            void* finalize_hint)
            : Finalizer(env, finalize_callback, finalize_data, finalize_hint)
        {
            env_->Ref();
        }

        ~BufferFinalizer() { env_->Unref(); }
    };

    void ThrowNodeApiVersionError(node::Environment* node_env,
        const char* module_name,
        int32_t module_api_version)
    {
        std::string error_message;
        error_message += module_name;
        error_message += " requires Node-API version ";
        error_message += std::to_string(module_api_version);
        error_message += ", but this version of Node.js only supports version ";
        error_message += NODE_STRINGIFY(NODE_API_SUPPORTED_VERSION_MAX) " add-ons.";
        node_env->ThrowError(error_message.c_str());
    }

} // end of anonymous namespace

} // end of namespace v8impl

// Intercepts the Node-V8 module registration callback. Converts parameters
// to NAPI equivalents and then calls the registration callback specified
// by the NAPI module.
static void napi_module_register_cb(v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    v8::Local<v8::Context> context,
    void* priv)
{
    napi_module_register_by_symbol(
        exports,
        module,
        context,
        static_cast<const napi_module*>(priv)->nm_register_func);
}

template <int32_t module_api_version>
static void node_api_context_register_func(v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    v8::Local<v8::Context> context,
    void* priv)
{
    napi_module_register_by_symbol(
        exports,
        module,
        context,
        reinterpret_cast<napi_addon_register_func>(priv),
        module_api_version);
}

// This function must be augmented for each new Node API version.
// The key role of this function is to encode module_api_version in the function
// pointer. We are not going to have many Node API versions and having one
// function per version is relatively cheap. It avoids dynamic memory
// allocations or implementing more expensive changes to module registration.
// Currently AddLinkedBinding is the only user of this function.
node::addon_context_register_func get_node_api_context_register_func(
    node::Environment* node_env,
    const char* module_name,
    int32_t module_api_version)
{
    static_assert(
        NODE_API_SUPPORTED_VERSION_MAX == 9,
        "New version of Node-API requires adding another else-if statement below "
        "for the new version and updating this assert condition.");
    if (module_api_version == 9) {
        return node_api_context_register_func<9>;
    } else if (module_api_version == NAPI_VERSION_EXPERIMENTAL) {
        return node_api_context_register_func<NAPI_VERSION_EXPERIMENTAL>;
    } else if (module_api_version >= NODE_API_SUPPORTED_VERSION_MIN && module_api_version <= NODE_API_DEFAULT_MODULE_API_VERSION) {
        return node_api_context_register_func<NODE_API_DEFAULT_MODULE_API_VERSION>;
    } else {
        v8impl::ThrowNodeApiVersionError(node_env, module_name, module_api_version);
        return nullptr;
    }
}

void napi_module_register_by_symbol(v8::Local<v8::Object> exports,
    v8::Local<v8::Value> module,
    v8::Local<v8::Context> context,
    napi_addon_register_func init,
    int32_t module_api_version)
{
    node::Environment* node_env = node::Environment::GetCurrent(context);
    std::string module_filename = "";
    if (init == nullptr) {
        CHECK_NOT_NULL(node_env);
        node_env->ThrowError("Module has no declared entry point.");
        return;
    }

    // We set `env->filename` from `module.filename` here, but we could just as
    // easily add a private property to `exports` in `process.dlopen`, which
    // receives the file name from JS, and retrieve *that* here. Thus, we are not
    // endorsing commonjs here by making use of `module.filename`.
    v8::Local<v8::Value> filename_js;
    v8::Local<v8::Object> modobj;
    // if (module->ToObject(context).ToLocal(&modobj) && modobj->Get(context, node_env->filename_string()).ToLocal(&filename_js) && filename_js->IsString()) {
    //     node::Utf8Value filename(node_env->isolate(), filename_js);

    // Turn the absolute path into a URL. Currently the absolute path is always
    // a file system path.
    // TODO(gabrielschulhof): Pass the `filename` through unchanged if/when we
    // receive it as a URL already.
    // module_filename = node::url::FromFilePath(filename.ToStringView());
    // }

    // Create a new napi_env for this specific module.
    napi_env env = new napi_env__(context, module_filename, module_api_version);

    napi_value _exports = nullptr;
    env->CallIntoModule([&](napi_env env) {
        _exports = init(env, v8impl::JsValueFromV8LocalValue(exports));
    });

    // If register function returned a non-null exports object different from
    // the exports object we passed it, set that as the "exports" property of
    // the module.
    if (_exports != nullptr && _exports != v8impl::JsValueFromV8LocalValue(exports)) {
        napi_value _module = v8impl::JsValueFromV8LocalValue(module);
        napi_set_named_property(env, _module, "exports", _exports);
    }
}

namespace node {
node_module napi_module_to_node_module(const napi_module* mod)
{
    return {
        -1,
        mod->nm_flags,
        nullptr,
        mod->nm_filename,
        nullptr,
        napi_module_register_cb,
        mod->nm_modname,
        const_cast<napi_module*>(mod), // priv
        nullptr,
    };
}
} // namespace node

// Registers a NAPI module.
void NAPI_CDECL napi_module_register(napi_module* mod)
{
    node::node_module* nm = new node::node_module(node::napi_module_to_node_module(mod));
    node::node_module_register(nm);
}

napi_status NAPI_CDECL napi_fatal_exception(napi_env env, napi_value err)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, err);

    v8::Local<v8::Value> local_err = v8impl::V8LocalValueFromJsValue(err);
    // static_cast<node_napi_env>(env)->trigger_fatal_exception(local_err);

    return napi_clear_last_error(env);
}

NAPI_NO_RETURN void NAPI_CDECL napi_fatal_error(const char* location,
    size_t location_len,
    const char* message,
    size_t message_len)
{
    std::string location_string;
    std::string message_string;

    if (location_len != NAPI_AUTO_LENGTH) {
        location_string.assign(const_cast<char*>(location), location_len);
    } else {
        location_string.assign(const_cast<char*>(location), strlen(location));
    }

    if (message_len != NAPI_AUTO_LENGTH) {
        message_string.assign(const_cast<char*>(message), message_len);
    } else {
        message_string.assign(const_cast<char*>(message), strlen(message));
    }

    node::OnFatalError(location_string.c_str(), message_string.c_str());
}

napi_status NAPI_CDECL napi_make_callback(napi_env env,
    napi_async_context async_context,
    napi_value recv,
    napi_value func,
    size_t argc,
    const napi_value* argv,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, recv);
    if (argc > 0) {
        CHECK_ARG(env, argv);
    }

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> v8recv;
    CHECK_TO_OBJECT(env, context, v8recv, recv);

    v8::Local<v8::Function> v8func;
    CHECK_TO_FUNCTION(env, v8func, func);

    v8::MaybeLocal<v8::Value> callback_result;

    callback_result = node::MakeCallback(
        env->isolate,
        v8recv,
        v8func,
        argc,
        reinterpret_cast<v8::Local<v8::Value>*>(const_cast<napi_value*>(argv)),
        { 0, 0 });

    if (try_catch.HasCaught()) {
        return napi_set_last_error(env, napi_pending_exception);
    } else {
        CHECK_MAYBE_EMPTY(env, callback_result, napi_generic_failure);
        if (result != nullptr) {
            *result = v8impl::JsValueFromV8LocalValue(callback_result.ToLocalChecked());
        }
    }

    return GET_RETURN_STATUS(env);
}

napi_status NAPI_CDECL napi_create_buffer(napi_env env,
    size_t length,
    void** data,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::MaybeLocal<v8::Object> maybe = node::Buffer::New(env->isolate, length);

    CHECK_MAYBE_EMPTY(env, maybe, napi_generic_failure);

    v8::Local<v8::Object> buffer = maybe.ToLocalChecked();

    *result = v8impl::JsValueFromV8LocalValue(buffer);

    if (data != nullptr) {
        *data = node::Buffer::Data(buffer);
    }

    return GET_RETURN_STATUS(env);
}

napi_status NAPI_CDECL napi_create_external_buffer(napi_env env,
    size_t length,
    void* data,
    napi_finalize finalize_cb,
    void* finalize_hint,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, result);

#if defined(V8_ENABLE_SANDBOX)
    return napi_set_last_error(env, napi_no_external_buffers_allowed);
#endif

    v8::Isolate* isolate = env->isolate;

    // The finalizer object will delete itself after invoking the callback.
    v8impl::BufferFinalizer* finalizer = v8impl::BufferFinalizer::New(env, finalize_cb, nullptr, finalize_hint);

    v8::MaybeLocal<v8::Object> maybe = node::Buffer::New(isolate,
        static_cast<char*>(data),
        length,
        v8impl::BufferFinalizer::FinalizeBufferCallback,
        finalizer);

    CHECK_MAYBE_EMPTY(env, maybe, napi_generic_failure);

    *result = v8impl::JsValueFromV8LocalValue(maybe.ToLocalChecked());
    return GET_RETURN_STATUS(env);
    // Tell coverity that 'finalizer' should not be freed when we return
    // as it will be deleted when the buffer to which it is associated
    // is finalized.
    // coverity[leaked_storage]
}

napi_status NAPI_CDECL napi_create_buffer_copy(napi_env env,
    size_t length,
    const void* data,
    void** result_data,
    napi_value* result)
{
    NAPI_PREAMBLE(env);
    CHECK_ARG(env, result);

    v8::MaybeLocal<v8::Object> maybe = node::Buffer::Copy(env->isolate, static_cast<const char*>(data), length);

    CHECK_MAYBE_EMPTY(env, maybe, napi_generic_failure);

    v8::Local<v8::Object> buffer = maybe.ToLocalChecked();
    *result = v8impl::JsValueFromV8LocalValue(buffer);

    if (result_data != nullptr) {
        *result_data = node::Buffer::Data(buffer);
    }

    return GET_RETURN_STATUS(env);
}

napi_status NAPI_CDECL napi_is_buffer(napi_env env,
    napi_value value,
    bool* result)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, value);
    CHECK_ARG(env, result);

    *result = node::Buffer::HasInstance(v8impl::V8LocalValueFromJsValue(value));
    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_get_buffer_info(napi_env env,
    napi_value value,
    void** data,
    size_t* length)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, value);

    v8::Local<v8::Value> buffer = v8impl::V8LocalValueFromJsValue(value);

    if (data != nullptr) {
        *data = node::Buffer::Data(buffer);
    }
    if (length != nullptr) {
        *length = node::Buffer::Length(buffer);
    }

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_get_node_version(napi_env env,
    const napi_node_version** result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);
    static const napi_node_version version = {
        NODE_MAJOR_VERSION, NODE_MINOR_VERSION, NODE_PATCH_VERSION, NODE_RELEASE
    };
    *result = &version;
    return napi_clear_last_error(env);
}

namespace {
namespace uvimpl {

    static napi_status ConvertUVErrorCode(int code)
    {
        switch (code) {
        case 0:
            return napi_ok;
        case UV_EINVAL:
            return napi_invalid_arg;
        case UV_ECANCELED:
            return napi_cancelled;
        default:
            return napi_generic_failure;
        }
    }

    // Wrapper around uv_work_t which calls user-provided callbacks.
    class Work : public node::AsyncResource, public node::ThreadPoolWork {
    private:
        explicit Work(node_napi_env env,
            v8::Local<v8::Object> async_resource,
            v8::Local<v8::String> async_resource_name,
            napi_async_execute_callback execute,
            napi_async_complete_callback complete = nullptr,
            void* data = nullptr)
            : AsyncResource(
                env->isolate,
                async_resource,
                *v8::String::Utf8Value(env->isolate, async_resource_name))
            , ThreadPoolWork(env->node_env(), "node_api")
            , _env(env)
            , _data(data)
            , _execute(execute)
            , _complete(complete)
        {
        }

        ~Work() override = default;

    public:
        static Work* New(node_napi_env env,
            v8::Local<v8::Object> async_resource,
            v8::Local<v8::String> async_resource_name,
            napi_async_execute_callback execute,
            napi_async_complete_callback complete,
            void* data)
        {
            return new Work(
                env, async_resource, async_resource_name, execute, complete, data);
        }

        static void Delete(Work* work) { delete work; }

        void DoThreadPoolWork() override { _execute(_env, _data); }

        void AfterThreadPoolWork(int status) override
        {
            if (_complete == nullptr)
                return;

            // Establish a handle scope here so that every callback doesn't have to.
            // Also it is needed for the exception-handling below.
            v8::HandleScope scope(_env->isolate);

            CallbackScope callback_scope(this);

            _env->CallbackIntoModule<true>([&](napi_env env) {
                _complete(env, ConvertUVErrorCode(status), _data);
            });

            // Note: Don't access `work` after this point because it was
            // likely deleted by the complete callback.
        }

    private:
        node_napi_env _env;
        void* _data;
        napi_async_execute_callback _execute;
        napi_async_complete_callback _complete;
    };

} // end of namespace uvimpl
} // end of anonymous namespace

#define CALL_UV(env, condition)                                  \
    do {                                                         \
        int result = (condition);                                \
        napi_status status = uvimpl::ConvertUVErrorCode(result); \
        if (status != napi_ok) {                                 \
            return napi_set_last_error(env, status, result);     \
        }                                                        \
    } while (0)

napi_status NAPI_CDECL
napi_create_async_work(napi_env env,
    napi_value async_resource,
    napi_value async_resource_name,
    napi_async_execute_callback execute,
    napi_async_complete_callback complete,
    void* data,
    napi_async_work* result)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, execute);
    CHECK_ARG(env, result);

    v8::Local<v8::Context> context = env->context();

    v8::Local<v8::Object> resource;
    if (async_resource != nullptr) {
        CHECK_TO_OBJECT(env, context, resource, async_resource);
    } else {
        resource = v8::Object::New(env->isolate);
    }

    v8::Local<v8::String> resource_name;
    CHECK_TO_STRING(env, context, resource_name, async_resource_name);

    uvimpl::Work* work = uvimpl::Work::New(reinterpret_cast<node_napi_env>(env),
        resource,
        resource_name,
        execute,
        complete,
        data);

    *result = reinterpret_cast<napi_async_work>(work);

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_delete_async_work(napi_env env,
    napi_async_work work)
{
    CHECK_ENV(env);
    env->CheckGCAccess();
    CHECK_ARG(env, work);

    uvimpl::Work::Delete(reinterpret_cast<uvimpl::Work*>(work));

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_get_uv_event_loop(napi_env env, uv_loop_t** loop)
{
    CHECK_ENV(env);
    CHECK_ARG(env, loop);
    *loop = reinterpret_cast<node_napi_env>(env)->node_env()->event_loop();
    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_queue_async_work(napi_env env,
    napi_async_work work)
{
    CHECK_ENV(env);
    CHECK_ARG(env, work);

    uv_loop_t* event_loop = nullptr;
    STATUS_CALL(napi_get_uv_event_loop(env, &event_loop));

    uvimpl::Work* w = reinterpret_cast<uvimpl::Work*>(work);

    w->ScheduleWork();

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL napi_cancel_async_work(napi_env env,
    napi_async_work work)
{
    CHECK_ENV(env);
    CHECK_ARG(env, work);

    uvimpl::Work* w = reinterpret_cast<uvimpl::Work*>(work);

    CALL_UV(env, w->CancelWork());

    return napi_clear_last_error(env);
}

napi_status NAPI_CDECL node_api_get_module_file_name(napi_env env,
    const char** result)
{
    CHECK_ENV(env);
    CHECK_ARG(env, result);

    *result = static_cast<node_napi_env>(env)->GetFilename();
    return napi_clear_last_error(env);
}
