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
#include "ifs/XmlNode.h"

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
    virtual result_t get_innerHTML(exlib::string& retVal) = 0;
    virtual result_t set_innerHTML(exlib::string newVal) = 0;
    virtual result_t get_outerHTML(exlib::string& retVal) = 0;
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

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_namespaceURI(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_prefix(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_prefix(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_localName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_tagName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_innerHTML(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_innerHTML(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_outerHTML(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_className(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_className(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_attributes(const v8::FunctionCallbackInfo<v8::Value>& args);
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

#include "ifs/XmlNamedNodeMap.h"
#include "ifs/XmlNodeList.h"

namespace fibjs {
inline ClassInfo& XmlElement_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getAttribute", s_getAttribute, false, ClassData::ASYNC_SYNC },
        { "getAttributeNS", s_getAttributeNS, false, ClassData::ASYNC_SYNC },
        { "setAttribute", s_setAttribute, false, ClassData::ASYNC_SYNC },
        { "setAttributeNS", s_setAttributeNS, false, ClassData::ASYNC_SYNC },
        { "removeAttribute", s_removeAttribute, false, ClassData::ASYNC_SYNC },
        { "removeAttributeNS", s_removeAttributeNS, false, ClassData::ASYNC_SYNC },
        { "hasAttribute", s_hasAttribute, false, ClassData::ASYNC_SYNC },
        { "hasAttributeNS", s_hasAttributeNS, false, ClassData::ASYNC_SYNC },
        { "getElementsByTagName", s_getElementsByTagName, false, ClassData::ASYNC_SYNC },
        { "getElementsByTagNameNS", s_getElementsByTagNameNS, false, ClassData::ASYNC_SYNC },
        { "getElementById", s_getElementById, false, ClassData::ASYNC_SYNC },
        { "getElementsByClassName", s_getElementsByClassName, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "namespaceURI", s_get_namespaceURI, block_set, false },
        { "prefix", s_get_prefix, s_set_prefix, false },
        { "localName", s_get_localName, block_set, false },
        { "tagName", s_get_tagName, block_set, false },
        { "id", s_get_id, s_set_id, false },
        { "innerHTML", s_get_innerHTML, s_set_innerHTML, false },
        { "outerHTML", s_get_outerHTML, block_set, false },
        { "className", s_get_className, s_set_className, false },
        { "attributes", s_get_attributes, block_set, false }
    };

    static ClassData s_cd = {
        "XmlElement", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &XmlNode_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlElement_base::s_get_namespaceURI(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_namespaceURI(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_prefix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_prefix(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_prefix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_prefix(v0);

    METHOD_VOID();
}

inline void XmlElement_base::s_get_localName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_localName(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_tagName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_tagName(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_id(v0);

    METHOD_VOID();
}

inline void XmlElement_base::s_get_innerHTML(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_innerHTML(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_innerHTML(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_innerHTML(v0);

    METHOD_VOID();
}

inline void XmlElement_base::s_get_outerHTML(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_outerHTML(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_get_className(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_className(vr);

    METHOD_RETURN();
}

inline void XmlElement_base::s_set_className(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_className(v0);

    METHOD_VOID();
}

inline void XmlElement_base::s_get_attributes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNamedNodeMap_base> vr;

    METHOD_INSTANCE(XmlElement_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

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
