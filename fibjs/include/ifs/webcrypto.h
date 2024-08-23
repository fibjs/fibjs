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

class subtle_base;

class webcrypto_base : public object_base {
    DECLARE_CLASS(webcrypto_base);

public:
    // webcrypto_base
    static result_t getRandomValues(v8::Local<v8::TypedArray> data, v8::Local<v8::TypedArray>& retVal);
    static result_t randomUUID(exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_getRandomValues(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomUUID(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/subtle.h"

namespace fibjs {
inline ClassInfo& webcrypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getRandomValues", s_static_getRandomValues, true, ClassData::ASYNC_SYNC },
        { "randomUUID", s_static_randomUUID, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "subtle", subtle_base::class_info }
    };

    static ClassData s_cd = {
        "webcrypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void webcrypto_base::s_static_getRandomValues(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::TypedArray> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::TypedArray>, 0);

    hr = getRandomValues(v0, vr);

    METHOD_RETURN();
}

inline void webcrypto_base::s_static_randomUUID(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = randomUUID(vr);

    METHOD_RETURN();
}
}
