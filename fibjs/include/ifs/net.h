/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Socket_base;
class Stream_base;
class Smtp_base;
class TcpServer_base;
class Handler_base;
class UrlObject_base;

class net_base : public object_base {
    DECLARE_CLASS(net_base);

public:
    enum {
        C_AF_UNIX = 1,
        C_AF_PIPE = 1,
        C_AF_INET = 2,
        C_AF_INET6 = 10
    };

public:
    // net_base
    static result_t get_use_uv_socket(bool& retVal);
    static result_t set_use_uv_socket(bool newVal);
    static result_t info(v8::Local<v8::Object>& retVal);
    static result_t resolve(exlib::string name, int32_t family, exlib::string& retVal, AsyncEvent* ac);
    static result_t ip(exlib::string name, exlib::string& retVal, AsyncEvent* ac);
    static result_t ipv6(exlib::string name, exlib::string& retVal, AsyncEvent* ac);
    static result_t connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(int32_t port, exlib::string host, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string path, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string path, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t openSmtp(exlib::string url, int32_t timeout, obj_ptr<Smtp_base>& retVal, AsyncEvent* ac);
    static result_t createServer(v8::Local<v8::Object> options, Handler_base* listener, obj_ptr<TcpServer_base>& retVal);
    static result_t createServer(Handler_base* listener, obj_ptr<TcpServer_base>& retVal);
    static result_t backend(exlib::string& retVal);
    static result_t isIP(exlib::string ip, int32_t& retVal);
    static result_t isIPv4(exlib::string ip, bool& retVal);
    static result_t isIPv6(exlib::string ip, bool& retVal);
    static result_t getDefaultAutoSelectFamily(bool& retVal);
    static result_t setDefaultAutoSelectFamily(bool enabled);
    static result_t getDefaultAutoSelectFamilyAttemptTimeout(int32_t& retVal);
    static result_t setDefaultAutoSelectFamilyAttemptTimeout(int32_t milliseconds);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<net_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_get_use_uv_socket(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_use_uv_socket(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ipv6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openSmtp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createServer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_backend(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isIP(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isIPv4(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isIPv6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getDefaultAutoSelectFamily(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setDefaultAutoSelectFamily(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getDefaultAutoSelectFamilyAttemptTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setDefaultAutoSelectFamilyAttemptTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(net_base, resolve, exlib::string, int32_t, exlib::string);
    ASYNC_STATICVALUE2(net_base, ip, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(net_base, ipv6, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(net_base, connect, v8::Local<v8::Object>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, connect, v8::Local<v8::Object>, v8::Local<v8::Function>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE4(net_base, connect, int32_t, exlib::string, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, connect, int32_t, v8::Local<v8::Function>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE4(net_base, connect, int32_t, exlib::string, v8::Local<v8::Function>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE5(net_base, connect, int32_t, exlib::string, int32_t, v8::Local<v8::Function>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, connect, exlib::string, v8::Local<v8::Function>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE4(net_base, connect, exlib::string, int32_t, v8::Local<v8::Function>, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, openSmtp, exlib::string, int32_t, obj_ptr<Smtp_base>);
};
}

#include "ifs/Socket.h"
#include "ifs/Stream.h"
#include "ifs/Smtp.h"
#include "ifs/TcpServer.h"
#include "ifs/Handler.h"
#include "ifs/UrlObject.h"

namespace fibjs {
inline ClassInfo& net_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "info", s_static_info, true, ClassData::ASYNC_SYNC },
        { "resolve", s_static_resolve, true, ClassData::ASYNC_ASYNC },
        { "ip", s_static_ip, true, ClassData::ASYNC_ASYNC },
        { "ipv6", s_static_ipv6, true, ClassData::ASYNC_ASYNC },
        { "connect", s_static_connect, true, ClassData::ASYNC_ASYNC },
        { "openSmtp", s_static_openSmtp, true, ClassData::ASYNC_ASYNC },
        { "createServer", s_static_createServer, true, ClassData::ASYNC_SYNC },
        { "backend", s_static_backend, true, ClassData::ASYNC_SYNC },
        { "isIP", s_static_isIP, true, ClassData::ASYNC_SYNC },
        { "isIPv4", s_static_isIPv4, true, ClassData::ASYNC_SYNC },
        { "isIPv6", s_static_isIPv6, true, ClassData::ASYNC_SYNC },
        { "getDefaultAutoSelectFamily", s_static_getDefaultAutoSelectFamily, true, ClassData::ASYNC_SYNC },
        { "setDefaultAutoSelectFamily", s_static_setDefaultAutoSelectFamily, true, ClassData::ASYNC_SYNC },
        { "getDefaultAutoSelectFamilyAttemptTimeout", s_static_getDefaultAutoSelectFamilyAttemptTimeout, true, ClassData::ASYNC_SYNC },
        { "setDefaultAutoSelectFamilyAttemptTimeout", s_static_setDefaultAutoSelectFamilyAttemptTimeout, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Socket", Socket_base::class_info },
        { "Smtp", Smtp_base::class_info },
        { "TcpServer", TcpServer_base::class_info },
        { "Url", UrlObject_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "use_uv_socket", s_static_get_use_uv_socket, s_static_set_use_uv_socket, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "AF_UNIX", ClassData::CONST_Integer, { .intValue = C_AF_UNIX } },
        { "AF_PIPE", ClassData::CONST_Integer, { .intValue = C_AF_PIPE } },
        { "AF_INET", ClassData::CONST_Integer, { .intValue = C_AF_INET } },
        { "AF_INET6", ClassData::CONST_Integer, { .intValue = C_AF_INET6 } }
    };

    static ClassData s_cd = {
        "net", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void net_base::s_static_get_use_uv_socket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_use_uv_socket(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_set_use_uv_socket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = set_use_uv_socket(v0);

    METHOD_VOID();
}

inline void net_base::s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = info(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER("net.resolve");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, C_AF_INET);

    if (!cb.IsEmpty())
        hr = acb_resolve(v0, v1, cb, args);
    else
        hr = ac_resolve(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void net_base::s_static_ip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER("net.ip");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_ip(v0, cb, args);
    else
        hr = ac_ip(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void net_base::s_static_ipv6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER("net.ipv6");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_ipv6(v0, cb, args);
    else
        hr = ac_ipv6(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void net_base::s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    ASYNC_METHOD_ENTER_FUNC("net.connect");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, cb, args);
    else
        hr = ac_connect(v0, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(exlib::string, 1, "localhost");
    OPT_ARG(int32_t, 2, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, cb, args);
    else
        hr = ac_connect(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(v8::Local<v8::Function>, 1);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Function>, 2);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, cb, args);
    else
        hr = ac_connect(v0, v1, v2, vr);

    METHOD_OVER(4, 4);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    ARG(int32_t, 2);
    ARG(v8::Local<v8::Function>, 3);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, v3, cb, args);
    else
        hr = ac_connect(v0, v1, v2, v3, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(v8::Local<v8::Function>, 2);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, cb, args);
    else
        hr = ac_connect(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void net_base::s_static_openSmtp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Smtp_base> vr;

    ASYNC_METHOD_ENTER("net.openSmtp");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty())
        hr = acb_openSmtp(v0, v1, cb, args);
    else
        hr = ac_openSmtp(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void net_base::s_static_createServer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<TcpServer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = createServer(v0, v1.get(), vr);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = createServer(v0.get(), vr);

    METHOD_RETURN();
}

inline void net_base::s_static_backend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = backend(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_isIP(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = isIP(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_isIPv4(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = isIPv4(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_isIPv6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = isIPv6(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_getDefaultAutoSelectFamily(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getDefaultAutoSelectFamily(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_setDefaultAutoSelectFamily(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = setDefaultAutoSelectFamily(v0);

    METHOD_VOID();
}

inline void net_base::s_static_getDefaultAutoSelectFamilyAttemptTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getDefaultAutoSelectFamilyAttemptTimeout(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_setDefaultAutoSelectFamilyAttemptTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = setDefaultAutoSelectFamilyAttemptTimeout(v0);

    METHOD_VOID();
}
}
