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

class XmlNode_base;
class Iterator_base;

class XmlNodeList_base : public object_base {
    DECLARE_CLASS(XmlNodeList_base);

public:
    // XmlNodeList_base
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t item(int32_t index, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t symbol_iterator(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t forEach(v8::Local<v8::Function> callback) = 0;
    virtual result_t keys(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t values(obj_ptr<Iterator_base>& retVal) = 0;
    virtual result_t entries(obj_ptr<Iterator_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<XmlNodeList_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_item(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Intercepted i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_symbol_iterator(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_values(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_entries(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/XmlNode.h"
#include "ifs/Iterator.h"

namespace fibjs {
inline ClassInfo& XmlNodeList_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "item", s_item, false, ClassData::ASYNC_SYNC },
        { "@iterator", s_symbol_iterator, false, ClassData::ASYNC_SYNC },
        { "forEach", s_forEach, false, ClassData::ASYNC_SYNC },
        { "keys", s_keys, false, ClassData::ASYNC_SYNC },
        { "values", s_values, false, ClassData::ASYNC_SYNC },
        { "entries", s_entries, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "length", s_get_length, block_set, false }
    };

    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData s_cd = {
        "XmlNodeList", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, &s_indexed, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlNodeList_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void XmlNodeList_base::s_item(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->item(v0, vr);

    METHOD_RETURN();
}

inline v8::Intercepted XmlNodeList_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    NAMED_METHOD_INSTANCE(XmlNodeList_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_RETURN();
}

inline void XmlNodeList_base::s_symbol_iterator(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->symbol_iterator(vr);

    METHOD_RETURN();
}

inline void XmlNodeList_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->forEach(v0);

    METHOD_VOID();
}

inline void XmlNodeList_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->keys(vr);

    METHOD_RETURN();
}

inline void XmlNodeList_base::s_values(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->values(vr);

    METHOD_RETURN();
}

inline void XmlNodeList_base::s_entries(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Iterator_base> vr;

    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->entries(vr);

    METHOD_RETURN();
}
}
