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

#include <osconfig.h>
#include <errno.h>

#ifdef _WIN32

#include <ws2tcpip.h>
#include <winsock2.h>
#include <mstcpip.h>

#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY 27
#endif

#include <windows.h>

#ifndef EWOULDBLOCK
#define EINPROGRESS         WSAEWOULDBLOCK
#endif

#define SHUT_RD     SD_READ
#define SHUT_WR     SD_SEND
#define SHUT_RDWR   SD_BOTH

#else

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket close

#if defined(FreeBSD) || defined(OpenBSD)
#include <net/if.h>
#include <netinet/tcp.h>

#ifndef ETIME
#define ETIME   ETIMEDOUT
#endif
#else
#include <netinet/tcp.h>

#ifndef TCP_KEEPIDLE
#define TCP_KEEPIDLE TCP_KEEPALIVE
#endif
#endif

#ifndef SOL_TCP
#define SOL_TCP IPPROTO_TCP
#endif

#endif

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

#include <v8/v8.h>
#include <string>
#include <math.h>

#include "qstring.h"
#include "date.h"
#include "obj_ptr.h"
#include "Variant.h"

#include <exlib/fiber.h>
#include <exlib/lockfree.h>

#ifdef _DEBUG
#undef _DEBUG
#endif

namespace fibjs
{

typedef int result_t;

#define CALL_RETURN_NULL        100000

#define CALL_E_MAX              -100000
// Invalid number of parameters.
#define CALL_E_BADPARAMCOUNT    (CALL_E_MAX - 1)
// Parameter not optional.
#define CALL_E_PARAMNOTOPTIONAL (CALL_E_MAX - 2)
// The input parameter is not a valid type.
#define CALL_E_BADVARTYPE       (CALL_E_MAX - 3)
// Invalid argument.
#define CALL_E_INVALIDARG       (CALL_E_MAX - 4)
// The argument could not be coerced to the specified type.
#define CALL_E_TYPEMISMATCH     (CALL_E_MAX - 5)
// Value is out of range.
#define CALL_E_OUTRANGE         (CALL_E_MAX - 6)

#define CALL_E_MIN_ARG          (CALL_E_OUTRANGE - 1)

// Constructor cannot be called as a function.
#define CALL_E_CONSTRUCTOR      (CALL_E_MAX - 7)
// Object is not an instance of declaring class.
#define CALL_E_NOTINSTANCE      (CALL_E_MAX - 8)
// Invalid procedure call.
#define CALL_E_INVALID_CALL      (CALL_E_MAX - 9)
// Invalid input data
#define CALL_E_INVALID_DATA      (CALL_E_MAX - 10)
// Index was out of range.
#define CALL_E_BADINDEX         (CALL_E_MAX - 11)
// Memory overflow error.
#define CALL_E_OVERFLOW         (CALL_E_MAX - 12)
// Collection is empty.
#define CALL_E_EMPTY            (CALL_E_MAX - 13)
// Operation now in progress.
#define CALL_E_PENDDING         (CALL_E_MAX - 14)
// Operation not support synchronous call.
#define CALL_E_NOSYNC           (CALL_E_MAX - 15)
// Operation not support asynchronous call.
#define CALL_E_NOASYNC          (CALL_E_MAX - 16)
// Internal error.
#define CALL_E_INTERNAL         (CALL_E_MAX - 17)
// Invalid return type.
#define CALL_E_RETURN_TYPE      (CALL_E_MAX - 18)
// Exception occurred.
#define CALL_E_EXCEPTION        (CALL_E_MAX - 19)
// Javascript error.
#define CALL_E_JAVASCRIPT       (CALL_E_MAX - 20)

#define CALL_E_MIN              -100100

#if 0
#define V8_SCOPE()  v8::HandleScope handle_scope(isolate)
#define V8_RETURN(v)  handle_scope.Close(v)
#else
#define V8_SCOPE()
#define V8_RETURN(v)  (v)
#endif

#define PROPERTY_ENTER() \
    V8_SCOPE(); \
    result_t hr = 0;do{

#define METHOD_OVER(c, o) \
    }while(0); \
    if(hr > CALL_E_MIN_ARG && hr < CALL_E_MAX)do{hr = 0;\
            int argc; \
            argc = args.Length(); \
            if((c) >= 0 && argc > (c)){hr = CALL_E_BADPARAMCOUNT;break;} \
            if((o) > 0 && argc < (o)){hr = CALL_E_PARAMNOTOPTIONAL;break;}

#define METHOD_ENTER(c, o) \
    V8_SCOPE(); \
    result_t hr = CALL_E_BADPARAMCOUNT; do{\
        METHOD_OVER(c, o)

#define CONSTRUCT_ENTER(c, o) \
    static bool s_bInit = false; \
    if(!s_bInit){s_bInit = true; return;} \
    if (!args.IsConstructCall()){ThrowResult(CALL_E_CONSTRUCTOR); return;} \
    METHOD_ENTER(c, o)

#define METHOD_INSTANCE(cls) \
    obj_ptr<cls> pInst = cls::getInstance(args.This()); \
    if(pInst == NULL){ ThrowResult(CALL_E_NOTINSTANCE); return;} \
    scope l(pInst);

#define PROPERTY_INSTANCE(cls) \
    obj_ptr<cls> pInst = cls::getInstance(args.This()); \
    if(pInst == NULL){hr = CALL_E_NOTINSTANCE;break;} \
    scope l(pInst);

#define PROPERTY_SET_LEAVE() \
    }while(0); \
    if(hr < 0)ThrowResult(hr);

#define METHOD_RETURN() \
    }while(0); \
    if(hr == CALL_RETURN_NULL){ args.GetReturnValue().SetNull(); return;} \
    if(hr >= 0){ args.GetReturnValue().Set(V8_RETURN(GetReturnValue(vr))); return;} \
    if(hr == CALL_E_JAVASCRIPT){ args.GetReturnValue().Set(v8::Local<v8::Value>()); return;} \
    ThrowResult(hr); return;

#define METHOD_RETURN1() \
    }while(0); args.GetReturnValue().Set(V8_RETURN(vr)); return;

#define METHOD_VOID() \
    }while(0); \
    if(hr >= 0){ args.GetReturnValue().Set(v8::Undefined(isolate)); return;} \
    if(hr == CALL_E_JAVASCRIPT){ args.GetReturnValue().Set(v8::Local<v8::Value>()); return;} \
    ThrowResult(hr); return;

#define CONSTRUCT_RETURN() \
    }while(0); \
    if(hr >= 0){ vr->wrap(args.This()); return;} \
    if(hr == CALL_E_JAVASCRIPT){ args.GetReturnValue().Set(v8::Local<v8::Value>()); return;} \
    ThrowResult(hr); return;

#define ARG_String(n) \
    v8::String::Utf8Value tv##n(args[n]); \
    const char* v##n = *tv##n; \
    if(v##n == NULL){hr = CALL_E_INVALIDARG;break;}

#define OPT_ARG_String(n, d) \
    v8::Local<v8::Value> tvv##n; \
    if(n < argc)tvv##n = args[n]; \
    v8::String::Utf8Value tv##n(tvv##n); \
    const char* v##n = ((n) < argc && !args[n]->IsUndefined()) ? *tv##n : (d); \
    if(v##n == NULL){hr = CALL_E_INVALIDARG;break;}

#define PROPERTY_VAL_String() \
    v8::String::Utf8Value tv0(value); \
    const char* v0 = *tv0; \
    if(v0 == NULL){hr = CALL_E_INVALIDARG;break;}

#define PROPERTY_VAL(t) \
    t v0; \
    hr = SafeGetValue(value, v0); \
    if(hr < 0)break;

#define ARG(t, n) \
    t v##n; \
    hr = SafeGetValue(args[n], v##n); \
    if(hr < 0)break;

#define OPT_ARG(t, n, d) \
    t v##n; \
    if((n) < argc){ \
        if(args[n]->IsUndefined())v##n = (d); \
        else {hr = SafeGetValue(args[n], v##n); \
            if(hr < 0)break;} \
    }else v##n = (d);

#define DECLARE_CLASSINFO(c) \
    public: \
    static ClassInfo& class_info(); \
    virtual ClassInfo& Classinfo() \
    {   return class_info();} \
    static c* getInstance(void *o) \
    {   return (c*)class_info().getInstance(o); } \
    static c* getInstance(v8::Local<v8::Value> o) \
    {   return (c*)class_info().getInstance(o); }

#define EVENT_SUPPORT() \
    public: \
    virtual result_t on(const char* ev, v8::Local<v8::Function> func) \
    {   return object_base::on(ev, func);} \
    virtual result_t on(v8::Local<v8::Object> map) \
    {   return object_base::on(map);} \
    virtual result_t once(const char* ev, v8::Local<v8::Function> func) \
    {   return object_base::once(ev, func);} \
    virtual result_t once(v8::Local<v8::Object> map) \
    {   return object_base::once(map);} \
    virtual result_t off(const char* ev, v8::Local<v8::Function> func) \
    {   return object_base::off(ev, func);} \
    virtual result_t off(v8::Local<v8::Object> map) \
    {   return object_base::off(map);} \
    virtual result_t trigger(const char* ev, const v8::FunctionCallbackInfo<v8::Value>& args) \
    {   return object_base::trigger(ev, args);}

#define FIBER_FREE() \
    public: \
    virtual void enter() \
    {} \
    virtual void leave() \
    {} \
     
#ifdef _MSC_VER
#define INFINITY (DBL_MAX+DBL_MAX)
#define NAN (INFINITY-INFINITY)
#define isnan _isnan
#endif

inline result_t SafeGetValue(v8::Local<v8::Value> v, double &n)
{
    if (v.IsEmpty())
        return CALL_E_INVALIDARG;

    if (!v->IsNumber() && !v->IsNumberObject())
    {
        v = v->ToNumber();
        if (v.IsEmpty())
            return CALL_E_INVALIDARG;
    }

    n = v->NumberValue();
    if (isnan(n))
        return CALL_E_INVALIDARG;

    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, int64_t &n)
{
    double num;

    result_t hr = SafeGetValue(v, num);
    if (hr < 0)
        return hr;

    if (num < -9007199254740992ll || num > 9007199254740992ll)
        return CALL_E_OUTRANGE;

    n = (int64_t) num;

    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, int32_t &n)
{
    double num;

    result_t hr = SafeGetValue(v, num);
    if (hr < 0)
        return hr;

    if (num < -2147483648ll || num > 2147483647ll)
        return CALL_E_OUTRANGE;

    n = (int32_t) num;

    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, bool &n)
{
    if (v.IsEmpty())
        return CALL_E_INVALIDARG;

    n = v->BooleanValue();
    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, date_t &d)
{
    if (v.IsEmpty())
        return CALL_E_INVALIDARG;

    if (!v->IsDate())
        return CALL_E_INVALIDARG;

    d = v;
    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, Variant &d)
{
    d = v;
    return 0;
}

template<class T>
inline result_t SafeGetValue(v8::Local<v8::Value> v, obj_ptr<T> &vr)
{
    vr = T::getInstance(v);
    if (vr == NULL)
        return CALL_E_INVALIDARG;

    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v,
                             v8::Local<v8::Object> &vr)
{
    if (v.IsEmpty())
        return CALL_E_INVALIDARG;

    if (!v->IsObject())
        return CALL_E_INVALIDARG;

    vr = v8::Local<v8::Object>::Cast(v);
    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, v8::Local<v8::Array> &vr)
{
    if (v.IsEmpty())
        return CALL_E_INVALIDARG;

    if (!v->IsArray())
        return CALL_E_INVALIDARG;

    vr = v8::Local<v8::Array>::Cast(v);
    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v, v8::Local<v8::Value> &vr)
{
    vr = v;
    return 0;
}

inline result_t SafeGetValue(v8::Local<v8::Value> v,
                             v8::Local<v8::Function> &vr)
{
    if (v.IsEmpty())
        return CALL_E_INVALIDARG;

    if (!v->IsFunction())
        return CALL_E_INVALIDARG;

    vr = v8::Local<v8::Function>::Cast(v);
    return 0;
}

inline v8::Local<v8::Value> GetReturnValue(int32_t v)
{
    return v8::Int32::New(isolate, v);
}

inline v8::Local<v8::Value> GetReturnValue(bool v)
{
    return v ? v8::True(isolate) : v8::False(isolate);
}

inline v8::Local<v8::Value> GetReturnValue(double v)
{
    return v8::Number::New(isolate, v);
}

inline v8::Local<v8::Value> GetReturnValue(int64_t v)
{
    return v8::Number::New(isolate, (double) v);
}

inline v8::Local<v8::Value> GetReturnValue(std::string &str)
{
    return v8::String::NewFromUtf8(isolate, str.c_str(),
                                   v8::String::kNormalString, (int) str.length());
}

inline v8::Local<v8::Value> GetReturnValue(date_t &v)
{
    return v;
}

inline v8::Local<v8::Value> GetReturnValue(Variant &v)
{
    return v;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Local<v8::Object> &obj)
{
    return obj;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Local<v8::Array> &array)
{
    return array;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Local<v8::Value> &value)
{
    return value;
}

inline v8::Local<v8::Value> GetReturnValue(v8::Local<v8::Function> &func)
{
    return func;
}

template<class T>
inline v8::Local<v8::Value> GetReturnValue(obj_ptr<T> &obj)
{
    v8::Local<v8::Value> retVal;
    obj->ValueOf(retVal);

    return retVal;
}

inline v8::Local<v8::Value> ThrowError(const char *msg)
{
    return isolate->ThrowException(v8::Exception::Error(
                                       v8::String::NewFromUtf8(isolate, msg)));
}

inline v8::Local<v8::Value> ThrowTypeError(const char *msg)
{
    return isolate->ThrowException(v8::Exception::TypeError(
                                       v8::String::NewFromUtf8(isolate, msg)));
}

inline v8::Local<v8::Value> ThrowRangeError(const char *msg)
{
    return isolate->ThrowException(v8::Exception::RangeError(
                                       v8::String::NewFromUtf8(isolate, msg)));
}

inline result_t LastError()
{
#ifdef _WIN32
    return - (int)GetLastError();
#else
    return -errno;
#endif
}

inline result_t SocketError()
{
#ifdef _WIN32
    return - WSAGetLastError();
#else
    return -errno;
#endif
}

extern exlib::Service *g_pService;

std::string traceInfo();
std::string getResultMessage(result_t hr);
v8::Local<v8::Value> ThrowResult(result_t hr);
void ReportException(v8::TryCatch &try_catch, result_t hr);
std::string GetException(v8::TryCatch &try_catch, result_t hr);

#ifdef _WIN32

#define PATH_SLASH  '\\'

inline bool isPathSlash(char ch)
{
    return ch == '/' || ch == '\\';
}

#else

#define PATH_SLASH  '/'

inline bool isPathSlash(char ch)
{
    return ch == '/';
}

#endif

#define URL_SLASH   '/'

inline bool isUrlSlash(char ch)
{
    return ch == '/';
}

class _step_checker
{
public:
    static _step_checker &g()
    {
        static _step_checker sc;
        return sc;
    }

    void chk(int n, const char *file, int line)
    {
        int n1 = exlib::atom_inc(&m_step);
        if (n1 != n)
            printf("[%s:%d]: %d, %d\n", file, line, n, n1);
    }

    void rst()
    {
        m_step = 0;
    }

private:
    _step_checker() :
        m_step(0)
    {
    }

    int m_step;
};

#define STEP_CHECK(n) _step_checker::g().chk((n), __FILE__, __LINE__)
#define STEP_RESET() _step_checker::g().rst()

#define COLOR_RESET "\x1b[0m"
#define COLOR_BLACK "\x1b[30m" /* Black */
#define COLOR_RED "\x1b[31m" /* Red */
#define COLOR_GREEN "\x1b[32m" /* Green */
#define COLOR_YELLOW "\x1b[33m" /* Yellow */
#define COLOR_BLUE "\x1b[34m" /* Blue */
#define COLOR_MAGENTA "\x1b[35m" /* Magenta */
#define COLOR_CYAN "\x1b[36m" /* Cyan */
#define COLOR_WHITE "\x1b[37m" /* White */
#define COLOR_NORMAL "\x1b[39m" /* Normal */
#define COLOR_GREY "\x1B[90m" /* Grey */

void flushLog();

}

#endif

