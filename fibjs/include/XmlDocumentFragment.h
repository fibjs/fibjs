/*
 * XmlDocumentFragment.h
 *
 *  Created on: Jan 5, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/XmlDocumentFragment.h"
#include "XmlNodeImpl.h"

namespace fibjs {

class XmlDocumentFragment : public XmlDocumentFragment_base, public XmlNodeImpl {
public:
    XmlDocumentFragment(XmlDocument_base* document)
        : XmlNodeImpl(document, this, xml_base::C_DOCUMENT_FRAGMENT_NODE)
    {
    }

    XmlDocumentFragment(const XmlDocumentFragment& from)
        : XmlNodeImpl(from.m_document, this, xml_base::C_DOCUMENT_FRAGMENT_NODE)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeType(int32_t& retVal);
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base>& retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base>& retVal);
    virtual result_t hasChildNodes(bool& retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t get_children(obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_firstElementChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_lastElementChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_previousElementSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_nextElementSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t normalize();
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);
    virtual result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t remove(obj_ptr<XmlNode_base>& retVal);
    virtual result_t replaceWith(OptArgs nodes);
};

} /* namespace fibjs */
