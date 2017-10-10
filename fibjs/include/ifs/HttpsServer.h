/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpsServer_base_H_
#define _HttpsServer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HttpServer.h"

namespace fibjs {

class HttpServer_base;
class Handler_base;
class X509Cert_base;
class PKey_base;

class HttpsServer_base : public HttpServer_base {
    DECLARE_CLASS(HttpsServer_base);

public:
    // HttpsServer_base
    static result_t _new(v8::Local<v8::Array> certs, int32_t port, Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Array> certs, exlib::string addr, int32_t port, Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(X509Cert_base* crt, PKey_base* key, int32_t port, Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port, Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_verification(int32_t& retVal) = 0;
    virtual result_t set_verification(int32_t newVal) = 0;
    virtual result_t get_ca(obj_ptr<X509Cert_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "Handler.h"
#include "X509Cert.h"
#include "PKey.h"

namespace fibjs {
inline ClassInfo& HttpsServer_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "verification", s_get_verification, s_set_verification, false },
        { "ca", s_get_ca, block_set, false }
    };

    static ClassData s_cd = {
        "HttpsServer", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &HttpServer_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpsServer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpsServer_base::__new(const T& args)
{
    obj_ptr<HttpsServer_base> vr;

    CONSTRUCT_ENTER("new HttpsServer()");

    METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Array>, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Handler_base>, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Array>, 0);
    ARG(exlib::string, 1);
    ARG(int32_t, 2);
    ARG(obj_ptr<Handler_base>, 3);

    hr = _new(v0, v1, v2, v3, vr, args.This());

    METHOD_OVER(4, 4);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);
    ARG(int32_t, 2);
    ARG(obj_ptr<Handler_base>, 3);

    hr = _new(v0, v1, v2, v3, vr, args.This());

    METHOD_OVER(5, 5);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);
    ARG(exlib::string, 2);
    ARG(int32_t, 3);
    ARG(obj_ptr<Handler_base>, 4);

    hr = _new(v0, v1, v2, v3, v4, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpsServer_base::s_get_verification(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpsServer_base);
    PROPERTY_ENTER("HttpsServer.verification");

    hr = pInst->get_verification(vr);

    METHOD_RETURN();
}

inline void HttpsServer_base::s_set_verification(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpsServer_base);
    PROPERTY_ENTER("HttpsServer.verification");
    PROPERTY_VAL(int32_t);

    hr = pInst->set_verification(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpsServer_base::s_get_ca(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_INSTANCE(HttpsServer_base);
    PROPERTY_ENTER("HttpsServer.ca");

    hr = pInst->get_ca(vr);

    METHOD_RETURN();
}
}

#endif
