/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _fj_utils_H_
#define _fj_utils_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <exlib/include/osconfig.h>
#include <errno.h>

#ifdef _WIN32

#include <ws2tcpip.h>
#include <winsock2.h>
#include <mstcpip.h>

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif // IPV6_V6ONLY

#include <windows.h>

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

#include <v8/include/v8.h>
#include "obj_ptr.h"
#include <Isolate.h>

#include <math.h>
#include <vector>

#ifdef _WIN32

#ifndef INFINITY
#define INFINITY (DBL_MAX + DBL_MAX)
#endif // INFINITY

#ifndef NAN
#define NAN (INFINITY - INFINITY)
#endif // NAN

#ifndef isnan
#define isnan !!_isnan
#endif // isnan

#endif // _WIN32

#include "date.h"
#include "Variant.h"

#include <exlib/include/service.h>

#ifdef _DEBUG
#undef _DEBUG
#endif // _DEBUG

namespace fibjs {

typedef int32_t result_t;

#define CALL_RETURN_NULL 100000

#define CALL_E_MAX -100000
// Invalid number of parameters.
#define CALL_E_BADPARAMCOUNT (CALL_E_MAX - 1)
// Parameter not optional.
#define CALL_E_PARAMNOTOPTIONAL (CALL_E_MAX - 2)
// The input parameter is not a valid type.
#define CALL_E_BADVARTYPE (CALL_E_MAX - 3)
// Invalid argument.
#define CALL_E_INVALIDARG (CALL_E_MAX - 4)
// The argument could not be coerced to the specified type.
#define CALL_E_TYPEMISMATCH (CALL_E_MAX - 5)
// Value is out of range.
#define CALL_E_OUTRANGE (CALL_E_MAX - 6)

#define CALL_E_MIN_ARG (CALL_E_OUTRANGE - 1)

// Constructor cannot be called as a function.
#define CALL_E_CONSTRUCTOR (CALL_E_MAX - 7)
// Object is not an instance of declaring class.
#define CALL_E_NOTINSTANCE (CALL_E_MAX - 8)
// Invalid procedure call.
#define CALL_E_INVALID_CALL (CALL_E_MAX - 9)
//  Re-entrant calls are not allowed.
#define CALL_E_REENTRANT (CALL_E_MAX - 10)
// Invalid input data
#define CALL_E_INVALID_DATA (CALL_E_MAX - 11)
// Index was out of range.
#define CALL_E_BADINDEX (CALL_E_MAX - 12)
// Memory overflow error.
#define CALL_E_OVERFLOW (CALL_E_MAX - 13)
// Collection is empty.
#define CALL_E_EMPTY (CALL_E_MAX - 14)
// Operation now in progress.
#define CALL_E_PENDDING (CALL_E_MAX - 15)
// Operation not support asynchronous call.
#define CALL_E_NOASYNC (CALL_E_MAX - 16)
// Operation not support synchronous call.
#define CALL_E_NOSYNC (CALL_E_MAX - 17)
// Operation is long synchronous call.
#define CALL_E_LONGSYNC (CALL_E_MAX - 18)
// Operation is GUI call.
#define CALL_E_GUICALL (CALL_E_MAX - 19)
// Internal error.
#define CALL_E_INTERNAL (CALL_E_MAX - 20)
// Invalid return type.
#define CALL_E_RETURN_TYPE (CALL_E_MAX - 21)
// Exception occurred.
#define CALL_E_EXCEPTION (CALL_E_MAX - 22)
// Javascript error.
#define CALL_E_JAVASCRIPT (CALL_E_MAX - 23)
// Permission denied
#define CALL_E_PERMIT (CALL_E_MAX - 24)

#define CALL_E_MIN -100100

#ifndef _WIN32
#define CALL_E_FILE_NOT_FOUND (-ENOENT)
#define CALL_E_PATH_NOT_FOUND (-ENOENT)
#define CALL_E_BAD_FILE (-EBADF)
#else
#define CALL_E_FILE_NOT_FOUND (-ERROR_FILE_NOT_FOUND)
#define CALL_E_PATH_NOT_FOUND (-ERROR_PATH_NOT_FOUND)
#define CALL_E_BAD_FILE (-ERROR_OPERATION_ABORTED)
#endif

#define STREAM_BUFF_SIZE 65536

#if 0
#define V8_SCOPE(isolate) v8::EscapableHandleScope handle_scope(isolate)
#define V8_RETURN(v) handle_scope.Escape(v)
#else
#define V8_SCOPE(isolate) (isolate = isolate)
#define V8_RETURN(v) (v)
#endif

#define PROPERTY_ENTER()                      \
    v8::Isolate* isolate = args.GetIsolate(); \
    V8_SCOPE(isolate);                        \
    result_t hr = 0;                          \
    bool bStrict = false;                     \
    do {                                      \
        do {

#define METHOD_OVER(c, o)                                       \
    }                                                           \
    while (0)                                                   \
        ;                                                       \
    if (hr > CALL_E_MIN_ARG && hr < CALL_E_MAX)                 \
        do {                                                    \
            hr = 0;                                             \
            int32_t argc1 = args.Length();                      \
            int32_t argc = argc1;                               \
            while (argc > (o) && args[argc - 1]->IsUndefined()) \
                argc--;                                         \
            if ((c) >= 0 && argc > (c)) {                       \
                hr = CALL_E_BADPARAMCOUNT;                      \
                break;                                          \
            }                                                   \
            if ((o) > 0 && argc < (o)) {                        \
                hr = CALL_E_PARAMNOTOPTIONAL;                   \
                break;                                          \
            }

#define ASYNC_METHOD_OVER(c, o)                                    \
    }                                                              \
    while (0)                                                      \
        ;                                                          \
    if (hr > CALL_E_MIN_ARG && hr < CALL_E_MAX)                    \
        do {                                                       \
            hr = 0;                                                \
            int32_t argc1 = args.Length();                         \
            v8::Local<v8::Function> cb;                            \
            if (argc1 > 0 && args[argc1 - 1]->IsFunction())        \
                cb = v8::Local<v8::Function>::Cast(args[--argc1]); \
            int32_t argc = argc1;                                  \
            while (argc > (o) && args[argc - 1]->IsUndefined())    \
                argc--;                                            \
            if ((c) >= 0 && argc > (c)) {                          \
                hr = CALL_E_BADPARAMCOUNT;                         \
                break;                                             \
            }                                                      \
            if ((o) > 0 && argc < (o)) {                           \
                hr = CALL_E_PARAMNOTOPTIONAL;                      \
                break;                                             \
            }

#define METHOD_ENTER()                        \
    v8::Isolate* isolate = args.GetIsolate(); \
    V8_SCOPE(isolate);                        \
    result_t hr = CALL_E_BADPARAMCOUNT;       \
    bool bStrict = true;                      \
    do {                                      \
        do {

#define CONSTRUCT_INIT()             \
    if (class_info().init_isolate()) \
        return;

#define CONSTRUCT_ENTER()                \
    if (!args.IsConstructCall()) {       \
        ThrowResult(CALL_E_CONSTRUCTOR); \
        return;                          \
    }                                    \
    METHOD_ENTER()

#define METHOD_INSTANCE(cls)                            \
    obj_ptr<cls> pInst = cls::getInstance(args.This()); \
    if (pInst == NULL) {                                \
        ThrowResult(CALL_E_NOTINSTANCE);                \
        return;                                         \
    }                                                   \
    scope l(pInst);

#define CHECK_ARGUMENT()                                                                                        \
    }                                                                                                           \
    while (0)                                                                                                   \
        ;                                                                                                       \
    if (!bStrict || (hr != CALL_E_BADPARAMCOUNT && hr != CALL_E_PARAMNOTOPTIONAL && hr != CALL_E_TYPEMISMATCH)) \
        break;                                                                                                  \
    bStrict = false;                                                                                            \
    }                                                                                                           \
    while (true)                                                                                                \
        ;

#define PROPERTY_SET_LEAVE() \
    CHECK_ARGUMENT()         \
    if (hr < 0)              \
        ThrowResult(hr);

#define THROW_ERROR()                                                 \
    if (hr == CALL_E_JAVASCRIPT) {                                    \
        args.GetReturnValue().Set(V8_RETURN(v8::Local<v8::Value>())); \
        return;                                                       \
    }                                                                 \
    ThrowResult(hr);                                                  \
    return;

#define METHOD_RETURN()                                                    \
    CHECK_ARGUMENT()                                                       \
    if (hr == CALL_RETURN_NULL) {                                          \
        args.GetReturnValue().SetNull();                                   \
        return;                                                            \
    }                                                                      \
    if (hr >= 0) {                                                         \
        args.GetReturnValue().Set(V8_RETURN(GetReturnValue(isolate, vr))); \
        return;                                                            \
    }                                                                      \
    THROW_ERROR()

#define METHOD_RETURN1()                      \
    CHECK_ARGUMENT()                          \
    args.GetReturnValue().Set(V8_RETURN(vr)); \
    return;

#define METHOD_VOID()                         \
    CHECK_ARGUMENT()                          \
    if (hr >= 0) {                            \
        args.GetReturnValue().SetUndefined(); \
        return;                               \
    }                                         \
    THROW_ERROR()

#define CONSTRUCT_RETURN()                                           \
    CHECK_ARGUMENT()                                                 \
    if (hr >= 0) {                                                   \
        args.GetReturnValue().Set(V8_RETURN(vr->wrap(args.This()))); \
        return;                                                      \
    }                                                                \
    THROW_ERROR()

#define PROPERTY_VAL(t)                                 \
    t v0;                                               \
    hr = GetArgumentValue(isolate, value, v0, bStrict); \
    if (hr < 0)                                         \
        break;

#define ARG(t, n)                                           \
    t v##n;                                                 \
    hr = GetArgumentValue(isolate, args[n], v##n, bStrict); \
    if (hr < 0)                                             \
        break;

#define STRICT_ARG(t, n)                                 \
    t v##n;                                              \
    hr = GetArgumentValue(isolate, args[n], v##n, true); \
    if (hr < 0)                                          \
        break;

#define OPT_ARG(t, n, d)                                        \
    t v##n = (d);                                               \
    if ((n) < argc && !args[n]->IsUndefined()) {                \
        hr = GetArgumentValue(isolate, args[n], v##n, bStrict); \
        if (hr < 0)                                             \
            break;                                              \
    }

#define ARG_LIST(n) OptArgs v##n(args, n, argc1);

#define DECLARE_CLASSINFO(c)                      \
public:                                           \
    static ClassInfo& class_info();               \
    virtual ClassInfo& Classinfo()                \
    {                                             \
        return class_info();                      \
    }                                             \
    static c* getInstance(void* o)                \
    {                                             \
        return (c*)class_info().getInstance(o);   \
    }                                             \
    static c* getInstance(v8::Local<v8::Value> o) \
    {                                             \
        return (c*)class_info().getInstance(o);   \
    }

#define DECLARE_CLASS(c)                      \
public:                                       \
    c() { c::class_info().Ref(); }            \
    virtual ~c() { c::class_info().Unref(); } \
    DECLARE_CLASSINFO(c)

#define DECLARE_MODULE(name)                      \
    class RootModule_##name : public RootModule { \
    public:                                       \
        virtual ClassInfo& class_info()           \
        {                                         \
            return name##_base::class_info();     \
        }                                         \
    } s_RootModule_##name;

#define DECLARE_MODULE_EX(mname, module)           \
    class RootModule_##mname : public RootModule { \
    public:                                        \
        virtual ClassInfo& class_info()            \
        {                                          \
            return module##_base::class_info();    \
        }                                          \
        virtual const char* name()                 \
        {                                          \
            return #mname;                         \
        }                                          \
    } s_RootModule_##mname;

#define EVENT_SUPPORT()                                                                    \
public:                                                                                    \
    virtual result_t on(exlib::string ev, v8::Local<v8::Function> func,                    \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::on(ev, func, retVal);                                          \
    }                                                                                      \
    virtual result_t on(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)          \
    {                                                                                      \
        return object_base::on(map, retVal);                                               \
    }                                                                                      \
    virtual result_t prependListener(exlib::string ev, v8::Local<v8::Function> func,       \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::prependListener(ev, func, retVal);                             \
    }                                                                                      \
    virtual result_t prependListener(v8::Local<v8::Object> map,                            \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::prependListener(map, retVal);                                  \
    }                                                                                      \
    virtual result_t addListener(exlib::string ev, v8::Local<v8::Function> func,           \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::on(ev, func, retVal);                                          \
    }                                                                                      \
    virtual result_t addListener(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal) \
    {                                                                                      \
        return object_base::on(map, retVal);                                               \
    }                                                                                      \
    virtual result_t prependOnceListener(exlib::string ev, v8::Local<v8::Function> func,   \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::prependOnceListener(ev, func, retVal);                         \
    }                                                                                      \
    virtual result_t prependOnceListener(v8::Local<v8::Object> map,                        \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::prependOnceListener(map, retVal);                              \
    }                                                                                      \
    virtual result_t once(exlib::string ev, v8::Local<v8::Function> func,                  \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::once(ev, func, retVal);                                        \
    }                                                                                      \
    virtual result_t once(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)        \
    {                                                                                      \
        return object_base::once(map, retVal);                                             \
    }                                                                                      \
    virtual result_t off(exlib::string ev, v8::Local<v8::Function> func,                   \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::off(ev, func, retVal);                                         \
    }                                                                                      \
    virtual result_t off(exlib::string ev, v8::Local<v8::Object>& retVal)                  \
    {                                                                                      \
        return object_base::off(ev, retVal);                                               \
    }                                                                                      \
    virtual result_t off(v8::Local<v8::Object> map, v8::Local<v8::Object>& retVal)         \
    {                                                                                      \
        return object_base::off(map, retVal);                                              \
    }                                                                                      \
    virtual result_t removeListener(exlib::string ev, v8::Local<v8::Function> func,        \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::off(ev, func, retVal);                                         \
    }                                                                                      \
    virtual result_t removeListener(exlib::string ev, v8::Local<v8::Object>& retVal)       \
    {                                                                                      \
        return object_base::off(ev, retVal);                                               \
    }                                                                                      \
    virtual result_t removeListener(v8::Local<v8::Object> map,                             \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::off(map, retVal);                                              \
    }                                                                                      \
    virtual result_t removeAllListeners(v8::Local<v8::Array> evs,                          \
        v8::Local<v8::Object>& retVal)                                                     \
    {                                                                                      \
        return object_base::removeAllListeners(evs, retVal);                               \
    }                                                                                      \
    virtual result_t setMaxListeners(int32_t n)                                            \
    {                                                                                      \
        return object_base::setMaxListeners(n);                                            \
    }                                                                                      \
    virtual result_t getMaxListeners(int32_t& retVal)                                      \
    {                                                                                      \
        return object_base::getMaxListeners(retVal);                                       \
    }                                                                                      \
    virtual result_t listeners(exlib::string ev, v8::Local<v8::Array>& retVal)             \
    {                                                                                      \
        return object_base::listeners(ev, retVal);                                         \
    }                                                                                      \
    virtual result_t listenerCount(exlib::string ev, int32_t& retVal)                      \
    {                                                                                      \
        return object_base::listenerCount(ev, retVal);                                     \
    }                                                                                      \
    virtual result_t emit(exlib::string ev, OptArgs args,                                  \
        bool& retVal)                                                                      \
    {                                                                                      \
        return object_base::emit(ev, args, retVal);                                        \
    }                                                                                      \
    virtual result_t eventNames(v8::Local<v8::Array>& retVal)                              \
    {                                                                                      \
        return object_base::eventNames(retVal);                                            \
    }

#define EVENT_FUNC(e)                                           \
    virtual result_t get_on##e(v8::Local<v8::Function>& retVal) \
    {                                                           \
        return getListener(#e, retVal);                         \
    }                                                           \
    virtual result_t set_on##e(v8::Local<v8::Function> newVal)  \
    {                                                           \
        return setListener(#e, newVal);                         \
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

#ifndef ARRAYSIZE
#define ARRAYSIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

inline result_t GetArgumentValue(v8::Local<v8::Value> v, exlib::string& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (bStrict && !v->IsString() && !v->IsStringObject())
        return CALL_E_TYPEMISMATCH;

    v8::String::Utf8Value tmp(v);
    n.assign(*tmp, tmp.length());

    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, exlib::string& n, bool bStrict = false)
{
    return GetArgumentValue(v, n, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, double& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsNumber() && !v->IsNumberObject()) {
        if (bStrict)
            return CALL_E_TYPEMISMATCH;

        v = v->ToNumber();
        if (v.IsEmpty())
            return CALL_E_TYPEMISMATCH;
    }

    n = v->NumberValue();
    if (isnan(n))
        return CALL_E_TYPEMISMATCH;

    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, double& n, bool bStrict = false)
{
    return GetArgumentValue(v, n, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, int64_t& n, bool bStrict = false)
{
    double num;

    result_t hr = GetArgumentValue(v, num, bStrict);
    if (hr < 0)
        return hr;

    if (num < -9007199254740992ll || num > 9007199254740992ll)
        return CALL_E_OUTRANGE;

    n = (int64_t)num;

    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, int64_t& n, bool bStrict = false)
{
    return GetArgumentValue(v, n, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, int32_t& n, bool bStrict = false)
{
    double num;

    result_t hr = GetArgumentValue(v, num, bStrict);
    if (hr < 0)
        return hr;

    if (num < -2147483648ll || num > 2147483647ll)
        return CALL_E_OUTRANGE;

    n = (int32_t)num;

    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, int32_t& n, bool bStrict = false)
{
    return GetArgumentValue(v, n, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, bool& n, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (bStrict && !v->IsBoolean() && !v->IsBooleanObject())
        return CALL_E_TYPEMISMATCH;

    n = v->BooleanValue();
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, bool& n, bool bStrict = false)
{
    return GetArgumentValue(v, n, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, date_t& d, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsDate())
        return CALL_E_TYPEMISMATCH;

    d = v;
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, date_t& d, bool bStrict = false)
{
    return GetArgumentValue(v, d, bStrict);
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, Variant& d, bool bStrict = false)
{
    d = v;
    return 0;
}

class OptArgs {
public:
    OptArgs(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t base, int32_t argc)
        : m_args(args)
        , m_base(base)
        , m_argc(argc)
    {
    }

    OptArgs(const OptArgs& a)
        : m_args(a.m_args)
        , m_base(a.m_base)
        , m_argc(a.m_argc)
    {
    }

    int32_t Length() const
    {
        return m_argc - m_base;
    }

    v8::Local<v8::Value> operator[](int32_t i) const
    {
        return m_args[i + m_base];
    }

    void GetData(std::vector<v8::Local<v8::Value>>& datas)
    {
        datas.resize(m_argc - m_base);
        for (int32_t i = 0; i < m_argc - m_base; i++)
            datas[i] = m_args[m_base + i];
    }

private:
    const v8::FunctionCallbackInfo<v8::Value>& m_args;
    int32_t m_base;
    int32_t m_argc;
};

class Value2Args {
public:
    Value2Args(v8::Isolate* isolate, v8::Local<v8::Value>& v, v8::Local<v8::Value>& vr)
        : m_isolate(isolate)
        , m_v(v)
        , m_vr(vr)
    {
    }

    int32_t Length() const
    {
        return 1;
    }

    bool IsConstructCall() const
    {
        return true;
    }

    v8::Local<v8::Object> This() const
    {
        return v8::Local<v8::Object>();
    }

    const Value2Args& GetReturnValue() const
    {
        return *this;
    }

    void Set(v8::Local<v8::Value> vr) const
    {
        m_vr = vr;
    }

    v8::Local<v8::Value>& operator[](size_t i) const
    {
        return m_v;
    }

    v8::Isolate* GetIsolate() const
    {
        return m_isolate;
    }

private:
    v8::Isolate* m_isolate;
    v8::Local<v8::Value>& m_v;
    v8::Local<v8::Value>& m_vr;
};

template <class T>
result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<T>& vr, bool bStrict = false)
{
    vr = T::getInstance(v);
    if (vr == NULL) {
        if (bStrict)
            return CALL_E_TYPEMISMATCH;

        TryCatch try_catch;

        v8::Local<v8::Value> vr1;
        Value2Args a(isolate, v, vr1);

        T::__new(a);
        vr = T::getInstance(vr1);

        if (vr == NULL)
            return CALL_E_TYPEMISMATCH;

        vr->dispose();
    }

    return 0;
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::Object>& vr, bool bStrict = false)
{
    Isolate* isolate = Isolate::current();

    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsObject())
        return CALL_E_TYPEMISMATCH;

    v8::Local<v8::Value> proto;
    if (isolate->m_proto.IsEmpty()) {
        proto = v8::Object::New(isolate->m_isolate)->GetPrototype();
        isolate->m_proto.Reset(isolate->m_isolate, proto);
    } else
        proto = v8::Local<v8::Value>::New(isolate->m_isolate, isolate->m_proto);

    v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(v);
    if (!proto->Equals(o->GetPrototype()))
        return CALL_E_TYPEMISMATCH;

    vr = o;
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v,
    v8::Local<v8::Object>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::Array>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsArray())
        return CALL_E_TYPEMISMATCH;

    vr = v8::Local<v8::Array>::Cast(v);
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, v8::Local<v8::Array>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::TypedArray>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsTypedArray())
        return CALL_E_TYPEMISMATCH;

    vr = v8::Local<v8::TypedArray>::Cast(v);
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v, v8::Local<v8::TypedArray>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::ArrayBuffer>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsArrayBuffer())
        return CALL_E_TYPEMISMATCH;

    vr = v8::Local<v8::ArrayBuffer>::Cast(v);
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v,
    v8::Local<v8::ArrayBuffer>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::ArrayBufferView>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsArrayBufferView())
        return CALL_E_TYPEMISMATCH;

    vr = v8::Local<v8::ArrayBufferView>::Cast(v);
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v,
    v8::Local<v8::ArrayBufferView>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::Value>& vr, bool bStrict = false)
{
    vr = v;
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v,
    v8::Local<v8::Value>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

inline result_t GetArgumentValue(v8::Local<v8::Value> v, v8::Local<v8::Function>& vr, bool bStrict = false)
{
    if (v.IsEmpty())
        return CALL_E_TYPEMISMATCH;

    if (!v->IsFunction())
        return CALL_E_TYPEMISMATCH;

    vr = v8::Local<v8::Function>::Cast(v);
    return 0;
}

inline result_t GetArgumentValue(v8::Isolate* isolate, v8::Local<v8::Value> v,
    v8::Local<v8::Function>& vr, bool bStrict = false)
{
    return GetArgumentValue(v, vr, bStrict);
}

template <typename T>
result_t GetConfigValue(v8::Isolate* isolate, v8::Local<v8::Object> o,
    const char* key, T& n, bool bStrict = false)
{
    v8::Local<v8::Value> v = o->Get(v8::String::NewFromUtf8(isolate, key));
    if (IsEmpty(v))
        return CALL_E_PARAMNOTOPTIONAL;

    return GetArgumentValue(isolate, v, n, bStrict);
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, int32_t v)
{
    return v8::Int32::New(isolate, v);
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, bool v)
{
    return v ? v8::True(isolate) : v8::False(isolate);
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, double v)
{
    return v8::Number::New(isolate, v);
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, int64_t v)
{
    return v8::Number::New(isolate, (double)v);
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, exlib::string& str)
{
    return v8::String::NewFromUtf8(isolate, str.c_str(),
        v8::String::kNormalString, (int32_t)str.length());
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, date_t& v)
{
    return v.value(isolate);
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, Variant& v)
{
    return v;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, v8::Local<v8::Object>& obj)
{
    return obj;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, v8::Local<v8::Array>& array)
{
    return array;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, v8::Local<v8::Value>& value)
{
    return value;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, v8::Local<v8::Function>& func)
{
    return func;
}

template <class T>
inline v8::Local<v8::Value> GetReturnValue(v8::Isolate* isolate, obj_ptr<T>& obj)
{
    return obj->wrap();
}

inline v8::Local<v8::Value> ThrowError(const char* msg)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(v8::Exception::Error(
        isolate->NewFromUtf8(msg)));
}

inline v8::Local<v8::Value> ThrowError(v8::Local<v8::Value> exception)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(exception);
}

inline v8::Local<v8::Value> ThrowTypeError(const char* msg)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(v8::Exception::TypeError(
        isolate->NewFromUtf8(msg)));
}

inline v8::Local<v8::Value> ThrowRangeError(const char* msg)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(v8::Exception::RangeError(
        isolate->NewFromUtf8(msg)));
}

inline v8::Local<v8::Value> ThrowError(exlib::string msg)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(v8::Exception::Error(
        isolate->NewFromUtf8(msg)));
}

inline v8::Local<v8::Value> ThrowTypeError(exlib::string msg)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(v8::Exception::TypeError(
        isolate->NewFromUtf8(msg)));
}

inline v8::Local<v8::Value> ThrowRangeError(exlib::string msg)
{
    Isolate* isolate = Isolate::current();

    return isolate->m_isolate->ThrowException(v8::Exception::RangeError(
        isolate->NewFromUtf8(msg)));
}

inline result_t LastError()
{
#ifdef _WIN32
    return -(int32_t)GetLastError();
#else
    return -errno;
#endif
}

inline result_t SocketError()
{
#ifdef _WIN32
    return -WSAGetLastError();
#else
    return -errno;
#endif
}

exlib::string traceInfo(int32_t deep);
exlib::string getResultMessage(result_t hr);
v8::Local<v8::Value> ThrowResult(result_t hr);
void ReportException(TryCatch& try_catch, result_t hr);
exlib::string GetException(TryCatch& try_catch, result_t hr);
result_t throwSyntaxError(TryCatch& try_catch);

#ifdef _WIN32

#define PATH_SLASH '\\'
#define PATH_DELIMITER ';'

inline bool isPathSlash(char ch)
{
    return ch == '/' || ch == '\\';
}

#else

#define PATH_SLASH '/'
#define PATH_DELIMITER ':'

inline bool isPathSlash(char ch)
{
    return ch == '/';
}

#endif

#define PATH_SLASH_WIN32 '\\'
#define PATH_DELIMITER_WIN32 ';'
#define PATH_SLASH_POSIX '/'
#define PATH_DELIMITER_POSIX ':'

inline bool isWin32PathSlash(char ch)
{
    return ch == '/' || ch == '\\';
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

void errorLog(exlib::string msg);

inline result_t _error_checker(result_t hr, const char* file, int32_t line)
{
    if (hr < 0 && hr != CALL_E_NOSYNC && hr != CALL_E_NOASYNC && hr != CALL_E_LONGSYNC && hr != CALL_E_GUICALL && hr != CALL_E_PENDDING) {
        exlib::string str = file;
        char tmp[64];

        sprintf(tmp, ":%d ", line);
        str.append(tmp);

        errorLog(str + getResultMessage(hr));
    }

    return hr;
}

#ifndef NDEBUG
#define CHECK_ERROR(hr) _error_checker((hr), __FILE__, __LINE__)
#else
#define CHECK_ERROR(hr) (hr)
#endif

#define DEPRECATED_SOON(name)                                       \
    {                                                               \
        static bool once = false;                                   \
        if (!once) {                                                \
            once = true;                                            \
            exlib::string str(name);                                \
            str.append(" is deprecated and will soon be removed."); \
            errorLog(str + traceInfo(16));                          \
        }                                                           \
    }

inline exlib::string niceSize(intptr_t sz)
{
    char buf[64];
    double num = (double)sz;
    double test = num >= 0 ? num : -num;
    int32_t cnt;

    if (test < 1024)
        cnt = sprintf(buf, "%d bytes", (int32_t)sz);
    else if (test < 1024 * 1024)
        cnt = sprintf(buf, "%.1f KB", num / 1024);
    else if (test < 1024 * 1024 * 1024)
        cnt = sprintf(buf, "%.1f MB", num / (1024 * 1024));
    else
        cnt = sprintf(buf, "%.1f GB", num / (1024 * 1024 * 1024));

    return exlib::string(buf, cnt);
}

inline exlib::string dump_str(exlib::string str)
{
    static const char hexs[] = "0123456789abcdef";
    exlib::string strHex;
    int32_t i;

    for (i = 0; i < (int32_t)str.length(); i++) {
        unsigned char ch = (unsigned char)str[i];
        strHex += hexs[ch >> 4];
        strHex += hexs[ch & 0xf];
    }

    return strHex;
}

void flushLog();
}

#endif
