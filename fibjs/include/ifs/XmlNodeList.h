/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlNodeList_base_H_
#define _XmlNodeList_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class XmlNode_base;

class XmlNodeList_base : public object_base {
    DECLARE_CLASS(XmlNodeList_base);

public:
    // XmlNodeList_base
    virtual result_t get_length(int32_t& retVal) = 0;
    virtual result_t item(int32_t index, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t _indexed_getter(uint32_t index, obj_ptr<XmlNode_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_length(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_item(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "XmlNode.h"

namespace fibjs {
inline ClassInfo& XmlNodeList_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "item", s_item, false }
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
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlNodeList_base::s_get_length(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("XmlNodeList.length");
    METHOD_INSTANCE(XmlNodeList_base);
    PROPERTY_ENTER();

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void XmlNodeList_base::s_item(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_NAME("XmlNodeList.item");
    METHOD_INSTANCE(XmlNodeList_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->item(v0, vr);

    METHOD_RETURN();
}

inline void XmlNodeList_base::i_IndexedGetter(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_NAME("XmlNodeList.undefined");
    METHOD_INSTANCE(XmlNodeList_base);
    PROPERTY_ENTER();

    hr = pInst->_indexed_getter(index, vr);
    if (hr == CALL_RETURN_NULL)
        return;

    METHOD_RETURN();
}
}

#endif
