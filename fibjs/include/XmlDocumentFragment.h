/*
 * XmlDocumentFragment.h
 *
 *  Created on: Jan 5, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/XmlDocumentFragment.h"
#include "XmlNodeMixin.h"

namespace fibjs {

class XmlDocumentFragment : public XmlNodeMixin<XmlDocumentFragment, XmlDocumentFragment_base> {
public:
    XmlDocumentFragment(XmlDocument_base* document)
        : XmlNodeMixin<XmlDocumentFragment, XmlDocumentFragment_base>(document, this, xml_base::C_DOCUMENT_FRAGMENT_NODE)
    {
    }

    XmlDocumentFragment(const XmlDocumentFragment& from)
        : XmlNodeMixin<XmlDocumentFragment, XmlDocumentFragment_base>(from.m_document, this, xml_base::C_DOCUMENT_FRAGMENT_NODE)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base - custom implementations (DocumentFragment-specific behavior)
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_previousElementSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_nextElementSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);
    virtual result_t remove(obj_ptr<XmlNode_base>& retVal);
    virtual result_t replaceWith(OptArgs nodes);
    virtual result_t before(OptArgs nodes);
    virtual result_t after(OptArgs nodes);
};

} /* namespace fibjs */
