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

class CSSStyleDeclaration_base : public object_base {
    DECLARE_CLASS(CSSStyleDeclaration_base);

public:
    // CSSStyleDeclaration_base
    virtual result_t get_cssText(exlib::string& retVal) = 0;
    virtual result_t set_cssText(exlib::string newVal) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t item(int32_t index, exlib::string& retVal) = 0;
    virtual result_t getPropertyValue(exlib::string property, exlib::string& retVal) = 0;
    virtual result_t getPropertyPriority(exlib::string property, exlib::string& retVal) = 0;
    virtual result_t setProperty(exlib::string property, exlib::string value, exlib::string priority) = 0;
    virtual result_t removeProperty(exlib::string property, exlib::string& retVal) = 0;
    virtual result_t _named_getter(exlib::string property, Variant& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t _named_setter(exlib::string property, Variant newVal) = 0;
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<CSSStyleDeclaration_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_cssText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_cssText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_item(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPropertyValue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPropertyPriority(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Intercepted i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
    static v8::Intercepted i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static v8::Intercepted i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args);
};
}

namespace fibjs {
inline ClassInfo& CSSStyleDeclaration_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "item", s_item, false, ClassData::ASYNC_SYNC },
        { "getPropertyValue", s_getPropertyValue, false, ClassData::ASYNC_SYNC },
        { "getPropertyPriority", s_getPropertyPriority, false, ClassData::ASYNC_SYNC },
        { "setProperty", s_setProperty, false, ClassData::ASYNC_SYNC },
        { "removeProperty", s_removeProperty, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "cssText", s_get_cssText, s_set_cssText, false },
        { "length", s_get_length, block_set, false }
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "CSSStyleDeclaration", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, &s_named,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void CSSStyleDeclaration_base::s_get_cssText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_cssText(vr);

    METHOD_RETURN();
}

inline void CSSStyleDeclaration_base::s_set_cssText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_cssText(v0);

    METHOD_VOID();
}

inline void CSSStyleDeclaration_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void CSSStyleDeclaration_base::s_item(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->item(v0, vr);

    METHOD_RETURN();
}

inline void CSSStyleDeclaration_base::s_getPropertyValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getPropertyValue(v0, vr);

    METHOD_RETURN();
}

inline void CSSStyleDeclaration_base::s_getPropertyPriority(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getPropertyPriority(v0, vr);

    METHOD_RETURN();
}

inline void CSSStyleDeclaration_base::s_setProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = pInst->setProperty(v0, v1, v2);

    METHOD_VOID();
}

inline void CSSStyleDeclaration_base::s_removeProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->removeProperty(v0, vr);

    METHOD_RETURN();
}

inline v8::Intercepted CSSStyleDeclaration_base::i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    Variant vr;

    NAMED_METHOD_INSTANCE(CSSStyleDeclaration_base);
    PROPERTY_ENTER();

    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str()))
        return v8::Intercepted::kNo;

    hr = pInst->_named_getter(k, vr);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_RETURN();
}

inline void CSSStyleDeclaration_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(CSSStyleDeclaration_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}

inline v8::Intercepted CSSStyleDeclaration_base::i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    NAMED_METHOD_INSTANCE(CSSStyleDeclaration_base);
    PROPERTY_ENTER();

    PROPERTY_VAL(Variant);
    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str()))
        return v8::Intercepted::kNo;

    hr = pInst->_named_setter(k, v0);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_VOID();
}

inline v8::Intercepted CSSStyleDeclaration_base::i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)
{
    v8::Local<v8::Boolean> vr;

    NAMED_METHOD_INSTANCE(CSSStyleDeclaration_base);
    PROPERTY_ENTER();

    exlib::string k;
    GetArgumentValue(isolate, property, k);
    if (class_info().has(k.c_str())) {
        args.GetReturnValue().Set(v8::False(isolate->m_isolate));
        return v8::Intercepted::kYes;
    }

    hr = pInst->_named_deleter(k, vr);
    METHOD_RETURN2();
}
}
