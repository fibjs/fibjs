#pragma once

#include <unordered_set>
#include <limits.h>
#include <uv/include/uv.h>

#define NAPI_EXPERIMENTAL
#include "js_native_api.h"
#include "node_api.h"
#include "node_api_internal.h"

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

#define CHECK_TO_TYPE(env, type, context, result, src, status)                    \
    do {                                                                          \
        CHECK_ARG((env), (src));                                                  \
        auto maybe = v8impl::V8LocalValueFromJsValue((src))->To##type((context)); \
        CHECK_MAYBE_EMPTY((env), maybe, (status));                                \
        (result) = maybe.ToLocalChecked();                                        \
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

#define CHECK_TO_TYPE_WITH_PREAMBLE(env, type, context, result, src, status)      \
    do {                                                                          \
        CHECK_ARG_WITH_PREAMBLE((env), (src));                                    \
        auto maybe = v8impl::V8LocalValueFromJsValue((src))->To##type((context)); \
        CHECK_MAYBE_EMPTY_WITH_PREAMBLE((env), maybe, (status));                  \
        (result) = maybe.ToLocalChecked();                                        \
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

inline napi_value JsValueFromV8LocalValue(v8::Local<v8::Value> local)
{
    return reinterpret_cast<napi_value>(*local);
}

inline v8::Local<v8::Value> V8LocalValueFromJsValue(napi_value v)
{
    v8::Local<v8::Value> local;
    memcpy(static_cast<void*>(&local), &v, sizeof(v));
    return local;
}

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

class RefTracker {
public:
    RefTracker() = default;
    virtual ~RefTracker() = default;
    virtual void Finalize() { }

    typedef RefTracker RefList;

    inline void Link(RefList* list)
    {
        prev_ = list;
        next_ = list->next_;
        if (next_ != nullptr) {
            next_->prev_ = this;
        }
        list->next_ = this;
    }

    inline void Unlink()
    {
        if (prev_ != nullptr) {
            prev_->next_ = next_;
        }
        if (next_ != nullptr) {
            next_->prev_ = prev_;
        }
        prev_ = nullptr;
        next_ = nullptr;
    }

    static void FinalizeAll(RefList* list)
    {
        while (list->next_ != nullptr) {
            list->next_->Finalize();
        }
    }

private:
    RefList* next_ = nullptr;
    RefList* prev_ = nullptr;
};

class PersistentToLocal {
public:
    // If persistent.IsWeak() == false, then do not call persistent.Reset()
    // while the returned Local<T> is still in scope, it will destroy the
    // reference to the object.
    template <class TypeName>
    static inline v8::Local<TypeName> Default(
        v8::Isolate* isolate,
        const v8::PersistentBase<TypeName>& persistent)
    {
        if (persistent.IsWeak()) {
            return PersistentToLocal::Weak(isolate, persistent);
        } else {
            return PersistentToLocal::Strong(persistent);
        }
    }

    // Unchecked conversion from a non-weak Persistent<T> to Local<T>,
    // use with care!
    //
    // Do not call persistent.Reset() while the returned Local<T> is still in
    // scope, it will destroy the reference to the object.
    template <class TypeName>
    static inline v8::Local<TypeName> Strong(
        const v8::PersistentBase<TypeName>& persistent)
    {
        DCHECK(!persistent.IsWeak());
        return *reinterpret_cast<v8::Local<TypeName>*>(
            const_cast<v8::PersistentBase<TypeName>*>(&persistent));
    }

    template <class TypeName>
    static inline v8::Local<TypeName> Weak(
        v8::Isolate* isolate,
        const v8::PersistentBase<TypeName>& persistent)
    {
        return v8::Local<TypeName>::New(isolate, persistent);
    }
};
}

struct napi_env__ {
    explicit napi_env__(v8::Local<v8::Context> context,
        int32_t module_api_version)
        : isolate(context->GetIsolate())
        , context_persistent(isolate, context)
        , module_api_version(module_api_version)
    {
        napi_clear_last_error(this);
    }

    inline v8::Local<v8::Context> context() const
    {
        return v8impl::PersistentToLocal::Strong(context_persistent);
    }

    inline void Ref() { refs++; }
    inline void Unref()
    {
        if (--refs == 0)
            DeleteMe();
    }

    virtual bool can_call_into_js() const { return true; }

    static inline void HandleThrow(napi_env env, v8::Local<v8::Value> value)
    {
        if (env->terminatedOrTerminating()) {
            return;
        }
        env->isolate->ThrowException(value);
    }

    // i.e. whether v8 exited or is about to exit
    inline bool terminatedOrTerminating()
    {
        return this->isolate->IsExecutionTerminating() || !can_call_into_js();
    }

    // v8 uses a special exception to indicate termination, the
    // `handle_exception` callback should identify such case using
    // terminatedOrTerminating() before actually handle the exception
    template <typename T, typename U = decltype(HandleThrow)>
    inline void CallIntoModule(T&& call, U&& handle_exception = HandleThrow)
    {
        int open_handle_scopes_before = open_handle_scopes;
        int open_callback_scopes_before = open_callback_scopes;
        napi_clear_last_error(this);
        call(this);
        CHECK_EQ(open_handle_scopes, open_handle_scopes_before);
        CHECK_EQ(open_callback_scopes, open_callback_scopes_before);
        if (!last_exception.IsEmpty()) {
            handle_exception(this, last_exception.Get(this->isolate));
            last_exception.Reset();
        }
    }

    // Call finalizer immediately.
    virtual void CallFinalizer(napi_finalize cb, void* data, void* hint)
    {
        v8::HandleScope handle_scope(isolate);
        CallIntoModule([&](napi_env env) { cb(env, data, hint); });
    }

    // Invoke finalizer from V8 garbage collector.
    void InvokeFinalizerFromGC(v8impl::RefTracker* finalizer);

    // Enqueue the finalizer to the napi_env's own queue of the second pass
    // weak callback.
    // Implementation should drain the queue at the time it is safe to call
    // into JavaScript.
    virtual void EnqueueFinalizer(v8impl::RefTracker* finalizer)
    {
        pending_finalizers.emplace(finalizer);
    }

    // Remove the finalizer from the scheduled second pass weak callback queue.
    // The finalizer can be deleted after this call.
    virtual void DequeueFinalizer(v8impl::RefTracker* finalizer)
    {
        pending_finalizers.erase(finalizer);
    }

    virtual void DeleteMe()
    {
        // First we must finalize those references that have `napi_finalizer`
        // callbacks. The reason is that addons might store other references which
        // they delete during their `napi_finalizer` callbacks. If we deleted such
        // references here first, they would be doubly deleted when the
        // `napi_finalizer` deleted them subsequently.
        v8impl::RefTracker::FinalizeAll(&finalizing_reflist);
        v8impl::RefTracker::FinalizeAll(&reflist);
        delete this;
    }

    void CheckGCAccess()
    {
    }

    v8::Isolate* const isolate; // Shortcut for context()->GetIsolate()
    v8impl::Persistent<v8::Context> context_persistent;

    v8impl::Persistent<v8::Value> last_exception;

    // We store references in two different lists, depending on whether they have
    // `napi_finalizer` callbacks, because we must first finalize the ones that
    // have such a callback. See `~napi_env__()` above for details.
    v8impl::RefTracker::RefList reflist;
    v8impl::RefTracker::RefList finalizing_reflist;
    // The invocation order of the finalizers is not determined.
    std::unordered_set<v8impl::RefTracker*> pending_finalizers;
    napi_extended_error_info last_error;
    int open_handle_scopes = 0;
    int open_callback_scopes = 0;
    int refs = 1;
    void* instance_data = nullptr;
    int32_t module_api_version = NODE_API_DEFAULT_MODULE_API_VERSION;
    bool in_gc_finalizer = false;

protected:
    // Should not be deleted directly. Delete with `napi_env__::DeleteMe()`
    // instead.
    virtual ~napi_env__() = default;
};

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
class TryCatch : public v8::TryCatch {
public:
    explicit TryCatch(napi_env env)
        : v8::TryCatch(env->isolate)
        , _env(env)
    {
    }

    ~TryCatch()
    {
        if (HasCaught()) {
            _env->last_exception.Reset(_env->isolate, Exception());
        }
    }

private:
    napi_env _env;
};

class Finalizer {
protected:
    Finalizer(napi_env env,
        napi_finalize finalize_callback,
        void* finalize_data,
        void* finalize_hint)
        : env_(env)
        , finalize_callback_(finalize_callback)
        , finalize_data_(finalize_data)
        , finalize_hint_(finalize_hint)
    {
    }

    virtual ~Finalizer() = default;

public:
    static Finalizer* New(napi_env env,
        napi_finalize finalize_callback = nullptr,
        void* finalize_data = nullptr,
        void* finalize_hint = nullptr)
    {
        return new Finalizer(env, finalize_callback, finalize_data, finalize_hint);
    }

    napi_finalize callback() { return finalize_callback_; }
    void* data() { return finalize_data_; }
    void* hint() { return finalize_hint_; }

    void ResetFinalizer();

protected:
    napi_env env_;
    napi_finalize finalize_callback_;
    void* finalize_data_;
    void* finalize_hint_;
};

enum class Ownership {
    // The reference is owned by the runtime. No userland call is needed to
    // destruct the reference.
    kRuntime,
    // The reference is owned by the userland. User code is responsible to delete
    // the reference with appropriate node-api calls.
    kUserland,
};

class TrackedFinalizer : public Finalizer, public RefTracker {
protected:
    TrackedFinalizer(napi_env env,
        napi_finalize finalize_callback,
        void* finalize_data,
        void* finalize_hint);

public:
    static TrackedFinalizer* New(napi_env env,
        napi_finalize finalize_callback,
        void* finalize_data,
        void* finalize_hint);
    ~TrackedFinalizer() override;

protected:
    void Finalize() override;
    void FinalizeCore(bool deleteMe);
};

// Wrapper around TrackedFinalizer that implements reference counting.
class RefBase : public TrackedFinalizer {
protected:
    RefBase(napi_env env,
        uint32_t initial_refcount,
        Ownership ownership,
        napi_finalize finalize_callback,
        void* finalize_data,
        void* finalize_hint);

public:
    static RefBase* New(napi_env env,
        uint32_t initial_refcount,
        Ownership ownership,
        napi_finalize finalize_callback,
        void* finalize_data,
        void* finalize_hint);

    void* Data();
    uint32_t Ref();
    uint32_t Unref();
    uint32_t RefCount();

    Ownership ownership() { return ownership_; }

protected:
    void Finalize() override;

private:
    uint32_t refcount_;
    Ownership ownership_;
};

// Wrapper around v8impl::Persistent.
class Reference : public RefBase {
protected:
    template <typename... Args>
    Reference(napi_env env, v8::Local<v8::Value> value, Args&&... args);

public:
    static Reference* New(napi_env env,
        v8::Local<v8::Value> value,
        uint32_t initial_refcount,
        Ownership ownership,
        napi_finalize finalize_callback = nullptr,
        void* finalize_data = nullptr,
        void* finalize_hint = nullptr);

    virtual ~Reference();
    uint32_t Ref();
    uint32_t Unref();
    v8::Local<v8::Value> Get();

protected:
    void Finalize() override;

private:
    static void WeakCallback(const v8::WeakCallbackInfo<Reference>& data);

    void SetWeak();

    v8impl::Persistent<v8::Value> persistent_;
    bool can_be_weak_;
};

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

struct node_napi_env__ : public napi_env__ {
    node_napi_env__(v8::Local<v8::Context> context,
        const std::string& module_filename,
        int32_t module_api_version);

    bool can_call_into_js() const override;
    void CallFinalizer(napi_finalize cb, void* data, void* hint) override;
    template <bool enforceUncaughtExceptionPolicy>
    void CallFinalizer(napi_finalize cb, void* data, void* hint);

    void EnqueueFinalizer(v8impl::RefTracker* finalizer) override;
    void DrainFinalizerQueue();

    void trigger_fatal_exception(v8::Local<v8::Value> local_err);
    template <bool enforceUncaughtExceptionPolicy, typename T>
    void CallbackIntoModule(T&& call);

    void DeleteMe() override;

    inline node::Environment* node_env() const
    {
        return node::Environment::GetCurrent(context());
    }
    inline const char* GetFilename() const { return filename.c_str(); }

    std::string filename;
    bool destructing = false;
    bool finalization_scheduled = false;
};

using node_napi_env = node_napi_env__*;
