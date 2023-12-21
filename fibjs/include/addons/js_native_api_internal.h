#pragma once

#include <unordered_set>
#include <limits.h>
#include <uv/include/uv.h>

#ifndef NAPI_EXTERN
#ifdef _WIN32
#define NAPI_EXTERN __declspec(dllexport)
#elif defined(__wasm__)
#define NAPI_EXTERN                        \
    __attribute__((visibility("default"))) \
    __attribute__((__import_module__("napi")))
#else
#define NAPI_EXTERN __attribute__((visibility("default"))) __attribute__((used))
#endif
#endif

#define NAPI_EXPERIMENTAL
#include "node_api/js_native_api.h"
#include "node_api/node_api.h"
#include "node_api_internal.h"
#include "util.h"
#include "js_native_api_v8_internals.h"
#include "js_native_api_v8.h"

#define CHECK_ENV(env)               \
    do {                             \
        if ((env) == nullptr) {      \
            return napi_invalid_arg; \
        }                            \
    } while (0)

#define CHECK_ARG(env, arg) \
    RETURN_STATUS_IF_FALSE((env), ((arg) != nullptr), napi_invalid_arg)

#define CHECK_MAYBE_NOTHING(env, maybe, status) \
    RETURN_STATUS_IF_FALSE((env), !((maybe).IsNothing()), (status))

#define CHECK_TO_TYPE(env, type, context, result, src, status)                      \
    do {                                                                            \
        CHECK_ARG((env), (src));                                                    \
        auto maybe = v8impl::V8LocalValueFromJsValue((src)) -> To##type((context)); \
        CHECK_MAYBE_EMPTY((env), maybe, (status));                                  \
        (result) = maybe.ToLocalChecked();                                          \
    } while (0)

#define CHECK_TO_OBJECT(env, context, result, src) \
    CHECK_TO_TYPE((env), Object, (context), (result), (src), napi_object_expected)

#define CHECK_NEW_FROM_UTF8_LEN(env, result, str, len)                             \
    do {                                                                           \
        static_assert(static_cast<int>(NAPI_AUTO_LENGTH) == -1,                    \
            "Casting NAPI_AUTO_LENGTH to int must result in -1");                  \
        RETURN_STATUS_IF_FALSE(                                                    \
            (env), (len == NAPI_AUTO_LENGTH) || len <= INT_MAX, napi_invalid_arg); \
        RETURN_STATUS_IF_FALSE((env), (str) != nullptr, napi_invalid_arg);         \
        auto str_maybe = v8::String::NewFromUtf8((env)->isolate,                   \
            (str),                                                                 \
            v8::NewStringType::kInternalized,                                      \
            static_cast<int>(len));                                                \
        CHECK_MAYBE_EMPTY((env), str_maybe, napi_generic_failure);                 \
        (result) = str_maybe.ToLocalChecked();                                     \
    } while (0)

#define CHECK_NEW_FROM_UTF8(env, result, str) \
    CHECK_NEW_FROM_UTF8_LEN((env), (result), (str), NAPI_AUTO_LENGTH)

#define STATUS_CALL(call)            \
    do {                             \
        napi_status status = (call); \
        if (status != napi_ok)       \
            return status;           \
    } while (0)

#define RETURN_STATUS_IF_FALSE(env, condition, status)   \
    do {                                                 \
        if (!(condition)) {                              \
            return napi_set_last_error((env), (status)); \
        }                                                \
    } while (0)

#define RETURN_STATUS_IF_FALSE_WITH_PREAMBLE(env, condition, status)               \
    do {                                                                           \
        if (!(condition)) {                                                        \
            return napi_set_last_error(                                            \
                (env), try_catch.HasCaught() ? napi_pending_exception : (status)); \
        }                                                                          \
    } while (0)

#define GET_RETURN_STATUS(env) \
    (!try_catch.HasCaught()    \
            ? napi_ok          \
            : napi_set_last_error((env), napi_pending_exception))

#define CHECK_MAYBE_EMPTY(env, maybe, status) \
    RETURN_STATUS_IF_FALSE((env), !((maybe).IsEmpty()), (status))

#define CHECK_MAYBE_EMPTY_WITH_PREAMBLE(env, maybe, status) \
    RETURN_STATUS_IF_FALSE_WITH_PREAMBLE((env), !((maybe).IsEmpty()), (status))

#define CHECK_TO_FUNCTION(env, result, src)                                     \
    do {                                                                        \
        CHECK_ARG((env), (src));                                                \
        v8::Local<v8::Value> v8value = v8impl::V8LocalValueFromJsValue((src));  \
        RETURN_STATUS_IF_FALSE((env), v8value->IsFunction(), napi_invalid_arg); \
        (result) = v8value.As<v8::Function>();                                  \
    } while (0)

#define CHECK_ARG_WITH_PREAMBLE(env, arg) \
    RETURN_STATUS_IF_FALSE_WITH_PREAMBLE( \
        (env), ((arg) != nullptr), napi_invalid_arg)

#define CHECK_TO_TYPE_WITH_PREAMBLE(env, type, context, result, src, status)        \
    do {                                                                            \
        CHECK_ARG_WITH_PREAMBLE((env), (src));                                      \
        auto maybe = v8impl::V8LocalValueFromJsValue((src)) -> To##type((context)); \
        CHECK_MAYBE_EMPTY_WITH_PREAMBLE((env), maybe, (status));                    \
        (result) = maybe.ToLocalChecked();                                          \
    } while (0)

#define CHECK_TO_OBJECT_WITH_PREAMBLE(env, context, result, src) \
    CHECK_TO_TYPE_WITH_PREAMBLE(                                 \
        (env), Object, (context), (result), (src), napi_object_expected)

#define CHECK_TO_STRING(env, context, result, src) \
    CHECK_TO_TYPE((env), String, (context), (result), (src), napi_string_expected)

#define THROW_RANGE_ERROR_IF_FALSE(env, condition, error, message)   \
    do {                                                             \
        if (!(condition)) {                                          \
            napi_throw_range_error((env), (error), (message));       \
            return napi_set_last_error((env), napi_generic_failure); \
        }                                                            \
    } while (0)

#define NAPI_PREAMBLE(env)                                               \
    CHECK_ENV((env));                                                    \
    RETURN_STATUS_IF_FALSE(                                              \
        (env), (env)->last_exception.IsEmpty(), napi_pending_exception); \
    RETURN_STATUS_IF_FALSE((env),                                        \
        (env)->can_call_into_js(),                                       \
        (env->module_api_version == NAPI_VERSION_EXPERIMENTAL            \
                ? napi_cannot_run_js                                     \
                : napi_pending_exception));                              \
    napi_clear_last_error((env));                                        \
    v8impl::TryCatch try_catch((env))

#define NAPI_ARRAYSIZE ARRAYSIZE

inline v8::Local<v8::Private> napi_private_key(v8::Local<v8::Context> context, const char* name)
{
    v8::Isolate* isolate = context->GetIsolate();
    return v8::Private::ForApi(isolate,
        v8::String::NewFromUtf8(isolate, name).FromMaybe(v8::Local<v8::String>()));
}

#define NAPI_PRIVATE_KEY(context, suffix) napi_private_key(context, #suffix)

inline napi_status napi_clear_last_error(napi_env env);
inline napi_status napi_set_last_error(napi_env env, napi_status error_code,
    uint32_t engine_error_code = 0, void* engine_reserved = nullptr);

namespace v8impl {

//=== Conversion between V8 Handles and napi_value ========================

// This asserts v8::Local<> will always be implemented with a single
// pointer field so that we can pass it around as a void*.
static_assert(sizeof(v8::Local<v8::Value>) == sizeof(napi_value),
    "Cannot convert between v8::Local<v8::Value> and napi_value");

/**
 * A PersistentBase which has move semantics.
 *
 * Note: Persistent class hierarchy is subject to future changes.
 */
template <class T>
class Global : public v8::PersistentBase<T> {
public:
    /**
     * A Global with no storage cell.
     */
    V8_INLINE Global() = default;

    /**
     * Construct a Global from a Local.
     * When the Local is non-empty, a new storage cell is created
     * pointing to the same object, and no flags are set.
     */
    template <class S>
    V8_INLINE Global(v8::Isolate* isolate, v8::Local<S> that)
        : v8::PersistentBase<T>(
            v8::PersistentBase<T>::New(isolate, that.template value<S>()))
    {
        static_assert(std::is_base_of<T, S>::value, "type check");
    }

    /**
     * Construct a Global from a PersistentBase.
     * When the Persistent is non-empty, a new storage cell is created
     * pointing to the same object, and no flags are set.
     */
    template <class S>
    V8_INLINE Global(v8::Isolate* isolate, const v8::PersistentBase<S>& that)
        : v8::PersistentBase<T>(
            v8::PersistentBase<T>::New(isolate, that.template value<S>()))
    {
        static_assert(std::is_base_of<T, S>::value, "type check");
    }

    /**
     * Move constructor.
     */
    V8_INLINE Global(Global&& other);

    V8_INLINE ~Global() { this->Reset(); }

    /**
     * Move via assignment.
     */
    template <class S>
    V8_INLINE Global& operator=(Global<S>&& rhs);

    /**
     * Pass allows returning uniques from functions, etc.
     */
    Global Pass() { return static_cast<Global&&>(*this); }

    /*
     * For compatibility with Chromium's base::Bind (base::Passed).
     */
    using MoveOnlyTypeForCPP03 = void;

    Global(const Global&) = delete;
    void operator=(const Global&) = delete;

private:
    template <class F>
    friend class ReturnValue;
};

template <typename T>
using Persistent = v8::Global<T>;

}

inline napi_status napi_clear_last_error(napi_env env)
{
    env->last_error.error_code = napi_ok;
    env->last_error.engine_error_code = 0;
    env->last_error.engine_reserved = nullptr;
    env->last_error.error_message = nullptr;
    return napi_ok;
}

inline napi_status napi_set_last_error(napi_env env, napi_status error_code,
    uint32_t engine_error_code, void* engine_reserved)
{
    env->last_error.error_code = error_code;
    env->last_error.engine_error_code = engine_error_code;
    env->last_error.engine_reserved = engine_reserved;
    return error_code;
}

namespace v8impl {

class CallbackBundle {
public:
    // Creates an object to be made available to the static function callback
    // wrapper, used to retrieve the native callback function and data pointer.
    static inline v8::Local<v8::Value> New(napi_env env,
        napi_callback cb,
        void* data)
    {
        CallbackBundle* bundle = new CallbackBundle();
        bundle->cb = cb;
        bundle->cb_data = data;
        bundle->env = env;

        v8::Local<v8::Value> cbdata = v8::External::New(env->isolate, bundle);
        Reference::New(
            env, cbdata, 0, Ownership::kRuntime, Delete, bundle, nullptr);
        return cbdata;
    }
    napi_env env; // Necessary to invoke C++ NAPI callback
    void* cb_data; // The user provided callback data
    napi_callback cb;

private:
    static void Delete(napi_env env, void* data, void* hint)
    {
        CallbackBundle* bundle = static_cast<CallbackBundle*>(data);
        delete bundle;
    }
};

class CallbackWrapper {
public:
    inline CallbackWrapper(napi_value this_arg, size_t args_length, void* data)
        : _this(this_arg)
        , _args_length(args_length)
        , _data(data)
    {
    }

    virtual napi_value GetNewTarget() = 0;
    virtual void Args(napi_value* buffer, size_t bufferlength) = 0;
    virtual void SetReturnValue(napi_value value) = 0;

    napi_value This() { return _this; }

    size_t ArgsLength() { return _args_length; }

    void* Data() { return _data; }

protected:
    const napi_value _this;
    const size_t _args_length;
    void* _data;
};

class CallbackWrapperBase : public CallbackWrapper {
public:
    inline CallbackWrapperBase(const v8::FunctionCallbackInfo<v8::Value>& cbinfo,
        const size_t args_length)
        : CallbackWrapper(
            JsValueFromV8LocalValue(cbinfo.This()), args_length, nullptr)
        , _cbinfo(cbinfo)
    {
        _bundle = reinterpret_cast<CallbackBundle*>(
            cbinfo.Data().As<v8::External>()->Value());
        _data = _bundle->cb_data;
    }

protected:
    inline void InvokeCallback()
    {
        napi_callback_info cbinfo_wrapper = reinterpret_cast<napi_callback_info>(
            static_cast<CallbackWrapper*>(this));

        // All other pointers we need are stored in `_bundle`
        napi_env env = _bundle->env;
        napi_callback cb = _bundle->cb;

        napi_value result = nullptr;
        bool exceptionOccurred = false;
        env->CallIntoModule([&](napi_env env) { result = cb(env, cbinfo_wrapper); },
            [&](napi_env env, v8::Local<v8::Value> value) {
                exceptionOccurred = true;
                if (env->terminatedOrTerminating()) {
                    return;
                }
                env->isolate->ThrowException(value);
            });

        if (!exceptionOccurred && (result != nullptr)) {
            this->SetReturnValue(result);
        }
    }

    const v8::FunctionCallbackInfo<v8::Value>& _cbinfo;
    CallbackBundle* _bundle;
};

class FunctionCallbackWrapper : public CallbackWrapperBase {
public:
    static void Invoke(const v8::FunctionCallbackInfo<v8::Value>& info)
    {
        FunctionCallbackWrapper cbwrapper(info);
        cbwrapper.InvokeCallback();
    }

    static inline napi_status NewFunction(napi_env env,
        napi_callback cb,
        void* cb_data,
        v8::Local<v8::Function>* result)
    {
        v8::Local<v8::Value> cbdata = v8impl::CallbackBundle::New(env, cb, cb_data);
        RETURN_STATUS_IF_FALSE(env, !cbdata.IsEmpty(), napi_generic_failure);

        v8::MaybeLocal<v8::Function> maybe_function = v8::Function::New(env->context(), Invoke, cbdata);
        CHECK_MAYBE_EMPTY(env, maybe_function, napi_generic_failure);

        *result = maybe_function.ToLocalChecked();
        return napi_clear_last_error(env);
    }

    static inline napi_status NewTemplate(
        napi_env env,
        napi_callback cb,
        void* cb_data,
        v8::Local<v8::FunctionTemplate>* result,
        v8::Local<v8::Signature> sig = v8::Local<v8::Signature>())
    {
        v8::Local<v8::Value> cbdata = v8impl::CallbackBundle::New(env, cb, cb_data);
        RETURN_STATUS_IF_FALSE(env, !cbdata.IsEmpty(), napi_generic_failure);

        *result = v8::FunctionTemplate::New(env->isolate, Invoke, cbdata, sig);
        return napi_clear_last_error(env);
    }

    explicit FunctionCallbackWrapper(
        const v8::FunctionCallbackInfo<v8::Value>& cbinfo)
        : CallbackWrapperBase(cbinfo, cbinfo.Length())
    {
    }

    napi_value GetNewTarget() override
    {
        if (_cbinfo.IsConstructCall()) {
            return v8impl::JsValueFromV8LocalValue(_cbinfo.NewTarget());
        } else {
            return nullptr;
        }
    }

    /*virtual*/
    void Args(napi_value* buffer, size_t buffer_length) override
    {
        size_t i = 0;
        size_t min = std::min(buffer_length, _args_length);

        for (; i < min; i += 1) {
            buffer[i] = v8impl::JsValueFromV8LocalValue(_cbinfo[i]);
        }

        if (i < buffer_length) {
            napi_value undefined = v8impl::JsValueFromV8LocalValue(v8::Undefined(_cbinfo.GetIsolate()));
            for (; i < buffer_length; i += 1) {
                buffer[i] = undefined;
            }
        }
    }

    /*virtual*/
    void SetReturnValue(napi_value value) override
    {
        v8::Local<v8::Value> val = v8impl::V8LocalValueFromJsValue(value);
        _cbinfo.GetReturnValue().Set(val);
    }
};

template <typename CCharType, typename StringMaker>
napi_status NewString(napi_env env,
    const CCharType* str,
    size_t length,
    napi_value* result,
    StringMaker string_maker)
{
    CHECK_ENV(env);
    if (length > 0)
        CHECK_ARG(env, str);
    CHECK_ARG(env, result);
    RETURN_STATUS_IF_FALSE(
        env, (length == NAPI_AUTO_LENGTH) || length <= INT_MAX, napi_invalid_arg);

    auto isolate = env->isolate;
    auto str_maybe = string_maker(isolate);
    CHECK_MAYBE_EMPTY(env, str_maybe, napi_generic_failure);
    *result = v8impl::JsValueFromV8LocalValue(str_maybe.ToLocalChecked());
    return napi_clear_last_error(env);
}

template <typename CharType, typename CreateAPI, typename StringMaker>
napi_status NewExternalString(napi_env env,
    CharType* str,
    size_t length,
    napi_finalize finalize_callback,
    void* finalize_hint,
    napi_value* result,
    bool* copied,
    CreateAPI create_api,
    StringMaker string_maker)
{
    napi_status status;
    status = NewString(env, str, length, result, string_maker);
    if (status == napi_ok && copied != nullptr) {
        *copied = false;
    }
    return status;
}

inline napi_status V8NameFromPropertyDescriptor(
    napi_env env,
    const napi_property_descriptor* p,
    v8::Local<v8::Name>* result)
{
    if (p->utf8name != nullptr) {
        CHECK_NEW_FROM_UTF8(env, *result, p->utf8name);
    } else {
        v8::Local<v8::Value> property_value = v8impl::V8LocalValueFromJsValue(p->name);

        RETURN_STATUS_IF_FALSE(env, property_value->IsName(), napi_name_expected);
        *result = property_value.As<v8::Name>();
    }

    return napi_ok;
}

// convert from n-api property attributes to v8::PropertyAttribute
inline v8::PropertyAttribute V8PropertyAttributesFromDescriptor(
    const napi_property_descriptor* descriptor)
{
    unsigned int attribute_flags = v8::PropertyAttribute::None;

    // The napi_writable attribute is ignored for accessor descriptors, but
    // V8 would throw `TypeError`s on assignment with nonexistence of a setter.
    if ((descriptor->getter == nullptr && descriptor->setter == nullptr) && (descriptor->attributes & napi_writable) == 0) {
        attribute_flags |= v8::PropertyAttribute::ReadOnly;
    }

    if ((descriptor->attributes & napi_enumerable) == 0) {
        attribute_flags |= v8::PropertyAttribute::DontEnum;
    }
    if ((descriptor->attributes & napi_configurable) == 0) {
        attribute_flags |= v8::PropertyAttribute::DontDelete;
    }

    return static_cast<v8::PropertyAttribute>(attribute_flags);
}
}

#include "node_api_internals.h"
