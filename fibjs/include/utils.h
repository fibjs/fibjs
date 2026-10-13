/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <uv/include/uv.h>
#include <errno.h>

#ifdef _WIN32

static_assert(sizeof(wchar_t) == 2, "wchar_t size is not 2 bytes");

#include <windows.h>

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif // IPV6_V6ONLY

#ifndef EWOULDBLOCK
#define EINPROGRESS WSAEWOULDBLOCK
#endif // EWOULDBLOCK

#define SHUT_RD SD_READ
#define SHUT_WR SD_SEND
#define SHUT_RDWR SD_BOTH

#else // _WIN32

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef int32_t SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket close

#if defined(FreeBSD) || defined(OpenBSD)
#include <net/if.h>
#include <netinet/tcp.h>

#ifndef ETIME
#define ETIME ETIMEDOUT
#endif // ETIME

#else // FreeBSD

#include <netinet/tcp.h>

#ifndef TCP_KEEPIDLE
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif // TCP_KEEPIDLE

#endif // FreeBSD

#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif // SOL_TCP

#endif // _WIN32

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif // MSG_NOSIGNAL

#include "qstring.h"

#include "result.h"

#include <v8.h>
#include "v8_api.h"

#include "obj_ptr.h"
#include <Isolate.h>
#include <JSType.h>

#include <cmath>
#include <vector>
#include <variant>
#include <utility>

#ifdef _WIN32

#ifndef INFINITY
#define INFINITY (DBL_MAX + DBL_MAX)
#endif // INFINITY

#ifndef NAN
#define NAN (INFINITY - INFINITY)
#endif // NAN

#endif // _WIN32

#include <exlib/include/service.h>
#include <exlib/include/ex_assert.h>

#include "date.h"
#include "Variant.h"

#ifdef _DEBUG
#undef _DEBUG
#endif // _DEBUG

namespace fibjs {

// Canonical error class names. The name is what JS sees as err.name and it is
// the single error-type channel: it rides in ErrorPayload, so it survives
// crossing a thread or fiber boundary (per-thread state does not).
namespace errtype {
constexpr const char* kError = "";
constexpr const char* kTypeError = "TypeError";
constexpr const char* kRangeError = "RangeError";
constexpr const char* kSyntaxError = "SyntaxError";
constexpr const char* kReferenceError = "ReferenceError";
constexpr const char* kURIError = "URIError";
constexpr const char* kEvalError = "EvalError";
constexpr const char* kAbortError = "AbortError";
constexpr const char* kTimeoutError = "TimeoutError";
}

// result_t and the CALL_E_* code space live in result.h so that Isolate.h --
// which this header includes -- can name the error channel as well.
#define STREAM_BUFF_SIZE 65536

enum {
    kObjectPrototype = 0,
    kBufferClassIndex = 1,
    kBufferPrototype = 2,
    kSandboxObject = 3,
    kBufferModule = 4
};

#if 1
#define V8_SCOPE(isolate) v8::EscapableHandleScope handle_scope(isolate)
#define V8_RETURN(v) handle_scope.Escape(v)
#else
#define V8_SCOPE(isolate) (isolate = isolate)
#define V8_RETURN(v) (v)
#endif

#define METHOD_NAME(name) save_method_name _save_method_name(name)

#define PROPERTY_ENTER()                                    \
    Isolate* isolate = Isolate::current(args.GetIsolate()); \
    V8_SCOPE(isolate->m_isolate);                           \
    result_t hr;                                        \
    result_t hr_argc;                                   \
    bool bStrict = false;                                   \
    do {                                                    \
        do {

#define METHOD_OVER(c, o)                                                                             \
    }                                                                                                 \
    while (0)                                                                                         \
        ;                                                                                             \
    if (hr.is_overload_mismatch())                                                                     \
        do {                                                                                          \
            /* A variant whose argument count does not fit must not overwrite the error of a  */      \
            /* variant that did pass that check: the error of the variant that got further is */      \
            /* the useful diagnosis (an arity error only carries when nothing got past it).    */     \
            result_t hr_prev = hr;                                                                    \
            hr = result_t();                                                                                   \
            int32_t argc = argc1;                                                                     \
            if (!bStrict)                                                                             \
                while (argc > (o) && (args[argc - 1]->IsUndefined() || args[argc - 1]->IsNull()))     \
                    argc--;                                                                           \
            if ((c) >= 0 && argc > (c)) {                                                             \
                if (hr_prev.is(0) || hr_prev.is(CALL_E_BADPARAMCOUNT) || hr_prev.is(CALL_E_PARAMNOTOPTIONAL)) { \
                    hr = result_t(CALL_E_BADPARAMCOUNT);                                                        \
                    hr_argc = result_t(CALL_E_BADPARAMCOUNT);                                                   \
                } else                                                                                \
                    hr = hr_prev;                                                                     \
                break;                                                                                \
            }                                                                                         \
            if ((o) > 0 && argc < (o)) {                                                              \
                if (hr_prev.is(0) || hr_prev.is(CALL_E_BADPARAMCOUNT) || hr_prev.is(CALL_E_PARAMNOTOPTIONAL)) { \
                    hr = setRuntimeError(CALL_E_PARAMNOTOPTIONAL);                                    \
                    hr_argc = result_t(CALL_E_PARAMNOTOPTIONAL);                                                \
                } else                                                                                \
                    hr = hr_prev;                                                                     \
                break;                                                                                \
            }                                                                                         \


#define METHOD_ENTER()                                      \
    Isolate* isolate = Isolate::current(args.GetIsolate()); \
    V8_SCOPE(isolate->m_isolate);                           \
    result_t hr(CALL_E_BADPARAMCOUNT);                     \
    result_t hr_argc;                                   \
    bool bStrict = true;                                    \
    int32_t argc1 = args.Length();                          \
    do {                                                    \
        do {

#define ASYNC_METHOD_ENTER(name)                                                                           \
    METHOD_NAME(name);                                                                                     \
    Isolate* isolate = Isolate::current(args.GetIsolate());                                                \
    V8_SCOPE(isolate->m_isolate);                                                                          \
    result_t hr(CALL_E_BADPARAMCOUNT);                                                                    \
    result_t hr_argc;                                                                                  \
    bool bStrict = true;                                                                                   \
    int32_t argc1 = args.Length();                                                                         \
    v8::Local<v8::Object> cb;                                                                              \
    if (args.Data()->IsTrue())                                                                             \
        cb = v8::Promise::Resolver::New(isolate->context()).FromMaybe(v8::Local<v8::Promise::Resolver>()); \
    else if (argc1 > 0 && args[argc1 - 1]->IsFunction())                                                   \
        cb = args[--argc1].As<v8::Object>();                                                               \
    do {                                                                                                   \
        do {

#define ASYNC_METHOD_ENTER_FUNC(name)                                                                      \
    METHOD_NAME(name);                                                                                     \
    Isolate* isolate = Isolate::current(args.GetIsolate());                                                \
    V8_SCOPE(isolate->m_isolate);                                                                          \
    result_t hr(CALL_E_BADPARAMCOUNT);                                                                    \
    result_t hr_argc;                                                                                  \
    bool bStrict = true;                                                                                   \
    int32_t argc1 = args.Length();                                                                         \
    v8::Local<v8::Object> cb;                                                                              \
    if (args.Data()->IsTrue())                                                                             \
        cb = v8::Promise::Resolver::New(isolate->context()).FromMaybe(v8::Local<v8::Promise::Resolver>()); \
    do {                                                                                                   \
        do {

#define CONSTRUCT_INIT()                       \
    Isolate* isolate = Isolate::current(args); \
    if (class_info().init_isolate(isolate))    \
        return;

#define CONSTRUCT_ENTER()                \
    if (!args.IsConstructCall()) {       \
        ThrowResult(result_t(CALL_E_CONSTRUCTOR)); \
        return;                          \
    }                                    \
    METHOD_ENTER()

#define METHOD_INSTANCE(cls)                    \
    cls* pInst = cls::getInstance(args.This()); \
    if (pInst == NULL) {                        \
        ThrowResult(result_t(CALL_E_NOTINSTANCE));        \
        return;                                 \
    }

#define NAMED_METHOD_INSTANCE(cls)              \
    cls* pInst = cls::getInstance(args.This()); \
    if (pInst == NULL) {                        \
        ThrowResult(result_t(CALL_E_NOTINSTANCE));        \
        return v8::Intercepted::kNo;            \
    }

#define ASYNC_METHOD_INSTANCE(cls) \
    METHOD_INSTANCE(cls)           \
    scope l(pInst);

#define LOAD_ENTER()                       \
    result_t hr(CALL_E_BADPARAMCOUNT);    \
    result_t hr_argc;                  \
    Isolate* isolate = Isolate::current(); \
    bool bStrict = true;                   \
    int32_t argc1 = 1;                     \
    OptArgs args(v);                       \
    do {                                   \
        do {

#define LOAD_RETURN()    \
    CHECK_ARGUMENT()     \
    if (hr.ok()) {       \
        retVal = vr;     \
        return result_t(); \
    }                    \
    return hr;

#define CHECK_ARGUMENT()                                                                                        \
    }                                                                                                           \
    while (0)                                                                                                   \
        ;                                                                                                       \
    if (!bStrict || (!hr.is_arg_error() && !hr_argc.is(CALL_E_BADPARAMCOUNT) && !hr_argc.is_param_missing())) \
        break;                                                                                          \
    bStrict = false;                                                                                            \
    }                                                                                                           \
    while (true)                                                                                                \
        ;

#define PROPERTY_SET_LEAVE() \
    CHECK_ARGUMENT()         \
    if (hr.failed())              \
        ThrowResult(hr);

#define THROW_ERROR()                                                 \
    if (hr.is_javascript()) {                                    \
        args.GetReturnValue().Set(V8_RETURN(v8::Local<v8::Value>())); \
        return;                                                       \
    }                                                                 \
    ThrowResult(hr);                                                  \
    return;

#define NAMED_THROW_ERROR()                                           \
    if (hr.is_javascript()) {                                    \
        args.GetReturnValue().Set(V8_RETURN(v8::Local<v8::Value>())); \
        return v8::Intercepted::kNo;                                  \
    }                                                                 \
    ThrowResult(hr);                                                  \
    return v8::Intercepted::kNo;

#define METHOD_RETURN()                                                    \
    CHECK_ARGUMENT()                                                       \
    if (hr.is_return_undefined())                                       \
        return;                                                            \
    if (hr.is_return_null()) {                                          \
        args.GetReturnValue().SetNull();                                   \
        return;                                                            \
    }                                                                      \
    if (hr.ok()) {                                                         \
        args.GetReturnValue().Set(V8_RETURN(GetReturnValue(isolate, vr))); \
        return;                                                            \
    }                                                                      \
    THROW_ERROR()

#define ASYNC_METHOD_RETURN()                                                  \
    CHECK_ARGUMENT()                                                           \
    if (hr.is_return_undefined())                                           \
        return;                                                                \
    if (hr.is_return_null()) {                                              \
        args.GetReturnValue().SetNull();                                       \
        return;                                                                \
    }                                                                          \
    if (hr.ok()) {                                                             \
        args.GetReturnValue().Set(V8_RETURN(GetReturnValue(isolate, vr)));     \
        return;                                                                \
    }                                                                          \
    if (!cb.IsEmpty() && args.Data()->IsTrue()) {                              \
        v8::Local<v8::Promise::Resolver> _resolver =                           \
            cb.As<v8::Promise::Resolver>();                                     \
        _resolver->Reject(isolate->context(), FillError(hr)).IsJust();         \
        args.GetReturnValue().Set(_resolver->GetPromise());                    \
        return;                                                                \
    }                                                                          \
    THROW_ERROR()

#define METHOD_RETURN1()                      \
    CHECK_ARGUMENT()                          \
    args.GetReturnValue().Set(V8_RETURN(vr)); \
    return;

#define METHOD_RETURN2()                      \
    CHECK_ARGUMENT()                          \
    args.GetReturnValue().Set(V8_RETURN(vr)); \
    return v8::Intercepted::kNo;

#define NAMED_METHOD_RETURN()                                              \
    CHECK_ARGUMENT()                                                       \
    if (hr.is_return_undefined())                                       \
        return v8::Intercepted::kNo;                                       \
    if (hr.is_return_null()) {                                          \
        args.GetReturnValue().SetNull();                                   \
        return v8::Intercepted::kNo;                                       \
    }                                                                      \
    if (hr.ok()) {                                                         \
        args.GetReturnValue().Set(V8_RETURN(GetReturnValue(isolate, vr))); \
        return v8::Intercepted::kYes;                                      \
    }                                                                      \
    NAMED_THROW_ERROR()

#define METHOD_VOID() \
    CHECK_ARGUMENT()  \
    if (hr.ok())      \
        return;       \
    THROW_ERROR()

#define ASYNC_METHOD_VOID()                                                    \
    CHECK_ARGUMENT()                                                           \
    if (hr.ok())                                                               \
        return;                                                                \
    if (!cb.IsEmpty() && args.Data()->IsTrue()) {                              \
        v8::Local<v8::Promise::Resolver> _resolver =                           \
            cb.As<v8::Promise::Resolver>();                                     \
        _resolver->Reject(isolate->context(), FillError(hr)).IsJust();         \
        args.GetReturnValue().Set(_resolver->GetPromise());                    \
        return;                                                                \
    }                                                                          \
    THROW_ERROR()

#define NAMED_METHOD_VOID()          \
    CHECK_ARGUMENT()                 \
    if (hr.ok()) {                   \
        return v8::Intercepted::kNo; \
    }                                \
    NAMED_THROW_ERROR()

#define CONSTRUCT_RETURN()                                                                                \
    CHECK_ARGUMENT()                                                                                      \
    if (hr.ok()) {                                                                                        \
        args.GetReturnValue().Set(V8_RETURN(vr->wrap(Isolate::current(args.GetIsolate()), args.This()))); \
        return;                                                                                           \
    }                                                                                                     \
    THROW_ERROR()

#define PROPERTY_VAL(t)                                 \
    t v0;                                               \
    hr = GetArgumentValue(isolate, value, v0, bStrict); \
    if (hr.failed())                                         \
        break;

#define ARG(t, n)                                           \
    t v##n;                                                 \
    hr = GetArgumentValue(isolate, args[n], v##n, bStrict); \
    if (hr.failed())                                             \
        break;

#define STRICT_ARG(t, n)                                 \
    t v##n;                                              \
    hr = GetArgumentValue(isolate, args[n], v##n, true); \
    if (hr.failed())                                          \
        break;

#define OPT_ARG(t, n, d)                                                \
    t v##n = (d);                                                       \
    if ((n) < argc && !(args[n]->IsUndefined() || args[n]->IsNull())) { \
        hr = GetArgumentValue(isolate, args[n], v##n, bStrict);         \
        if (hr.failed())                                                     \
            break;                                                      \
    }

#define ARG_LIST(n) OptArgs v##n(args, n, argc1);

#define DECLARE_CLASSINFO(c)                                   \
public:                                                        \
    static ClassInfo& class_info();                            \
    virtual ClassInfo& Classinfo()                             \
    {                                                          \
        return class_info();                                   \
    }                                                          \
    static c* getInstance(object_base* o)                      \
    {                                                          \
        if (!o || !c::class_info().isInstance(o->Classinfo())) \
            return NULL;                                       \
        return (c*)o;                                          \
    }                                                          \
    static c* getInstance(v8::Local<v8::Value> o)              \
    {                                                          \
        return getInstance((object_base*)unwrap(o));           \
    }

#define DECLARE_CLASS(c)                  \
public:                                   \
    c()                                   \
    {                                     \
        if (m_in_trace)                   \
            c::class_info().RefClass();   \
    }                                     \
    virtual ~c()                          \
    {                                     \
        if (m_in_trace)                   \
            c::class_info().UnrefClass(); \
    }                                     \
    DECLARE_CLASSINFO(c)

#define DECLARE_MODULE(name)                      \
    class RootModule_##name : public RootModule { \
    public:                                       \
        virtual ClassInfo& class_info()           \
        {                                         \
            return name##_base::class_info();     \
        }                                         \
    };                                            \
    static RootModule_##name s_RootModule_##name; \
    RootModule* Module_##name = &s_RootModule_##name;

#define DECLARE_MODULE_WITH_CONSTRUCTOR_EX(mname, module) \
    class RootModule_##mname : public RootModule {        \
    public:                                               \
        RootModule_##mname();                             \
                                                          \
    public:                                               \
        virtual ClassInfo& class_info()                   \
        {                                                 \
            return module##_base::class_info();           \
        }                                                 \
        virtual const char* name()                        \
        {                                                 \
            return #mname;                                \
        }                                                 \
    };                                                    \
    static RootModule_##mname s_RootModule_##mname;       \
    RootModule* Module_##mname = &s_RootModule_##mname;

#define DECLARE_MODULE_EX(mname, module)            \
    class RootModule_##mname : public RootModule {  \
    public:                                         \
        virtual ClassInfo& class_info()             \
        {                                           \
            return module##_base::class_info();     \
        }                                           \
        virtual const char* name()                  \
        {                                           \
            return #mname;                          \
        }                                           \
    };                                              \
    static RootModule_##mname s_RootModule_##mname; \
    RootModule* Module_##mname = &s_RootModule_##mname;

#define IMPORT_MODULE(name)           \
    extern RootModule* Module_##name; \
    Module_##name->install();

#define EVENT_SUPPORT()                                                                       \
public:                                                                                       \
    virtual result_t on(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,                       \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::on(ev, func, retVal);                                             \
    }                                                                                         \
    virtual result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)             \
    {                                                                                         \
        return object_base::on(map, retVal);                                                  \
    }                                                                                         \
    virtual result_t prependListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,          \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::prependListener(ev, func, retVal);                                \
    }                                                                                         \
    virtual result_t prependListener(v8::Local<v8::Object> map,                               \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::prependListener(map, retVal);                                     \
    }                                                                                         \
    virtual result_t addListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,              \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::on(ev, func, retVal);                                             \
    }                                                                                         \
    virtual result_t addListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)    \
    {                                                                                         \
        return object_base::on(map, retVal);                                                  \
    }                                                                                         \
    virtual result_t addEventListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,         \
        v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)                         \
    {                                                                                         \
        return object_base::addEventListener(ev, func, options, retVal);                      \
    }                                                                                         \
    virtual result_t prependOnceListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,      \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::prependOnceListener(ev, func, retVal);                            \
    }                                                                                         \
    virtual result_t prependOnceListener(v8::Local<v8::Object> map,                           \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::prependOnceListener(map, retVal);                                 \
    }                                                                                         \
    virtual result_t once(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,                     \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::once(ev, func, retVal);                                           \
    }                                                                                         \
    virtual result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)           \
    {                                                                                         \
        return object_base::once(map, retVal);                                                \
    }                                                                                         \
    virtual result_t off(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,                      \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::off(ev, func, retVal);                                            \
    }                                                                                         \
    virtual result_t off(v8::Local<v8::Value> ev, v8::Local<v8::Object>& retVal)                     \
    {                                                                                         \
        return object_base::off(ev, retVal);                                                  \
    }                                                                                         \
    virtual result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)            \
    {                                                                                         \
        return object_base::off(map, retVal);                                                 \
    }                                                                                         \
    virtual result_t removeListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,           \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::off(ev, func, retVal);                                            \
    }                                                                                         \
    virtual result_t removeListener(v8::Local<v8::Value> ev, v8::Local<v8::Object>& retVal)          \
    {                                                                                         \
        return object_base::off(ev, retVal);                                                  \
    }                                                                                         \
    virtual result_t removeListener(v8::Local<v8::Object> map,                                \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::off(map, retVal);                                                 \
    }                                                                                         \
    virtual result_t removeEventListener(v8::Local<v8::Value> ev, v8::Local<v8::Function> func,      \
        v8::Local<v8::Object> options, v8::Local<v8::Object>& retVal)                         \
    {                                                                                         \
        return object_base::removeEventListener(ev, func, options, retVal);                   \
    }                                                                                         \
    virtual result_t removeAllListeners(v8::Local<v8::Value> ev,                                     \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::removeAllListeners(ev, retVal);                                   \
    }                                                                                         \
    virtual result_t removeAllListeners(v8::Local<v8::Array> evs,                             \
        v8::Local<v8::Object>& retVal)                                                        \
    {                                                                                         \
        return object_base::removeAllListeners(evs, retVal);                                  \
    }                                                                                         \
    virtual result_t setMaxListeners(int32_t n)                                               \
    {                                                                                         \
        return object_base::setMaxListeners(n);                                               \
    }                                                                                         \
    virtual result_t getMaxListeners(int32_t& retVal)                                         \
    {                                                                                         \
        return object_base::getMaxListeners(retVal);                                          \
    }                                                                                         \
    virtual result_t listeners(v8::Local<v8::Value> ev, v8::Local<v8::Array>& retVal)                \
    {                                                                                         \
        return object_base::listeners(ev, retVal);                                            \
    }                                                                                         \
    virtual result_t rawListeners(v8::Local<v8::Value> ev, v8::Local<v8::Array>& retVal)             \
    {                                                                                         \
        return object_base::rawListeners(ev, retVal);                                         \
    }                                                                                         \
    virtual result_t listenerCount(v8::Local<v8::Value> ev, int32_t& retVal)                         \
    {                                                                                         \
        return object_base::listenerCount(ev, retVal);                                        \
    }                                                                                         \
    virtual result_t listenerCount(v8::Local<v8::Value> o, v8::Local<v8::Value> ev, int32_t& retVal) \
    {                                                                                         \
        return object_base::listenerCount(o, ev, retVal);                                     \
    }                                                                                         \
    virtual result_t emit(v8::Local<v8::Value> ev, OptArgs args,                                     \
        bool& retVal)                                                                         \
    {                                                                                         \
        return object_base::emit(ev, args, retVal);                                           \
    }                                                                                         \
    virtual result_t eventNames(v8::Local<v8::Array>& retVal)                                 \
    {                                                                                         \
        return object_base::eventNames(retVal);                                               \
    }

#define FIBER_FREE()                                     \
public:                                                  \
    virtual bool enterTask(exlib::Task_base* current)    \
    {                                                    \
        return true;                                     \
    }                                                    \
    virtual void enter()                                 \
    {                                                    \
    }                                                    \
    virtual void leave(exlib::Task_base* current = NULL) \
    {                                                    \
    }

#define LOAD_OPTION_MEMBER(r, data, elem)                                 \
    hr = GetConfigValue(opt, BOOST_PP_STRINGIZE(elem), data->elem, true); \
    if (hr.failed())                                                           \
        return hr;

#define LOAD_OPTIONS(Class, Members)                                      \
    static Class* getInstance(v8::Local<v8::Value> v) { return nullptr; } \
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Class>& retVal)  \
    {                                                                     \
        if (!IsJSObject(v))                                               \
            return result_t::type_mismatch();                             \
        v8::Local<v8::Object> opt = v.As<v8::Object>();                   \
        obj_ptr<Class> o = new Class();                                   \
        result_t hr;                                                      \
        BOOST_PP_SEQ_FOR_EACH(LOAD_OPTION_MEMBER, o, Members)             \
        retVal = o;                                                       \
        return result_t();                                                \
    }

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

#ifndef _offsetof
#define _offsetof(TYPE, MEMBER) ((size_t)&((TYPE*)0)->MEMBER)
#endif

#ifndef container_of
#define container_of(ptr, TYPE, MEMBER) \
    ((TYPE*)((char*)(ptr) - _offsetof(TYPE, MEMBER)))
#endif

result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, exlib::string& n, bool bStrict = false);

// the DOMString conversion of the DOM, the String(v) of the web platform: a
// number is "1", null is "null", an object goes through its own toString()
// and a native object without one renders its class tag ("[object File]",
// what Object.prototype.toString gives). It serves the APIs whose parameter is
// a DOMString -- btoa, WebSocket.send, the value of a collection -- while the
// argument conversion above serves the String parameters and takes real
// strings on purpose
result_t GetDOMStringValue(v8::Local<v8::Value> v, exlib::string& retVal);

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, double& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    v8::Local<v8::Context> context = isolate->context();

    if (v->IsNumber() || v->IsNumberObject()) {
        n = v->NumberValue(context).FromMaybe(NAN);
        return std::isnan(n) ? result_t::type_mismatch() : result_t();
    }

    if (bStrict)
        return result_t::type_mismatch();

    v = v->ToNumber(context).FromMaybe(v8::Local<v8::Number>());
    if (v.IsEmpty())
        return result_t(CALL_E_JAVASCRIPT);

    n = v->NumberValue(context).FromMaybe(NAN);
    return std::isnan(n) ? result_t::type_mismatch() : result_t();
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, int64_t& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    v8::Local<v8::BigInt> mv;
    v8::Local<v8::Context> context = isolate->context();

    if (!v->IsBigInt() && !v->IsBigIntObject()) {
        if (!v->IsNumber() && !v->IsNumberObject()) {
            if (bStrict)
                return result_t::type_mismatch();

            {
                TryCatch try_catch;
                mv = v->ToBigInt(context).FromMaybe(v8::Local<v8::BigInt>());
            }
            if (mv.IsEmpty()) {
                v = v->ToNumber(context).FromMaybe(v8::Local<v8::Number>());
                if (v.IsEmpty())
                    return result_t(CALL_E_JAVASCRIPT);
            }
        }
    } else {
        mv = v->ToBigInt(context).FromMaybe(v8::Local<v8::BigInt>());
        if (mv.IsEmpty())
            return result_t(CALL_E_JAVASCRIPT);
    }

    if (!mv.IsEmpty()) {
        bool less;

        n = mv->Int64Value(&less);
        return less ? result_t() : result_t::out_of_range();
    } else {
        double num;

        num = v->NumberValue(context).FromMaybe(NAN);
        if (std::isnan(num))
            return result_t::type_mismatch();

        if (num < -9007199254740992ll || num > 9007199254740992ll)
            return result_t::out_of_range();

        n = (int64_t)num;
        return result_t();
    }
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, int32_t& n, bool bStrict = false)
{
    int64_t num;

    result_t hr = GetArgumentValue(isolate, v, num, bStrict);
    if (hr.failed())
        return hr;

    if (num < -2147483648ll || num > 2147483647ll)
        return result_t::out_of_range();

    n = (int32_t)num;

    return result_t();
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, bool& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    if (bStrict && !v->IsBoolean() && !v->IsBooleanObject())
        return result_t::type_mismatch();

    n = isolate->toBoolean(v);
    return result_t();
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, date_t& d, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    if (v->IsDate()) {
        d = v;
    } else {
        if (bStrict)
            return result_t::type_mismatch();

        double n;
        result_t hr = GetArgumentValue(isolate, v, n);
        if (hr.failed())
            return hr;

        d = n;
    }

    return result_t();
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, Variant& d, bool bStrict = false)
{
    d = v;
    return result_t();
}

class OptArgs {
public:
    OptArgs(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t base, int32_t argc)
        : m_args(&args)
        , m_base(base)
        , m_argc(argc)
    {
        if (m_base > m_argc)
            m_base = m_argc;
    }

    OptArgs(std::vector<v8::Local<v8::Value>>& argv)
        : m_v(argv.data())
        , m_base(0)
        , m_argc((int32_t)argv.size())
    {
    }

    OptArgs(v8::Local<v8::Value>& v)
        : m_v(&v)
        , m_base(0)
        , m_argc(1)
    {
    }

    OptArgs(const OptArgs& a)
        : m_args(a.m_args)
        , m_v(a.m_v)
        , m_base(a.m_base)
        , m_argc(a.m_argc)
    {
    }

    OptArgs()
    {
    }

    int32_t Length() const
    {
        return m_argc - m_base;
    }

    v8::Local<v8::Value> operator[](int32_t i) const
    {
        if (m_v)
            return m_v[i];

        return (*m_args)[i + m_base];
    }

    v8::Local<v8::Object> This() const
    {
        return v8::Local<v8::Object>();
    }

    void GetData(std::vector<v8::Local<v8::Value>>& datas)
    {
        if (m_v) {
            datas.resize(m_argc);
            for (int32_t i = 0; i < m_argc; i++)
                datas[i] = m_v[i];
            return;
        }

        datas.resize(m_argc - m_base);
        for (int32_t i = 0; i < m_argc - m_base; i++)
            datas[i] = (*m_args)[m_base + i];
    }

private:
    const v8::FunctionCallbackInfo<v8::Value>* m_args = nullptr;
    v8::Local<v8::Value>* m_v = nullptr;
    int32_t m_base = 0;
    int32_t m_argc = 0;
};

template <class T>
result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<T>& vr, bool bStrict = false)
{
    vr = T::getInstance(v);
    if (vr)
        return result_t();
    if (bStrict)
        return result_t::type_mismatch();

    return T::load(v, vr);
}

class Buffer_base;
result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Buffer_base>& vr, bool bStrict = false);
result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<object_base>& vr, bool bStrict = false);

// a native object is one that wraps an object_base: the class identity is
// asked for through getInstance, never guessed from the internal field layout
bool IsNativeObject(v8::Local<v8::Value> v);

inline bool IsJSObject(v8::Local<v8::Value> v)
{
    if (!v->IsObject() || v->IsProxy() || v->IsArrayBuffer() || v->IsArrayBufferView()
        || v->IsTypedArray() || v->IsStringObject() || v->IsNumberObject()
        || v->IsBooleanObject() || v->IsSymbolObject())
        return false;

    if (IsNativeObject(v))
        return false;

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
    v8::Local<v8::Value> proto = o->GetPrototype();

    // accept Object.create(null) — null-prototype plain objects
    if (proto->IsNull())
        return true;

    // NOTE: must use the value's own creation context. A sandbox/vm value keeps
    // its own Object.prototype in embedder slot kObjectPrototype, so comparing
    // against the current isolate's context yields a false negative.
    v8::Local<v8::Context> _context;
    if (!o->GetCreationContext().ToLocal(&_context))
        return false;

    JSValue expected = _context->GetEmbedderData(kObjectPrototype);
    if (!expected->Equals(_context, proto).FromMaybe(false))
        return false;

    return true;
}

inline bool IsJSBuffer(v8::Local<v8::Value> v, bool strict = true)
{
    if (!strict && (v->IsArrayBuffer() || v->IsArrayBufferView() || v->IsTypedArray()))
        return true;

    if (!v->IsUint8Array())
        return false;

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);

    // NOTE: must use the value's own creation context (see IsJSObject).
    v8::Local<v8::Context> _context;
    if (!o->GetCreationContext().ToLocal(&_context))
        return false;

    JSValue proto = _context->GetEmbedderData(kBufferPrototype);
    if (!proto->Equals(_context, o->GetPrototype()).FromMaybe(false))
        return false;

    return true;
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, v8::Local<v8::Object>& vr, bool bStrict = false)
{
    if (v.IsEmpty() || !IsJSObject(v))
        return result_t::type_mismatch();

    vr = v8::Local<v8::Object>::Cast(v);
    return result_t();
}

#define GET_JSVALUE(type)                                                                                                     \
    inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, v8::Local<v8::type>& vr, bool bStrict = false) \
    {                                                                                                                         \
        if (v.IsEmpty())                                                                                                      \
            return result_t::type_mismatch();                                                                                       \
        if (!v->Is##type())                                                                                                   \
            return result_t::type_mismatch();                                                                                       \
        vr = v8::Local<v8::type>::Cast(v);                                                                                    \
        return result_t();                                                                                                             \
    }

GET_JSVALUE(Array);
GET_JSVALUE(Uint8Array);
GET_JSVALUE(TypedArray);
GET_JSVALUE(ArrayBuffer);
GET_JSVALUE(ArrayBufferView);
GET_JSVALUE(Function);
GET_JSVALUE(Promise);
GET_JSVALUE(RegExp);

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, v8::Local<v8::Value>& vr, bool bStrict = false)
{
    vr = v;
    return result_t();
}

inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, std::shared_ptr<v8::BackingStore>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    if (!v->IsArrayBuffer() && !v->IsArrayBufferView() && !v->IsTypedArray())
        return result_t::type_mismatch();

    v8::Local<v8::ArrayBuffer> ab = v8::Local<v8::ArrayBuffer>::Cast(v);
    vr = ab->GetBackingStore();
    if (vr == nullptr)
        return result_t::type_mismatch();

    return result_t();
}

template <class T>
inline result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, std::vector<T>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    if (!v->IsArray())
        return result_t::type_mismatch();

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(v);
    v8::Local<v8::Context> context = isolate->context();

    std::vector<T> r = std::vector<T>();

    for (uint32_t i = 0; i < arr->Length(); i++) {
        v8::Local<v8::Value> v1;
        if (!arr->Get(context, i).ToLocal(&v1))
            return result_t(CALL_E_JAVASCRIPT);
        T n;
        result_t hr = GetArgumentValue(isolate, v1, n, false);
        if (hr.failed())
            return hr;

        r.push_back(n);
    }

    vr = r;

    return result_t();
}

// A parameter-position union (`Buffer|String`, an IDL `A|B`) converts to one
// std::variant: the alternatives are tried in declaration order within the
// same pass (the whole variant chain runs the strict pass first, then the
// lenient one), and the first conversion that succeeds wins — the order is
// the IDL's preference order (plans/idl-union-types-2026-10-02.md).
//
// The helpers live above the overload so two-phase lookup finds the per-type
// converters declared earlier in this header.
template <std::size_t I, typename... Ts>
bool tryVariantAlternative(Isolate* isolate, v8::Local<v8::Value> v,
    std::variant<Ts...>& n, bool bStrict, result_t& hr)
{
    std::variant_alternative_t<I, std::variant<Ts...>> value;
    hr = GetArgumentValue(isolate, v, value, bStrict);
    if (hr.failed())
        return false;

    n = std::move(value);
    return true;
}

template <typename... Ts, std::size_t... Is>
result_t getVariantValue(Isolate* isolate, v8::Local<v8::Value> v,
    std::variant<Ts...>& n, bool bStrict, std::index_sequence<Is...>)
{
    result_t hr(CALL_E_TYPEMISMATCH);
    bool done = false;

    (void)std::initializer_list<bool>{
        (done ? false
              : (done = tryVariantAlternative<Is>(isolate, v, n, bStrict, hr)))...
    };

    return done ? result_t() : hr;
}

template <typename... Ts>
result_t GetArgumentValue(Isolate* isolate, v8::Local<v8::Value> v, std::variant<Ts...>& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return result_t::type_mismatch();

    return getVariantValue(isolate, v, n, bStrict, std::index_sequence_for<Ts...>());
}

result_t setRuntimeError(int32_t code, const char* err = nullptr);

template <typename T>
result_t GetConfigValue(v8::Local<v8::Object> o, const char* key, T& n, bool bStrict = false)
{
    if (o.IsEmpty())
        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, key);

    Isolate* isolate = Isolate::current(o);
    JSValue v = o->Get(isolate->context(), isolate->NewString(key));
    if (v->IsUndefined() || v->IsNull())
        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, key);

    return GetArgumentValue(isolate, v, n, bStrict);
}

// a binary option (key material, passphrase, header, ...) accepts a string as
// its utf8 bytes: the string form is the nominal way to write it, while the
// lenient Buffer argument conversion no longer takes strings (the definition
// lives in src/global/Buffer.cpp, where Buffer_base is complete)
result_t GetConfigValue(v8::Local<v8::Object> o, const char* key, obj_ptr<Buffer_base>& n, bool bStrict = false);

template <typename T>
result_t GetConfigValue(v8::Local<v8::Object> o, const char* key, std::optional<T>& n, bool bStrict = false)
{
    if (o.IsEmpty())
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    T n1;
    Isolate* isolate = Isolate::current(o);
    result_t hr = GetConfigValue(o, key, n1, bStrict);
    if (hr.ok())
        n = n1;
    else if (!hr.is_param_missing())
        return hr;

    return result_t();
}

template <typename T>
result_t GetConfigValue(v8::Local<v8::Array> o, int32_t i, T& n, bool bStrict = false)
{
    JSValue v;
    Isolate* isolate = Isolate::current(o);
    if (!o.IsEmpty())
        v = o->Get(isolate->context(), i);
    if (v.IsEmpty() || v->IsUndefined()) {
        char key[32];
        snprintf(key, sizeof(key), "%d", i + 1);
        return setRuntimeError(CALL_E_PARAMNOTOPTIONAL, key);
    }

    return GetArgumentValue(isolate, v, n, bStrict);
}

result_t CheckConfig(v8::Local<v8::Object> opts, const char** keys);

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, int32_t v)
{
    return v8::Int32::New(isolate->m_isolate, v);
}

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, bool v)
{
    return v ? v8::True(isolate->m_isolate) : v8::False(isolate->m_isolate);
}

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, double v)
{
    return v8::Number::New(isolate->m_isolate, v);
}

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, int64_t v)
{
    return v8::Number::New(isolate->m_isolate, (double)v);
}

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, exlib::string& str)
{
    return isolate->NewString(str);
}

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, std::shared_ptr<v8::BackingStore>& store)
{
    return v8::ArrayBuffer::New(isolate->m_isolate, std::move(store));
}

// inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, std::string& str)
// {
//     return isolate->NewString(str.c_str(), str.length());
// }

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, date_t& v)
{
    return v.value(isolate->m_isolate);
}

inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, Variant& v)
{
    return v;
}

template <typename T>
inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, v8::Local<T>& v)
{
    return v;
}

template <class T>
inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, obj_ptr<T>& obj)
{
    v8::Local<v8::Value> v;
    obj->holder(isolate);
    obj->valueOf(v);
    return v;
}

template <typename T>
inline v8::Local<v8::Value> GetReturnValue(Isolate* isolate, std::vector<T>& vec)
{
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Array> arr = v8::Array::New(isolate->m_isolate, vec.size());

    for (int i = 0; i < vec.size(); i++)
        arr->Set(context, i, GetReturnValue(isolate, vec[i]));

    return arr;
}

// Render a bounded, secret-free summary of a value for error diagnostics.
// Both helpers never touch JS handles: they only read plain C++ state.
exlib::string summarize_for_error(const Variant& value);
exlib::string summarize_for_error(object_base* obj);

// Parameter summary entry carried by ErrorPayload. Values are pre-rendered
// summaries instead of raw objects: a payload travels across fibers/threads
// and must never keep JS handles nor unbounded data alive.
// ErrorArg / ErrorPayload live in result.h (below result_t: the payload rides
// with the value, and the header ordering makes that possible).

v8::Local<v8::Value> FillError(result_t hr);v8::Local<v8::Value> FillError(result_t hr, exlib::string msg);
v8::Local<v8::Value> FillError(result_t hr, v8::Local<v8::StackTrace> stack);

// Build a DOMException in the current sandbox; falls back to a plain Error
// carrying the same `name` when the constructor is unavailable.
v8::Local<v8::Value> MakeDOMException(Isolate* isolate, const char* name, exlib::string msg);

inline v8::Local<v8::Value> ThrowError(v8::Local<v8::Value> exception)
{
    Isolate* isolate = Isolate::current();
    return isolate->m_isolate->ThrowException(exception);
}

inline v8::Local<v8::Value> ThrowError(result_t hr, exlib::string msg)
{
    return ThrowError(FillError(hr, msg));
}

inline v8::Local<v8::Value> ThrowError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    return ThrowError(v8::Exception::Error(isolate->NewString(msg)));
}

inline v8::Local<v8::Value> ThrowError(exlib::string msg)
{
    return ThrowError(msg.c_str());
}

inline v8::Local<v8::Value> ThrowTypeError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    return ThrowError(v8::Exception::TypeError(isolate->NewString(msg)));
}

inline v8::Local<v8::Value> ThrowTypeError(exlib::string msg)
{
    return ThrowTypeError(msg.c_str());
}

inline v8::Local<v8::Value> ThrowRangeError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    return ThrowError(v8::Exception::RangeError(isolate->NewString(msg)));
}

inline v8::Local<v8::Value> ThrowRangeError(exlib::string msg)
{
    return ThrowRangeError(msg.c_str());
}

inline v8::Local<v8::Value> ThrowReferenceError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    return ThrowError(v8::Exception::ReferenceError(isolate->NewString(msg)));
}

inline v8::Local<v8::Value> ThrowReferenceError(exlib::string msg)
{
    return ThrowReferenceError(msg.c_str());
}

inline v8::Local<v8::Value> ThrowSyntaxError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    return ThrowError(v8::Exception::SyntaxError(isolate->NewString(msg)));
}

inline v8::Local<v8::Value> ThrowSyntaxError(exlib::string msg)
{
    return ThrowSyntaxError(msg.c_str());
}

inline v8::Local<v8::Value> ThrowURIError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    auto _context = isolate->context();
    auto glob = _context->Global();
    auto URIError = (JSValue(glob->Get(_context, isolate->NewString("URIError")))).As<v8::Object>();

    v8::Local<v8::Value> args[] = { isolate->NewString(msg) };
    v8::Local<v8::Value> error = URIError->CallAsConstructor(_context, 1, args).FromMaybe(v8::Local<v8::Value>());
    return ThrowError(error);
}

inline v8::Local<v8::Value> ThrowURIError(exlib::string msg)
{
    return ThrowURIError(msg.c_str());
}

inline v8::Local<v8::Value> ThrowEvalError(const char* msg)
{
    Isolate* isolate = Isolate::current();
    auto _context = isolate->context();
    auto glob = _context->Global();
    auto EvalError = (JSValue(glob->Get(_context, isolate->NewString("EvalError")))).As<v8::Object>();

    v8::Local<v8::Value> args[] = { isolate->NewString(msg) };
    v8::Local<v8::Value> error = EvalError->CallAsConstructor(_context, 1, args).FromMaybe(v8::Local<v8::Value>());
    return ThrowError(error);
}

inline v8::Local<v8::Value> ThrowEvalError(exlib::string msg)
{
    return ThrowEvalError(msg.c_str());
}

// The raw errno-domain code, not yet a value: the code stays int32_t until the
// enrichment overload below constructs the result_t (single-shot creation, at
// the failing layer -- upper layers forward, they do not rebuild).
inline int32_t LastError()
{
#ifdef _WIN32
    return -(int32_t)GetLastError();
#else
    return -errno;
#endif
}

// Node.js compatible system-error context (errno + syscall + path/hostname).
// A non-system error carries no such context and passes through untouched: the
// fields only describe errno-domain failures, so there is nothing to attach.
inline result_t LastError(const char* syscall, const exlib::string& path = exlib::string(),
    const exlib::string& hostname = exlib::string())
{
    int32_t err = LastError();

    if (err >= 0 || err <= CALL_E_MAX)
        return result_t(err);

    return result_t::from_system(err)
        .with_syscall(syscall)
        .with_path(path)
        .with_hostname(hostname);
}

// Error of a failed read(2). POSIX reports EBADF when the descriptor was opened
// write-only, Windows fails the same read with ERROR_ACCESS_DENIED, so it is
// translated here the way libuv does in its fs__read.
inline int32_t ReadError()
{
#ifdef _WIN32
    DWORD err = GetLastError();

    if (err == ERROR_ACCESS_DENIED)
        return UV_EBADF;

    return -(int32_t)err;
#else
    return -errno;
#endif
}

inline result_t ReadError(const char* syscall, const exlib::string& path = exlib::string(),
    const exlib::string& hostname = exlib::string())
{
    int32_t err = ReadError();

    if (err >= 0 || err <= CALL_E_MAX)
        return result_t(err);

    return result_t::from_system(err)
        .with_syscall(syscall)
        .with_path(path)
        .with_hostname(hostname);
}

inline int32_t SocketError()
{
#ifdef _WIN32
    return -WSAGetLastError();
#else
    return -errno;
#endif
}

inline result_t SocketError(const char* syscall, const exlib::string& path = exlib::string(),
    const exlib::string& hostname = exlib::string())
{
    int32_t err = SocketError();

    if (err >= 0 || err <= CALL_E_MAX)
        return result_t(err);

    return result_t::from_system(err)
        .with_syscall(syscall)
        .with_path(path)
        .with_hostname(hostname);
}

exlib::string getResultMessage(result_t hr);
v8::Local<v8::Value> ThrowResult(result_t hr);
exlib::string ReportException(TryCatch& try_catch, result_t hr, bool repl);
exlib::string GetException(v8::Local<v8::Value> err, bool repl, bool trace);
exlib::string GetException(TryCatch& try_catch, result_t hr, bool repl, bool trace);
result_t throwSyntaxError(TryCatch& try_catch);
const char* signo_string(int signo);

#ifdef _WIN32

#define PATH_SLASH '\\'
#define PATH_SLASH_STR "\\"
#define PATH_DELIMITER ';'

inline bool isPathSlash(char ch)
{
    return ch == '/' || ch == '\\';
}

#else

#define PATH_SLASH '/'
#define PATH_SLASH_STR "/"
#define PATH_DELIMITER ':'

inline bool isPathSlash(char ch)
{
    return ch == '/';
}

#endif

#define PATH_SLASH_WIN32 '\\'
#define PATH_SLASH_WIN32_STR "\\"
#define PATH_DELIMITER_WIN32 ';'
#define PATH_SLASH_POSIX '/'
#define PATH_SLASH_POSIX_STR "/"
#define PATH_DELIMITER_POSIX ':'

inline bool isWin32PathSlash(char ch)
{
    return ch == '/' || ch == '\\';
}

/* some char ascii map constants :start */
/* A */
#define CHAR_UPPERCASE_A 65
/* a */
#define CHAR_LOWERCASE_A 97
/* Z */
#define CHAR_UPPERCASE_Z 90
/* z */
#define CHAR_LOWERCASE_Z 122
/* / */
#define CHAR_FORWARD_SLASH 47
/* \ */
#define CHAR_BACKWARD_SLASH 92
/* . */
#define CHAR_DOT 46
/* : */
#define CHAR_COLON 58

inline bool isWindowsDeviceRoot(char ch)
{
    return (ch >= CHAR_UPPERCASE_A && ch <= CHAR_UPPERCASE_Z) || (ch >= CHAR_LOWERCASE_A && ch <= CHAR_LOWERCASE_Z);
}

inline bool isPathSeparator(char ch)
{
    return (ch == CHAR_FORWARD_SLASH) || (ch == CHAR_BACKWARD_SLASH);
}

inline bool isPosixPathSlash(char ch)
{
    return ch == '/';
}

#define URL_SLASH '/'

inline bool isUrlSlash(char ch)
{
    return ch == '/';
}

void outLog(int32_t priority, exlib::string msg);
void errorLog(exlib::string msg);

extern bool g_no_deprecation;
#define DEPRECATED_SOON(name)                                         \
    {                                                                 \
        static bool once = false;                                     \
        if (!g_no_deprecation && !once) {                             \
            once = true;                                              \
            exlib::string str(name);                                  \
            str.append(" is deprecated and will soon be removed.\n"); \
            errorLog(str + traceInfo(isolate->m_isolate, 16));        \
        }                                                             \
    }

inline exlib::string niceSize(int64_t sz)
{
    char buf[64];
    double num = (double)sz;
    double test = num >= 0 ? num : -num;
    int32_t cnt;

    if (test < 1024)
        cnt = snprintf(buf, sizeof(buf), "%d bytes", (int32_t)sz);
    else if (test < 1024 * 1024)
        cnt = snprintf(buf, sizeof(buf), "%.1f KB", num / 1024);
    else if (test < 1024 * 1024 * 1024)
        cnt = snprintf(buf, sizeof(buf), "%.1f MB", num / (1024 * 1024));
    else if (test < 1024ll * 1024 * 1024 * 1024)
        cnt = snprintf(buf, sizeof(buf), "%.1f GB", num / (1024 * 1024 * 1024));
    else if (test < 1024ll * 1024 * 1024 * 1024 * 1024)
        cnt = snprintf(buf, sizeof(buf), "%.1f TB", num / (1024ll * 1024 * 1024 * 1024));
    else if (test < 1024ll * 1024 * 1024 * 1024 * 1024 * 1024)
        cnt = snprintf(buf, sizeof(buf), "%.1f PB", num / (1024ll * 1024 * 1024 * 1024 * 1024));
    else
        cnt = snprintf(buf, sizeof(buf), "%.1f EB", num / (1024ll * 1024 * 1024 * 1024 * 1024 * 1024));

    return exlib::string(buf, cnt);
}

void flushLog();

// Converts a V8 value to a C string.
const auto ToCString = [](const v8::String::Utf8Value& value) {
    return *value ? *value : "<string conversion failed>";
};

inline exlib::string clean_string(const char* s, size_t len)
{
    exlib::string s1(s, len);
    char* c_buf = s1.data();

    for (int32_t i = 0; i < len; i++)
        if ((c_buf[i] < 32 && c_buf[i] != 0xd && c_buf[i] != 0xa) || c_buf[i] > 127)
            c_buf[i] = '.';

    return s1;
}

inline exlib::string clean_string(exlib::string s)
{
    return clean_string(s.c_str(), s.length());
}

class save_method_name {
public:
    save_method_name(const char* name);
    ~save_method_name();

private:
    JSFiber* m_fb;
    const char* m_name;
};

inline bool is_big_endian()
{
    union {
        uint32_t i;
        char c[4];
    } bint = { 0x01020304 };

    return bint.c[0] == 1;
}

// Returns true if the named property exists on obj and is not undefined/null.
// Prefer this over Has() so that {key: undefined} is treated as absent.
inline bool js_obj_has_value(v8::Local<v8::Object> obj, v8::Local<v8::Context> context,
    const char* name)
{
    v8::Local<v8::Value> val;
    Isolate* isolate = Isolate::current(context);
    return obj->Get(context, isolate->NewString(name)).ToLocal(&val)
        && !val->IsUndefined() && !val->IsNull();
}
}
