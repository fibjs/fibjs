/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _XmlNode_base_H_
#define _XmlNode_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class XmlDocument_base;
class XmlNodeList_base;

class XmlNode_base : public object_base {
    DECLARE_CLASS(XmlNode_base);

public:
    // XmlNode_base
    virtual result_t get_nodeType(int32_t& retVal) = 0;
    virtual result_t get_nodeName(exlib::string& retVal) = 0;
    virtual result_t get_nodeValue(exlib::string& retVal) = 0;
    virtual result_t set_nodeValue(exlib::string newVal) = 0;
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base>& retVal) = 0;
    virtual result_t get_parentNode(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t hasChildNodes(bool& retVal) = 0;
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base>& retVal) = 0;
    virtual result_t get_firstChild(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_lastChild(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t normalize() = 0;
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal) = 0;
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal) = 0;
    virtual result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_nodeType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_nodeName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_nodeValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_nodeValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ownerDocument(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_parentNode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_hasChildNodes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_childNodes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_firstChild(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_lastChild(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_previousSibling(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_nextSibling(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_cloneNode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lookupPrefix(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lookupNamespaceURI(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insertAfter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_appendChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_replaceChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeChild(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "XmlDocument.h"
#include "XmlNodeList.h"

namespace fibjs {
inline ClassInfo& XmlNode_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "hasChildNodes", s_hasChildNodes, false },
        { "normalize", s_normalize, false },
        { "cloneNode", s_cloneNode, false },
        { "lookupPrefix", s_lookupPrefix, false },
        { "lookupNamespaceURI", s_lookupNamespaceURI, false },
        { "insertBefore", s_insertBefore, false },
        { "insertAfter", s_insertAfter, false },
        { "appendChild", s_appendChild, false },
        { "replaceChild", s_replaceChild, false },
        { "removeChild", s_removeChild, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "nodeType", s_get_nodeType, block_set, false },
        { "nodeName", s_get_nodeName, block_set, false },
        { "nodeValue", s_get_nodeValue, s_set_nodeValue, false },
        { "ownerDocument", s_get_ownerDocument, block_set, false },
        { "parentNode", s_get_parentNode, block_set, false },
        { "childNodes", s_get_childNodes, block_set, false },
        { "firstChild", s_get_firstChild, block_set, false },
        { "lastChild", s_get_lastChild, block_set, false },
        { "previousSibling", s_get_previousSibling, block_set, false },
        { "nextSibling", s_get_nextSibling, block_set, false }
    };

    static ClassData s_cd = {
        "XmlNode", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlNode_base::s_get_nodeType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.nodeType");

    hr = pInst->get_nodeType(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nodeName(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.nodeName");

    hr = pInst->get_nodeName(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nodeValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.nodeValue");

    hr = pInst->get_nodeValue(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_set_nodeValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.nodeValue");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_nodeValue(v0);

    PROPERTY_SET_LEAVE();
}

inline void XmlNode_base::s_get_ownerDocument(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlDocument_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.ownerDocument");

    hr = pInst->get_ownerDocument(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_parentNode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.parentNode");

    hr = pInst->get_parentNode(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_hasChildNodes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.hasChildNodes");

    METHOD_OVER(0, 0);

    hr = pInst->hasChildNodes(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_childNodes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNodeList_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.childNodes");

    hr = pInst->get_childNodes(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_firstChild(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.firstChild");

    hr = pInst->get_firstChild(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_lastChild(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.lastChild");

    hr = pInst->get_lastChild(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_previousSibling(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.previousSibling");

    hr = pInst->get_previousSibling(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nextSibling(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    PROPERTY_ENTER("XmlNode.nextSibling");

    hr = pInst->get_nextSibling(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.normalize");

    METHOD_OVER(0, 0);

    hr = pInst->normalize();

    METHOD_VOID();
}

inline void XmlNode_base::s_cloneNode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.cloneNode");

    METHOD_OVER(1, 0);

    OPT_ARG(bool, 0, true);

    hr = pInst->cloneNode(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_lookupPrefix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.lookupPrefix");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->lookupPrefix(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_lookupNamespaceURI(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.lookupNamespaceURI");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->lookupNamespaceURI(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.insertBefore");

    METHOD_OVER(2, 2);

    ARG(obj_ptr<XmlNode_base>, 0);
    ARG(obj_ptr<XmlNode_base>, 1);

    hr = pInst->insertBefore(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_insertAfter(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.insertAfter");

    METHOD_OVER(2, 2);

    ARG(obj_ptr<XmlNode_base>, 0);
    ARG(obj_ptr<XmlNode_base>, 1);

    hr = pInst->insertAfter(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_appendChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.appendChild");

    METHOD_OVER(1, 1);

    ARG(obj_ptr<XmlNode_base>, 0);

    hr = pInst->appendChild(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_replaceChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.replaceChild");

    METHOD_OVER(2, 2);

    ARG(obj_ptr<XmlNode_base>, 0);
    ARG(obj_ptr<XmlNode_base>, 1);

    hr = pInst->replaceChild(v0, v1, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_removeChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER("XmlNode.removeChild");

    METHOD_OVER(1, 1);

    ARG(obj_ptr<XmlNode_base>, 0);

    hr = pInst->removeChild(v0, vr);

    METHOD_RETURN();
}
}

#endif
