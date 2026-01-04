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

class DOMTokenList_base : public object_base {
    DECLARE_CLASS(DOMTokenList_base);

public:
    using object_base::toString;

public:
    // DOMTokenList_base
    virtual result_t _indexed_getter(uint32_t index, exlib::string& retVal) = 0;
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t get_value(exlib::string& retVal) = 0;
    virtual result_t item(int32_t index, exlib::string& retVal) = 0;
    virtual result_t contains(exlib::string token, bool& retVal) = 0;
    virtual result_t add(OptArgs tokens) = 0;
    virtual result_t remove(OptArgs tokens) = 0;
    virtual result_t toggle(exlib::string token, OptArgs force, bool& retVal) = 0;
    virtual result_t replace(exlib::string oldToken, exlib::string newToken, bool& retVal) = 0;
    virtual result_t toString(exlib::string& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<DOMTokenList_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static v8::Intercepted i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_value(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_item(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_contains(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toggle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_replace(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& DOMTokenList_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "item", s_item, false, ClassData::ASYNC_SYNC },
        { "contains", s_contains, false, ClassData::ASYNC_SYNC },
        { "add", s_add, false, ClassData::ASYNC_SYNC },
        { "remove", s_remove, false, ClassData::ASYNC_SYNC },
        { "toggle", s_toggle, false, ClassData::ASYNC_SYNC },
        { "replace", s_replace, false, ClassData::ASYNC_SYNC },
        { "toString", s_toString, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "length", s_get_length, block_set, false },
        { "value", s_get_value, block_set, false }
    };

    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData s_cd = {
        "DOMTokenList", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, &s_indexed, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline v8::Intercepted DOMTokenList_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    NAMED_METHOD_INSTANCE(DOMTokenList_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_RETURN();
}

inline void DOMTokenList_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void DOMTokenList_base::s_get_value(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_value(vr);

    METHOD_RETURN();
}

inline void DOMTokenList_base::s_item(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->item(v0, vr);

    METHOD_RETURN();
}

inline void DOMTokenList_base::s_contains(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->contains(v0, vr);

    METHOD_RETURN();
}

inline void DOMTokenList_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->add(v0);

    METHOD_VOID();
}

inline void DOMTokenList_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->remove(v0);

    METHOD_VOID();
}

inline void DOMTokenList_base::s_toggle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(exlib::string, 0);
    ARG_LIST(1);

    hr = pInst->toggle(v0, v1, vr);

    METHOD_RETURN();
}

inline void DOMTokenList_base::s_replace(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->replace(v0, v1, vr);

    METHOD_RETURN();
}

inline void DOMTokenList_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(DOMTokenList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->toString(vr);

    METHOD_OVER(0, 0);

    hr = pInst->toString(vr);

    METHOD_RETURN();
}
}
