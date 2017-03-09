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

namespace fibjs
{

class SslSocket_base;
class SslHandler_base;
class SslServer_base;
class Stream_base;
class X509Cert_base;
class PKey_base;

class ssl_base : public object_base
{
    DECLARE_CLASS(ssl_base);

public:
    enum{
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
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_VERIFY_NONE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_VERIFY_OPTIONAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_VERIFY_REQUIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BADCERT_EXPIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BADCERT_REVOKED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BADCERT_CN_MISMATCH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BADCERT_NOT_TRUSTED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_ssl3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_tls1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_tls1_1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_tls1_2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadClientCertFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_min_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_min_version(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_max_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_max_version(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);

public:
    ASYNC_STATICVALUE3(ssl_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
};

}

#include "SslSocket.h"
#include "SslHandler.h"
#include "SslServer.h"
#include "Stream.h"
#include "X509Cert.h"
#include "PKey.h"

namespace fibjs
{
    inline ClassInfo& ssl_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"connect", s_connect, true},
            {"setClientCert", s_setClientCert, true},
            {"loadClientCertFile", s_loadClientCertFile, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Socket", SslSocket_base::class_info},
            {"Handler", SslHandler_base::class_info},
            {"Server", SslServer_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"VERIFY_NONE", s_get_VERIFY_NONE, block_set, true},
            {"VERIFY_OPTIONAL", s_get_VERIFY_OPTIONAL, block_set, true},
            {"VERIFY_REQUIRED", s_get_VERIFY_REQUIRED, block_set, true},
            {"BADCERT_EXPIRED", s_get_BADCERT_EXPIRED, block_set, true},
            {"BADCERT_REVOKED", s_get_BADCERT_REVOKED, block_set, true},
            {"BADCERT_CN_MISMATCH", s_get_BADCERT_CN_MISMATCH, block_set, true},
            {"BADCERT_NOT_TRUSTED", s_get_BADCERT_NOT_TRUSTED, block_set, true},
            {"ssl3", s_get_ssl3, block_set, true},
            {"tls1", s_get_tls1, block_set, true},
            {"tls1_1", s_get_tls1_1, block_set, true},
            {"tls1_2", s_get_tls1_2, block_set, true},
            {"ca", s_get_ca, block_set, true},
            {"verification", s_get_verification, s_set_verification, true},
            {"min_version", s_get_min_version, s_set_min_version, true},
            {"max_version", s_get_max_version, s_set_max_version, true}
        };

        static ClassData s_cd = 
        { 
            "ssl", true, s__new, NULL, 
            ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void ssl_base::s_get_VERIFY_NONE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _VERIFY_NONE;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_VERIFY_OPTIONAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _VERIFY_OPTIONAL;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_VERIFY_REQUIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _VERIFY_REQUIRED;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_BADCERT_EXPIRED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BADCERT_EXPIRED;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_BADCERT_REVOKED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BADCERT_REVOKED;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_BADCERT_CN_MISMATCH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BADCERT_CN_MISMATCH;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_BADCERT_NOT_TRUSTED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BADCERT_NOT_TRUSTED;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_ssl3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _ssl3;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_tls1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _tls1;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_tls1_1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _tls1_1;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_get_tls1_2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _tls1_2;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void ssl_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Stream_base> vr;

        METHOD_ENTER();

        ASYNC_METHOD_OVER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(int32_t, 1, 0);

        if(!cb.IsEmpty()) {
            acb_connect(v0, v1, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_connect(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void ssl_base::s_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<X509Cert_base>, 0);
        ARG(obj_ptr<PKey_base>, 1);

        hr = setClientCert(v0, v1);

        METHOD_VOID();
    }

    inline void ssl_base::s_loadClientCertFile(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER();

        METHOD_OVER(3, 2);

        ARG(exlib::string, 0);
        ARG(exlib::string, 1);
        OPT_ARG(exlib::string, 2, "");

        hr = loadClientCertFile(v0, v1, v2);

        METHOD_VOID();
    }

    inline void ssl_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<X509Cert_base> vr;

        PROPERTY_ENTER();

        hr = get_ca(vr);

        METHOD_RETURN();
    }

    inline void ssl_base::s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_verification(vr);

        METHOD_RETURN();
    }

    inline void ssl_base::s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_verification(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void ssl_base::s_get_min_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_min_version(vr);

        METHOD_RETURN();
    }

    inline void ssl_base::s_set_min_version(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_min_version(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void ssl_base::s_get_max_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_max_version(vr);

        METHOD_RETURN();
    }

    inline void ssl_base::s_set_max_version(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_max_version(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

