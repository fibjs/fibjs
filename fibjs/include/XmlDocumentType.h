/*
 * XmlDocumentType.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/XmlDocumentType.h"
#include "XmlNodeMixin.h"

namespace fibjs {

class XmlDocumentType : public XmlNodeMixin<XmlDocumentType, XmlDocumentType_base> {
public:
    XmlDocumentType(XmlDocument_base* document, const char* name)
        : XmlNodeMixin<XmlDocumentType, XmlDocumentType_base>(document, this, xml_base::C_DOCUMENT_TYPE_NODE)
        , m_name(name)
    {
    }

    XmlDocumentType(XmlDocument_base* document, const char* name,
        const char* systemId, const char* publicId)
        : XmlNodeMixin<XmlDocumentType, XmlDocumentType_base>(document, this, xml_base::C_DOCUMENT_TYPE_NODE)
        , m_name(name)
        , m_systemId(systemId)
        , m_publicId(publicId)
    {
    }

    XmlDocumentType(const XmlDocumentType& from)
        : XmlNodeMixin<XmlDocumentType, XmlDocumentType_base>(from.m_document, this, xml_base::C_DOCUMENT_TYPE_NODE)
        , m_name(from.m_name)
        , m_systemId(from.m_systemId)
        , m_publicId(from.m_publicId)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);

public:
    // XmlNode_base - custom implementations
    virtual result_t get_nodeName(exlib::string& retVal);
    virtual result_t get_nodeValue(exlib::string& retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_textContent(exlib::string& retVal);
    virtual result_t set_textContent(exlib::string newVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);

public:
    // XmlDocumentType_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_publicId(exlib::string& retVal);
    virtual result_t get_systemId(exlib::string& retVal);

private:
    exlib::string m_name;
    exlib::string m_systemId;
    exlib::string m_publicId;
};

} /* namespace fibjs */
