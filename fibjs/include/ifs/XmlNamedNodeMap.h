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

class XmlAttr_base;

class XmlNamedNodeMap_base : public object_base {
    DECLARE_CLASS(XmlNamedNodeMap_base);

public:
    // XmlNamedNodeMap_base
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t item(int32_t index, obj_ptr<XmlAttr_base>& retVal) = 0;
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlAttr_base>& retVal) = 0;
    virtual result_t getNamedItem(exlib::string name, obj_ptr<XmlAttr_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_item(const v8::FunctionCallbackInfo<v8::Value>& args);
    static v8::Intercepted i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_getNamedItem(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/XmlAttr.h"

namespace fibjs {
inline ClassInfo& XmlNamedNodeMap_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "item", s_item, false, ClassData::ASYNC_SYNC },
        { "getNamedItem", s_getNamedItem, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "length", s_get_length, block_set, false }
    };

    static ClassData::ClassIndexed s_indexed = {
        i_IndexedGetter, i_IndexedSetter
    };

    static ClassData s_cd = {
        "XmlNamedNodeMap", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, &s_indexed, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlNamedNodeMap_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(XmlNamedNodeMap_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void XmlNamedNodeMap_base::s_item(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlAttr_base> vr;

    METHOD_INSTANCE(XmlNamedNodeMap_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->item(v0, vr);

    METHOD_RETURN();
}

inline v8::Intercepted XmlNamedNodeMap_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlAttr_base> vr;

    NAMED_METHOD_INSTANCE(XmlNamedNodeMap_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return v8::Intercepted::kNo;

    NAMED_METHOD_RETURN();
}

inline void XmlNamedNodeMap_base::s_getNamedItem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlAttr_base> vr;

    METHOD_INSTANCE(XmlNamedNodeMap_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getNamedItem(v0, vr);

    METHOD_RETURN();
}
}
