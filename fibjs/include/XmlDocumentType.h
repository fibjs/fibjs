/*
 * XmlDocumentType.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlDocumentType.h"
#include "XmlNodeImpl.h"

#ifndef XMLDOCUMENTTYPE_H_
#define XMLDOCUMENTTYPE_H_

namespace fibjs
{

class XmlDocumentType: public XmlDocumentType_base, public XmlNodeImpl
{
public:
    XmlDocumentType(XmlDocument_base *document, const char *name):
        XmlNodeImpl(document, this, xml_base::_DOCUMENT_TYPE_NODE), m_name(name)
    {
    }

    XmlDocumentType(XmlDocument_base *document, const char *name,
                    const char *systemId, const char *publicId):
        XmlNodeImpl(document, this, xml_base::_DOCUMENT_TYPE_NODE),
        m_name(name), m_systemId(systemId), m_publicId(publicId)
    {
    }

    XmlDocumentType(const XmlDocumentType &from):
        XmlNodeImpl(from.m_document, this, xml_base::_DOCUMENT_TYPE_NODE),
        m_name(from.m_name), m_systemId(from.m_systemId), m_publicId(from.m_publicId)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string &retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeType(int32_t &retVal);
    virtual result_t get_nodeName(exlib::string &retVal);
    virtual result_t get_nodeValue(exlib::string &retVal);
    virtual result_t set_nodeValue(exlib::string newVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base> &retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base> &retVal);
    virtual result_t hasChildNodes(bool &retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t normalize();
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string &retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string &retVal);
    virtual result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t insertAfter(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);

public:
    // XmlDocumentType_base
    virtual result_t get_name(exlib::string &retVal);
    virtual result_t get_publicId(exlib::string &retVal);
    virtual result_t get_systemId(exlib::string &retVal);

private:
    exlib::string m_name;
    exlib::string m_systemId;
    exlib::string m_publicId;
};

} /* namespace fibjs */
#endif /* XMLDOCUMENTTYPE_H_ */
