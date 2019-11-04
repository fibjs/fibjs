/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _ssl_base_H_
#define _ssl_base_H_

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
        _VERIFY_NONE = 0,
        _VERIFY_OPTIONAL = 1,
        _VERIFY_REQUIRED = 2,
        _BADCERT_EXPIRED = 1,
        _BADCERT_REVOKED = 2,
        _BADCERT_CN_MISMATCH = 4,
        _BADCERT_NOT_TRUSTED = 8,
        _ssl3 = 0,
        _tls1 = 1,
        _tls1_1 = 2,
        _tls1_2 = 3
    };

public:
    // ssl_base
    static result_t connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t setClientCert(X509Cert_base* crt, PKey_base* key);
    static result_t loadClientCertFile(exlib::string crtFile, exlib::string keyFile, exlib::string password);
    static result_t loadRootCerts();
    static result_t get_ca(obj_ptr<X509Cert_base>& retVal);
    static result_t get_verification(int32_t& retVal);
    static result_t set_verification(int32_t newVal);
    static result_t get_min_version(int32_t& retVal);
    static result_t set_min_version(int32_t newVal);
    static result_t get_max_version(int32_t& retVal);
    static result_t set_max_version(int32_t newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadClientCertFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadRootCerts(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_verification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_verification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_min_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_min_version(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_max_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_max_version(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_STATICVALUE3(ssl_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
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
        { "connect", s_static_connect, true },
        { "connectSync", s_static_connect, true },
        { "setClientCert", s_static_setClientCert, true },
        { "loadClientCertFile", s_static_loadClientCertFile, true },
        { "loadRootCerts", s_static_loadRootCerts, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Socket", SslSocket_base::class_info },
        { "Handler", SslHandler_base::class_info },
        { "Server", SslServer_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "ca", s_static_get_ca, block_set, true },
        { "verification", s_static_get_verification, s_static_set_verification, true },
        { "min_version", s_static_get_min_version, s_static_set_min_version, true },
        { "max_version", s_static_get_max_version, s_static_set_max_version, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "VERIFY_NONE", _VERIFY_NONE },
        { "VERIFY_OPTIONAL", _VERIFY_OPTIONAL },
        { "VERIFY_REQUIRED", _VERIFY_REQUIRED },
        { "BADCERT_EXPIRED", _BADCERT_EXPIRED },
        { "BADCERT_REVOKED", _BADCERT_REVOKED },
        { "BADCERT_CN_MISMATCH", _BADCERT_CN_MISMATCH },
        { "BADCERT_NOT_TRUSTED", _BADCERT_NOT_TRUSTED },
        { "ssl3", _ssl3 },
        { "tls1", _tls1 },
        { "tls1_1", _tls1_1 },
        { "tls1_2", _tls1_2 }
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

    METHOD_NAME("ssl.connect");
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

inline void ssl_base::s_static_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("ssl.setClientCert");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);

    hr = setClientCert(v0, v1);

    METHOD_VOID();
}

inline void ssl_base::s_static_loadClientCertFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("ssl.loadClientCertFile");
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = loadClientCertFile(v0, v1, v2);

    METHOD_VOID();
}

inline void ssl_base::s_static_loadRootCerts(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("ssl.loadRootCerts");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = loadRootCerts();

    METHOD_VOID();
}

inline void ssl_base::s_static_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_NAME("ssl.ca");
    PROPERTY_ENTER();

    hr = get_ca(vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_get_verification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("ssl.verification");
    PROPERTY_ENTER();

    hr = get_verification(vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_set_verification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("ssl.verification");
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_verification(v0);

    PROPERTY_SET_LEAVE();
}

inline void ssl_base::s_static_get_min_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("ssl.min_version");
    PROPERTY_ENTER();

    hr = get_min_version(vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_set_min_version(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("ssl.min_version");
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_min_version(v0);

    PROPERTY_SET_LEAVE();
}

inline void ssl_base::s_static_get_max_version(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("ssl.max_version");
    PROPERTY_ENTER();

    hr = get_max_version(vr);

    METHOD_RETURN();
}

inline void ssl_base::s_static_set_max_version(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("ssl.max_version");
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_max_version(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
