/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _net_base_H_
#define _net_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Socket_base;
class Stream_base;
class Smtp_base;
class TcpServer_base;
class UrlObject_base;

class net_base : public object_base {
    DECLARE_CLASS(net_base);

public:
    enum {
        _AF_INET = 2,
        _AF_INET6 = 10,
        _SOCK_STREAM = 1,
        _SOCK_DGRAM = 2
    };

public:
    // net_base
    static result_t info(v8::Local<v8::Object>& retVal);
    static result_t resolve(exlib::string name, int32_t family, exlib::string& retVal, AsyncEvent* ac);
    static result_t ip(exlib::string name, exlib::string& retVal, AsyncEvent* ac);
    static result_t ipv6(exlib::string name, exlib::string& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t openSmtp(exlib::string url, int32_t timeout, obj_ptr<Smtp_base>& retVal, AsyncEvent* ac);
    static result_t backend(exlib::string& retVal);
    static result_t isIP(exlib::string ip, int32_t& retVal);
    static result_t isIPv4(exlib::string ip, bool& retVal);
    static result_t isIPv6(exlib::string ip, bool& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ipv6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openSmtp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_backend(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isIP(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isIPv4(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_isIPv6(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(net_base, resolve, exlib::string, int32_t, exlib::string);
    ASYNC_STATICVALUE2(net_base, ip, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(net_base, ipv6, exlib::string, exlib::string);
    ASYNC_STATICVALUE3(net_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, openSmtp, exlib::string, int32_t, obj_ptr<Smtp_base>);
};
}

#include "ifs/Socket.h"
#include "ifs/Stream.h"
#include "ifs/Smtp.h"
#include "ifs/TcpServer.h"
#include "ifs/UrlObject.h"

namespace fibjs {
inline ClassInfo& net_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "info", s_static_info, true },
        { "resolve", s_static_resolve, true },
        { "resolveSync", s_static_resolve, true },
        { "ip", s_static_ip, true },
        { "ipSync", s_static_ip, true },
        { "ipv6", s_static_ipv6, true },
        { "ipv6Sync", s_static_ipv6, true },
        { "connect", s_static_connect, true },
        { "connectSync", s_static_connect, true },
        { "openSmtp", s_static_openSmtp, true },
        { "openSmtpSync", s_static_openSmtp, true },
        { "backend", s_static_backend, true },
        { "isIP", s_static_isIP, true },
        { "isIPv4", s_static_isIPv4, true },
        { "isIPv6", s_static_isIPv6, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Socket", Socket_base::class_info },
        { "Smtp", Smtp_base::class_info },
        { "TcpServer", TcpServer_base::class_info },
        { "Url", UrlObject_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "AF_INET", _AF_INET },
        { "AF_INET6", _AF_INET6 },
        { "SOCK_STREAM", _SOCK_STREAM },
        { "SOCK_DGRAM", _SOCK_DGRAM }
    };

    static ClassData s_cd = {
        "net", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void net_base::s_static_info(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_NAME("net.info");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = info(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("net.resolve");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, _AF_INET);

    if (!cb.IsEmpty()) {
        acb_resolve(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_resolve(v0, v1, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_ip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("net.ip");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_ip(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_ip(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_ipv6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("net.ipv6");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        acb_ipv6(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_ipv6(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_NAME("net.connect");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty()) {
        acb_connect(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_connect(v0, v1, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_openSmtp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Smtp_base> vr;

    METHOD_NAME("net.openSmtp");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty()) {
        acb_openSmtp(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_openSmtp(v0, v1, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_backend(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("net.backend");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = backend(vr);

    METHOD_RETURN();
}

inline void net_base::s_static_isIP(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("net.isIP");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = isIP(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_isIPv4(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("net.isIPv4");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = isIPv4(v0, vr);

    METHOD_RETURN();
}

inline void net_base::s_static_isIPv6(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("net.isIPv6");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = isIPv6(v0, vr);

    METHOD_RETURN();
}
}

#endif
