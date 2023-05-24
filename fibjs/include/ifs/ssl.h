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

class SslSocket_base;
class SslHandler_base;
class SslServer_base;
class Stream_base;
class X509Cert_base;
class PKey_base;

class ssl_base : public object_base {
    DECLARE_CLASS(ssl_base);

public:
    enum {
        C_VERIFY_NONE = 0,
        C_VERIFY_OPTIONAL = 1,
        C_VERIFY_REQUIRED = 2,
        C_BADCERT_EXPIRED = 1,
        C_BADCERT_REVOKED = 2,
        C_BADCERT_CN_MISMATCH = 4,
        C_BADCERT_NOT_TRUSTED = 8
    };

public:
    // ssl_base
    static result_t connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, X509Cert_base* crt, PKey_base* key, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, int32_t verification, X509Cert_base* crt, PKey_base* key, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t setClientCert(X509Cert_base* crt, PKey_base* key);
    static result_t loadRootCerts();
    static result_t get_ca(obj_ptr<X509Cert_base>& retVal);
    static result_t get_verification(int32_t& retVal);
    static result_t set_verification(int32_t newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadRootCerts(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_verification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_verification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_STATICVALUE3(ssl_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE5(ssl_base, connect, exlib::string, X509Cert_base*, PKey_base*, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE6(ssl_base, connect, exlib::string, int32_t, X509Cert_base*, PKey_base*, int32_t, obj_ptr<Stream_base>);
};
}

#include "ifs/SslSocket.h"
#include "ifs/SslHandler.h"
#include "ifs/SslServer.h"
#include "ifs/Stream.h"
#include "ifs/X509Cert.h"
#include "ifs/PKey.h"

namespace fibjs {
inline ClassInfo& ssl_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_static_connect, true, true },
        { "connectSync", s_static_connect, true, false },
        { "setClientCert", s_static_setClientCert, true, false },
        { "loadRootCerts", s_static_loadRootCerts, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Socket", SslSocket_base::class_info },
        { "Handler", SslHandler_base::class_info },
        { "Server", SslServer_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "ca", s_static_get_ca, block_set, true },
        { "verification", s_static_get_verification, s_static_set_verification, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "VERIFY_NONE", C_VERIFY_NONE },
        { "VERIFY_OPTIONAL", C_VERIFY_OPTIONAL },
        { "VERIFY_REQUIRED", C_VERIFY_REQUIRED },
        { "BADCERT_EXPIRED", C_BADCERT_EXPIRED },
        { "BADCERT_REVOKED", C_BADCERT_REVOKED },
        { "BADCERT_CN_MISMATCH", C_BADCERT_CN_MISMATCH },
        { "BADCERT_NOT_TRUSTED", C_BADCERT_NOT_TRUSTED }
    };

    static ClassData s_cd = {
        "ssl", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ssl_base::s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    ASYNC_METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<X509Cert_base>, 1);
    ARG(obj_ptr<PKey_base>, 2);
    OPT_ARG(int32_t, 3, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, v3, cb, args);
    else
        hr = ac_connect(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(5, 4);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<X509Cert_base>, 2);
    ARG(obj_ptr<PKey_base>, 3);
    OPT_ARG(int32_t, 4, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_connect(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);

    hr = setClientCert(v0, v1);

    METHOD_VOID();
}

inline void ssl_base::s_static_loadRootCerts(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = loadRootCerts();

    METHOD_VOID();
}

inline void ssl_base::s_static_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    PROPERTY_ENTER();

    hr = get_ca(vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_get_verification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_verification(vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_set_verification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_verification(v0);

    PROPERTY_SET_LEAVE();
}
}
