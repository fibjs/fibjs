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
#include "node_mutex.h"
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

#include "node_api_internals.h"
