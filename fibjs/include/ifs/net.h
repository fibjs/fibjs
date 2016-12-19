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

namespace fibjs
{

class Socket_base;
class Stream_base;
class Smtp_base;
class TcpServer_base;
class UrlObject_base;

class net_base : public object_base
{
    DECLARE_CLASS(net_base);

public:
    enum{
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
    static result_t connect(exlib::string host, int32_t port, int32_t timeout, int32_t family, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t openSmtp(exlib::string url, int32_t timeout, obj_ptr<Smtp_base>& retVal, AsyncEvent* ac);
    static result_t backend(exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_AF_INET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_AF_INET6(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_info(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ipv6(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_openSmtp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_backend(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(net_base, resolve, exlib::string, int32_t, exlib::string);
    ASYNC_STATICVALUE2(net_base, ip, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(net_base, ipv6, exlib::string, exlib::string);
    ASYNC_STATICVALUE5(net_base, connect, exlib::string, int32_t, int32_t, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(net_base, openSmtp, exlib::string, int32_t, obj_ptr<Smtp_base>);
};

}

#include "Socket.h"
#include "Stream.h"
#include "Smtp.h"
#include "TcpServer.h"
#include "UrlObject.h"

namespace fibjs
{
    inline ClassInfo& net_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"info", s_info, true},
            {"resolve", s_resolve, true},
            {"ip", s_ip, true},
            {"ipv6", s_ipv6, true},
            {"connect", s_connect, true},
            {"openSmtp", s_openSmtp, true},
            {"backend", s_backend, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Socket", Socket_base::class_info},
            {"Smtp", Smtp_base::class_info},
            {"TcpServer", TcpServer_base::class_info},
            {"UrlObject", UrlObject_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"AF_INET", s_get_AF_INET, block_set, true},
            {"AF_INET6", s_get_AF_INET6, block_set, true},
            {"SOCK_STREAM", s_get_SOCK_STREAM, block_set, true},
            {"SOCK_DGRAM", s_get_SOCK_DGRAM, block_set, true}
        };

        static ClassData s_cd = 
        { 
            "net", s__new, NULL, 
            7, s_method, 4, s_object, 4, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void net_base::s_get_AF_INET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _AF_INET;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void net_base::s_get_AF_INET6(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _AF_INET6;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void net_base::s_get_SOCK_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _SOCK_STREAM;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void net_base::s_get_SOCK_DGRAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _SOCK_DGRAM;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void net_base::s_info(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Object> vr;

        METHOD_ENTER(0, 0);

        hr = info(vr);

        METHOD_RETURN();
    }

    inline void net_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        ASYNC_METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(int32_t, 1, _AF_INET);

        if(!cb.IsEmpty()) {
            acb_resolve(v0, v1, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_resolve(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void net_base::s_ip(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(exlib::string, 0);

        if(!cb.IsEmpty()) {
            acb_ip(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_ip(v0, vr);

        METHOD_RETURN();
    }

    inline void net_base::s_ipv6(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        ASYNC_METHOD_ENTER(1, 1);

        ARG(exlib::string, 0);

        if(!cb.IsEmpty()) {
            acb_ipv6(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_ipv6(v0, vr);

        METHOD_RETURN();
    }

    inline void net_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Stream_base> vr;

        ASYNC_METHOD_ENTER(4, 2);

        ARG(exlib::string, 0);
        ARG(int32_t, 1);
        OPT_ARG(int32_t, 2, 0);
        OPT_ARG(int32_t, 3, net_base::_AF_INET);

        if(!cb.IsEmpty()) {
            acb_connect(v0, v1, v2, v3, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_connect(v0, v1, v2, v3, vr);

        METHOD_OVER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(int32_t, 1, 0);

        if(!cb.IsEmpty()) {
            acb_connect(v0, v1, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_connect(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void net_base::s_openSmtp(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Smtp_base> vr;

        ASYNC_METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(int32_t, 1, 0);

        if(!cb.IsEmpty()) {
            acb_openSmtp(v0, v1, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_openSmtp(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void net_base::s_backend(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_ENTER(0, 0);

        hr = backend(vr);

        METHOD_RETURN();
    }

}

#endif

