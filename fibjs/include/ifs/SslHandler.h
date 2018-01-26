/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SslHandler_base_H_
#define _SslHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs {

class Handler_base;
class X509Cert_base;
class PKey_base;

class SslHandler_base : public Handler_base {
    DECLARE_CLASS(SslHandler_base);

public:
    // SslHandler_base
    static result_t _new(v8::Local<v8::Array> certs, Handler_base* hdlr, obj_ptr<SslHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(X509Cert_base* crt, PKey_base* key, Handler_base* hdlr, obj_ptr<SslHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_verification(int32_t& retVal) = 0;
    virtual result_t set_verification(int32_t newVal) = 0;
    virtual result_t get_ca(obj_ptr<X509Cert_base>& retVal) = 0;
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
    virtual result_t set_handler(Handler_base* newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_verification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_verification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_handler(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_handler(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

#include "X509Cert.h"
#include "PKey.h"

namespace fibjs {
inline ClassInfo& SslHandler_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "verification", s_get_verification, s_set_verification, false },
        { "ca", s_get_ca, block_set, false },
        { "handler", s_get_handler, s_set_handler, false }
    };

    static ClassData s_cd = {
        "SslHandler", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Handler_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void SslHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void SslHandler_base::__new(const T& args)
{
    obj_ptr<SslHandler_base> vr;

    METHOD_NAME("new SslHandler()");
    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(3, 3);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);
    ARG(obj_ptr<Handler_base>, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void SslHandler_base::s_get_verification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("SslHandler.verification");
    METHOD_INSTANCE(SslHandler_base);
    PROPERTY_ENTER();

    hr = pInst->get_verification(vr);

    METHOD_RETURN();
}

inline void SslHandler_base::s_set_verification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("SslHandler.verification");
    METHOD_INSTANCE(SslHandler_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_verification(v0);

    PROPERTY_SET_LEAVE();
}

inline void SslHandler_base::s_get_ca(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_NAME("SslHandler.ca");
    METHOD_INSTANCE(SslHandler_base);
    PROPERTY_ENTER();

    hr = pInst->get_ca(vr);

    METHOD_RETURN();
}

inline void SslHandler_base::s_get_handler(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_NAME("SslHandler.handler");
    METHOD_INSTANCE(SslHandler_base);
    PROPERTY_ENTER();

    hr = pInst->get_handler(vr);

    METHOD_RETURN();
}

inline void SslHandler_base::s_set_handler(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("SslHandler.handler");
    METHOD_INSTANCE(SslHandler_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(obj_ptr<Handler_base>);

    hr = pInst->set_handler(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
