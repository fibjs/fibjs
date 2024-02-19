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

class KeyObject_base : public object_base {
    DECLARE_CLASS(KeyObject_base);

public:
    // KeyObject_base
    virtual result_t get_asymmetricKeyDetails(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_asymmetricKeyType(exlib::string& retVal) = 0;
    virtual result_t get_symmetricKeySize(int32_t& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t _export(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t equals(KeyObject_base* otherKey, bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_asymmetricKeyDetails(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_asymmetricKeyType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_symmetricKeySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_type(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s__export(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_equals(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& KeyObject_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "export", s__export, false, false },
        { "equals", s_equals, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "asymmetricKeyDetails", s_get_asymmetricKeyDetails, block_set, false },
        { "asymmetricKeyType", s_get_asymmetricKeyType, block_set, false },
        { "symmetricKeySize", s_get_symmetricKeySize, block_set, false },
        { "type", s_get_type, block_set, false }
    };

    static ClassData s_cd = {
        "KeyObject", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void KeyObject_base::s_get_asymmetricKeyDetails(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(KeyObject_base);
    PROPERTY_ENTER();

    hr = pInst->get_asymmetricKeyDetails(vr);

    METHOD_RETURN();
}

inline void KeyObject_base::s_get_asymmetricKeyType(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(KeyObject_base);
    PROPERTY_ENTER();

    hr = pInst->get_asymmetricKeyType(vr);

    METHOD_RETURN();
}

inline void KeyObject_base::s_get_symmetricKeySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(KeyObject_base);
    PROPERTY_ENTER();

    hr = pInst->get_symmetricKeySize(vr);

    METHOD_RETURN();
}

inline void KeyObject_base::s_get_type(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(KeyObject_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void KeyObject_base::s__export(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(KeyObject_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = pInst->_export(v0, vr);

    METHOD_RETURN();
}

inline void KeyObject_base::s_equals(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(KeyObject_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<KeyObject_base>, 0);

    hr = pInst->equals(v0, vr);

    METHOD_RETURN();
}
}
