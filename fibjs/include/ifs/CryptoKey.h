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

class CryptoKey_base : public object_base {
    DECLARE_CLASS(CryptoKey_base);

public:
    // CryptoKey_base
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_algorithm(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_extractable(bool& retVal) = 0;
    virtual result_t get_usages(v8::Local<v8::Array>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_type(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_algorithm(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_extractable(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_usages(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& CryptoKey_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "type", s_get_type, block_set, false },
        { "algorithm", s_get_algorithm, block_set, false },
        { "extractable", s_get_extractable, block_set, false },
        { "usages", s_get_usages, block_set, false }
    };

    static ClassData s_cd = {
        "CryptoKey", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void CryptoKey_base::s_get_type(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(CryptoKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void CryptoKey_base::s_get_algorithm(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(CryptoKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_algorithm(vr);

    METHOD_RETURN();
}

inline void CryptoKey_base::s_get_extractable(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(CryptoKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_extractable(vr);

    METHOD_RETURN();
}

inline void CryptoKey_base::s_get_usages(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(CryptoKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_usages(vr);

    METHOD_RETURN();
}
}
