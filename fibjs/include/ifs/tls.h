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

class SecureContext_base;

class tls_base : public object_base {
    DECLARE_CLASS(tls_base);

public:
    // tls_base
    static result_t createSecureContext(v8::Local<v8::Object> options, obj_ptr<SecureContext_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_createSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/SecureContext.h"

namespace fibjs {
inline ClassInfo& tls_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createSecureContext", s_static_createSecureContext, true, false }
    };

    static ClassData s_cd = {
        "tls", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void tls_base::s_static_createSecureContext(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = createSecureContext(v0, vr);

    METHOD_RETURN();
}
}
