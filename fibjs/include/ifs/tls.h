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

class TLSSocket_base;
class SecureContext_base;
class Stream_base;

class tls_base : public object_base {
    DECLARE_CLASS(tls_base);

public:
    // tls_base
    static result_t createSecureContext(v8::Local<v8::Object> options, bool isServer, obj_ptr<SecureContext_base>& retVal);
    static result_t createSecureContext(bool isServer, obj_ptr<SecureContext_base>& retVal);
    static result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    static result_t connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, SecureContext_base* secureContext, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t connect(exlib::string url, v8::Local<v8::Object> optionns, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_createSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(tls_base, connect, exlib::string, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE4(tls_base, connect, exlib::string, SecureContext_base*, int32_t, obj_ptr<Stream_base>);
    ASYNC_STATICVALUE3(tls_base, connect, exlib::string, v8::Local<v8::Object>, obj_ptr<Stream_base>);
};
}

#include "ifs/TLSSocket.h"
#include "ifs/SecureContext.h"
#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& tls_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createSecureContext", s_static_createSecureContext, true, false },
        { "connect", s_static_connect, true, true },
        { "connectSync", s_static_connect, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "TLSSocket", TLSSocket_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "secureContext", s_static_get_secureContext, block_set, true }
    };

    static ClassData s_cd = {
        "tls", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void tls_base::s_static_createSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Object>, 0);
    OPT_ARG(bool, 1, false);

    hr = createSecureContext(v0, v1, vr);

    METHOD_OVER(1, 0);

    OPT_ARG(bool, 0, false);

    hr = createSecureContext(v0, vr);

    METHOD_RETURN();
}

inline void tls_base::s_static_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    PROPERTY_ENTER();

    hr = get_secureContext(vr);

    METHOD_RETURN();
}

inline void tls_base::s_static_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
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

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<SecureContext_base>, 1);
    OPT_ARG(int32_t, 2, 0);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, v2, cb, args);
    else
        hr = ac_connect(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_connect(v0, v1, cb, args);
    else
        hr = ac_connect(v0, v1, vr);

    METHOD_RETURN();
}
}
