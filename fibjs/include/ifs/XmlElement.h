/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlElement_base_H_
#define _XmlElement_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "XmlNode.h"

namespace fibjs {

class XmlNode_base;
class XmlNamedNodeMap_base;
class XmlNodeList_base;

class XmlElement_base : public XmlNode_base {
    DECLARE_CLASS(XmlElement_base);

public:
    // XmlElement_base
    virtual result_t get_namespaceURI(exlib::string& retVal) = 0;
    virtual result_t get_prefix(exlib::string& retVal) = 0;
    virtual result_t set_prefix(exlib::string newVal) = 0;
    virtual result_t get_localName(exlib::string& retVal) = 0;
    virtual result_t get_tagName(exlib::string& retVal) = 0;
    virtual result_t get_id(exlib::string& retVal) = 0;
    virtual result_t set_id(exlib::string newVal) = 0;
    virtual result_t get_textContent(exlib::string& retVal) = 0;
    virtual result_t set_textContent(exlib::string newVal) = 0;
    virtual result_t get_innerHTML(exlib::string& retVal) = 0;
    virtual result_t set_innerHTML(exlib::string newVal) = 0;
    virtual result_t get_className(exlib::string& retVal) = 0;
    virtual result_t set_className(exlib::string newVal) = 0;
    virtual result_t get_attributes(obj_ptr<XmlNamedNodeMap_base>& retVal) = 0;
    virtual result_t getAttribute(exlib::string name, exlib::string& retVal) = 0;
    virtual result_t getAttributeNS(exlib::string namespaceURI, exlib::string localName, exlib::string& retVal) = 0;
    virtual result_t setAttribute(exlib::string name, exlib::string value) = 0;
    virtual result_t setAttributeNS(exlib::string namespaceURI, exlib::string qualifiedName, exlib::string value) = 0;
    virtual result_t removeAttribute(exlib::string name) = 0;
    virtual result_t removeAttributeNS(exlib::string namespaceURI, exlib::string localName) = 0;
    virtual result_t hasAttribute(exlib::string name, bool& retVal) = 0;
    virtual result_t hasAttributeNS(exlib::string namespaceURI, exlib::string localName, bool& retVal) = 0;
    virtual result_t getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal) = 0;
    virtual result_t getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlNodeList_base>& retVal) = 0;
    virtual result_t getElementById(exlib::string id, obj_ptr<XmlElement_base>& retVal) = 0;
    virtual result_t getElementsByClassName(exlib::string className, obj_ptr<XmlNodeList_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_namespaceURI(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_prefix(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_prefix(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_localName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_tagName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_id(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_id(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_textContent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_textContent(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_innerHTML(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_innerHTML(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_className(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_className(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_attributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_getAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hasAttribute(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hasAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getElementsByTagNameNS(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getElementById(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getElementsByClassName(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "XmlNamedNodeMap.h"
#include "XmlNodeList.h"

namespace fibjs {
inline ClassInfo& XmlElement_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getAttribute", s_getAttribute, false },
        { "getAttributeNS", s_getAttributeNS, false },
        { "setAttribute", s_setAttribute, false },
        { "setAttributeNS", s_setAttributeNS, false },
        { "removeAttribute", s_removeAttribute, false },
        { "removeAttributeNS", s_removeAttributeNS, false },
        { "hasAttribute", s_hasAttribute, false },
        { "hasAttributeNS", s_hasAttributeNS, false },
        { "getElementsByTagName", s_getElementsByTagName, false },
        { "getElementsByTagNameNS", s_getElementsByTagNameNS, false },
        { "getElementById", s_getElementById, false },
        { "getElementsByClassName", s_getElementsByClassName, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "namespaceURI", s_get_namespaceURI, block_set, false },
        { "prefix", s_get_prefix, s_set_prefix, false },
        { "localName", s_get_localName, block_set, false },
        { "tagName", s_get_tagName, block_set, false },
        { "id", s_get_id, s_set_id, false },
        { "textContent", s_get_textContent, s_set_textContent, false },
        { "innerHTML", s_get_innerHTML, s_set_innerHTML, false },
        { "className", s_get_className, s_set_className, false },
        { "attributes", s_get_attributes, block_set, false }
    };

    static ClassData s_cd = {
        "XmlElement", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &XmlNode_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlElement_base::s_get_namespaceURI(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_namespaceURI(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_prefix(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_prefix(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_prefix(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_prefix(v0);

    PROPERTY_SET_LEAVE();
}

inline void XmlElement_base::s_get_localName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_localName(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_tagName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_tagName(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_id(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_id(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_id(v0);

    PROPERTY_SET_LEAVE();
}

inline void XmlElement_base::s_get_textContent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_textContent(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_textContent(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_textContent(v0);

    PROPERTY_SET_LEAVE();
}

inline void XmlElement_base::s_get_innerHTML(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_innerHTML(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_innerHTML(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_innerHTML(v0);

    PROPERTY_SET_LEAVE();
}

inline void XmlElement_base::s_get_className(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_className(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_className(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_className(v0);

    PROPERTY_SET_LEAVE();
}

inline void XmlElement_base::s_get_attributes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNamedNodeMap_base> vr;

    METHOD_INSTANCE(XmlElement_base);
    PROPERTY_ENTER();

    hr = pInst->get_attributes(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_getAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getAttribute(v0, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_getAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->getAttributeNS(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_setAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->setAttribute(v0, v1);

    METHOD_VOID();
}

inline void XmlElement_base::s_setAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(exlib::string, 2);

    hr = pInst->setAttributeNS(v0, v1, v2);

    METHOD_VOID();
}

inline void XmlElement_base::s_removeAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->removeAttribute(v0);

    METHOD_VOID();
}

inline void XmlElement_base::s_removeAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->removeAttributeNS(v0, v1);

    METHOD_VOID();
}

inline void XmlElement_base::s_hasAttribute(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->hasAttribute(v0, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_hasAttributeNS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->hasAttributeNS(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_getElementsByTagName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNodeList_base> vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getElementsByTagName(v0, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_getElementsByTagNameNS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNodeList_base> vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->getElementsByTagNameNS(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_getElementById(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlElement_base> vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getElementById(v0, vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_getElementsByClassName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNodeList_base> vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getElementsByClassName(v0, vr);

    METHOD_RETURN();
}
}

#endif
