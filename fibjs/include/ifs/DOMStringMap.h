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

class DOMStringMap_base : public object_base {
    DECLARE_CLASS(DOMStringMap_base);

public:
    // DOMStringMap_base
    virtual result_t _named_getter(exlib::string property, Variant& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
    virtual result_t _named_setter(exlib::string property, Variant newVal) = 0;
    virtual result_t _named_deleter(exlib::string property, v8::Local<v8::Boolean>& retVal) = 0;
    virtual result_t _indexed_getter(uint32_t index, Variant& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<DOMStringMap_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static v8::Intercepted i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args);
    static v8::Intercepted i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static v8::Intercepted i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args);
    static v8::Intercepted i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& DOMStringMap_base::class_info()
{
    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData::ClassNamed s_named = {
        i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
    };

    static ClassData s_cd = {
        "DOMStringMap", false, s__new, NULL,
        0, NULL, 0, NULL, 0, NULL, 0, NULL, &s_indexed, &s_named,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline v8::Intercepted DOMStringMap_base::i_NamedGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    Variant vr;

    NAMED_METHOD_INSTANCE(DOMStringMap_base);
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

inline void DOMStringMap_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_INSTANCE(DOMStringMap_base);
    PROPERTY_ENTER();

    hr = pInst->_named_enumerator(vr);

    METHOD_RETURN1();
}

inline v8::Intercepted DOMStringMap_base::i_NamedSetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    NAMED_METHOD_INSTANCE(DOMStringMap_base);
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

inline v8::Intercepted DOMStringMap_base::i_NamedDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean>& args)
{
    v8::Local<v8::Boolean> vr;

    NAMED_METHOD_INSTANCE(DOMStringMap_base);
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

inline v8::Intercepted DOMStringMap_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    Variant vr;

    NAMED_METHOD_INSTANCE(DOMStringMap_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_RETURN();
}
}
