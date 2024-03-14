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
#include "ifs/TcpServer.h"

namespace fibjs {

class TcpServer_base;
class SecureContext_base;
class Handler_base;

class TLSServer_base : public TcpServer_base {
    DECLARE_CLASS(TLSServer_base);

public:
    // TLSServer_base
    static result_t _new(SecureContext_base* context, int32_t port, Handler_base* listener, obj_ptr<TLSServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* listener, obj_ptr<TLSServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, Handler_base* listener, obj_ptr<TLSServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "ifs/SecureContext.h"
#include "ifs/Handler.h"

namespace fibjs {
inline ClassInfo& TLSServer_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "secureContext", s_get_secureContext, block_set, false }
    };

    static ClassData s_cd = {
        "TLSServer", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &TcpServer_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TLSServer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void TLSServer_base::__new(const T& args)
{
    obj_ptr<TLSServer_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(3, 3);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Handler_base>, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(4, 4);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(exlib::string, 1);
    ARG(int32_t, 2);
    ARG(obj_ptr<Handler_base>, 3);

    hr = _new(v0, v1, v2, v3, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void TLSServer_base::s_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    METHOD_INSTANCE(TLSServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_secureContext(vr);

    METHOD_RETURN();
}
}
