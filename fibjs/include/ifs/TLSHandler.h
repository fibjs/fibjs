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
#include "ifs/Handler.h"

namespace fibjs {

class Handler_base;
class SecureContext_base;

class TLSHandler_base : public Handler_base {
    DECLARE_CLASS(TLSHandler_base);

public:
    // TLSHandler_base
    static result_t _new(SecureContext_base* context, Handler_base* handler, obj_ptr<TLSHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, Handler_base* handler, obj_ptr<TLSHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal) = 0;
    virtual result_t setSecureContext(SecureContext_base* context) = 0;
    virtual result_t setSecureContext(v8::Local<v8::Object> options) = 0;
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
    virtual result_t set_handler(Handler_base* newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_setSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_handler(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_handler(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

#include "ifs/SecureContext.h"

namespace fibjs {
inline ClassInfo& TLSHandler_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setSecureContext", s_setSecureContext, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "secureContext", s_get_secureContext, block_set, false },
        { "handler", s_get_handler, s_set_handler, false }
    };

    static ClassData s_cd = {
        "TLSHandler", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Handler_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TLSHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void TLSHandler_base::__new(const T& args)
{
    obj_ptr<TLSHandler_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void TLSHandler_base::s_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    METHOD_INSTANCE(TLSHandler_base);
    PROPERTY_ENTER();

    hr = pInst->get_secureContext(vr);

    METHOD_RETURN();
}

inline void TLSHandler_base::s_setSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TLSHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<SecureContext_base>, 0);

    hr = pInst->setSecureContext(v0);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->setSecureContext(v0);

    METHOD_VOID();
}

inline void TLSHandler_base::s_get_handler(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_INSTANCE(TLSHandler_base);
    PROPERTY_ENTER();

    hr = pInst->get_handler(vr);

    METHOD_RETURN();
}

inline void TLSHandler_base::s_set_handler(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(TLSHandler_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(obj_ptr<Handler_base>);

    hr = pInst->set_handler(v0);

    PROPERTY_SET_LEAVE();
}
}
