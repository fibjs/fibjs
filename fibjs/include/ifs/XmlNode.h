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
    virtual result_t get_children(obj_ptr<XmlNodeList_base>& retVal) = 0;
    virtual result_t get_firstChild(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_lastChild(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_firstElementChild(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_lastElementChild(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_previousElementSibling(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_nextElementSibling(obj_ptr<XmlNode_base>& retVal) = 0;
    virtual result_t get_textContent(exlib::string& retVal) = 0;
    virtual result_t set_textContent(exlib::string newVal) = 0;
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

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_nodeType(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_nodeName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_nodeValue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_nodeValue(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ownerDocument(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_parentNode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hasChildNodes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_childNodes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_children(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_firstChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_previousSibling(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_nextSibling(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_firstElementChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastElementChild(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_previousElementSibling(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_nextElementSibling(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_textContent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_textContent(const v8::FunctionCallbackInfo<v8::Value>& args);
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

#include "ifs/XmlDocument.h"
#include "ifs/XmlNodeList.h"

namespace fibjs {
inline ClassInfo& XmlNode_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "hasChildNodes", s_hasChildNodes, false, ClassData::ASYNC_SYNC },
        { "normalize", s_normalize, false, ClassData::ASYNC_SYNC },
        { "cloneNode", s_cloneNode, false, ClassData::ASYNC_SYNC },
        { "lookupPrefix", s_lookupPrefix, false, ClassData::ASYNC_SYNC },
        { "lookupNamespaceURI", s_lookupNamespaceURI, false, ClassData::ASYNC_SYNC },
        { "insertBefore", s_insertBefore, false, ClassData::ASYNC_SYNC },
        { "insertAfter", s_insertAfter, false, ClassData::ASYNC_SYNC },
        { "appendChild", s_appendChild, false, ClassData::ASYNC_SYNC },
        { "replaceChild", s_replaceChild, false, ClassData::ASYNC_SYNC },
        { "removeChild", s_removeChild, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "nodeType", s_get_nodeType, block_set, false },
        { "nodeName", s_get_nodeName, block_set, false },
        { "nodeValue", s_get_nodeValue, s_set_nodeValue, false },
        { "ownerDocument", s_get_ownerDocument, block_set, false },
        { "parentNode", s_get_parentNode, block_set, false },
        { "childNodes", s_get_childNodes, block_set, false },
        { "children", s_get_children, block_set, false },
        { "firstChild", s_get_firstChild, block_set, false },
        { "lastChild", s_get_lastChild, block_set, false },
        { "previousSibling", s_get_previousSibling, block_set, false },
        { "nextSibling", s_get_nextSibling, block_set, false },
        { "firstElementChild", s_get_firstElementChild, block_set, false },
        { "lastElementChild", s_get_lastElementChild, block_set, false },
        { "previousElementSibling", s_get_previousElementSibling, block_set, false },
        { "nextElementSibling", s_get_nextElementSibling, block_set, false },
        { "textContent", s_get_textContent, s_set_textContent, false }
    };

    static ClassData s_cd = {
        "XmlNode", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void XmlNode_base::s_get_nodeType(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nodeType(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nodeName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nodeName(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nodeValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nodeValue(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_set_nodeValue(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_nodeValue(v0);

    METHOD_VOID();
}

inline void XmlNode_base::s_get_ownerDocument(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlDocument_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ownerDocument(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_parentNode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_parentNode(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_hasChildNodes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->hasChildNodes(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_childNodes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNodeList_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_childNodes(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_children(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNodeList_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_children(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_firstChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_firstChild(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_lastChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastChild(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_previousSibling(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_previousSibling(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nextSibling(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nextSibling(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_firstElementChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_firstElementChild(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_lastElementChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastElementChild(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_previousElementSibling(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_previousElementSibling(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_nextElementSibling(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_nextElementSibling(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_get_textContent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_textContent(vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_set_textContent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_textContent(v0);

    METHOD_VOID();
}

inline void XmlNode_base::s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->normalize();

    METHOD_VOID();
}

inline void XmlNode_base::s_cloneNode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(bool, 0, true);

    hr = pInst->cloneNode(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_lookupPrefix(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->lookupPrefix(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_lookupNamespaceURI(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->lookupNamespaceURI(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

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
    METHOD_ENTER();

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
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<XmlNode_base>, 0);

    hr = pInst->appendChild(v0, vr);

    METHOD_RETURN();
}

inline void XmlNode_base::s_replaceChild(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<XmlNode_base> vr;

    METHOD_INSTANCE(XmlNode_base);
    METHOD_ENTER();

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
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<XmlNode_base>, 0);

    hr = pInst->removeChild(v0, vr);

    METHOD_RETURN();
}
}
