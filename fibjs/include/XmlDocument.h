/*
 * XmlDocument.h
 *
 *  Created on: Sep 8, 2014
 *      Author: lion
 */

#include "ifs/XmlDocument.h"
#include "XmlNodeImpl.h"

#ifndef XMLDOCUMENT_H_
#define XMLDOCUMENT_H_

namespace fibjs {

class XmlDocument : public XmlDocument_base, public XmlNodeImpl {
public:
    XmlDocument(bool isXml)
        : XmlNodeImpl(NULL, this, xml_base::_DOCUMENT_NODE)
        , m_isXml(isXml)
        , m_standalone(-1)
    {
    }

    XmlDocument(const XmlDocument& from)
        : XmlNodeImpl(NULL, this, xml_base::_DOCUMENT_NODE)
        , m_isXml(from.m_isXml)
        , m_version(from.m_version)
        , m_encoding(from.m_encoding)
        , m_standalone(from.m_standalone)
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
    virtual result_t get_firstChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base>& retVal);
    virtual result_t normalize();
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal);
    virtual result_t lookupPrefix(exlib::string namespaceURI, exlib::string& retVal);
    virtual result_t lookupNamespaceURI(exlib::string prefix, exlib::string& retVal);
    virtual result_t insertBefore(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t insertAfter(XmlNode_base* newChild, XmlNode_base* refChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);
    virtual result_t removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal);

public:
    // XmlDocument_base
    virtual result_t load(exlib::string source);
    virtual result_t load(Buffer_base* source);
    virtual result_t get_inputEncoding(exlib::string& retVal);
    virtual result_t get_xmlStandalone(bool& retVal);
    virtual result_t set_xmlStandalone(bool newVal);
    virtual result_t get_xmlVersion(exlib::string& retVal);
    virtual result_t set_xmlVersion(exlib::string newVal);
    virtual result_t get_doctype(obj_ptr<XmlDocumentType_base>& retVal);
    virtual result_t get_head(obj_ptr<XmlElement_base>& retVal);
    virtual result_t get_title(exlib::string& retVal);
    virtual result_t get_body(obj_ptr<XmlElement_base>& retVal);
    virtual result_t get_documentElement(obj_ptr<XmlElement_base>& retVal);
    virtual result_t getElementsByTagName(exlib::string tagName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t getElementsByTagNameNS(exlib::string namespaceURI, exlib::string localName, obj_ptr<XmlNodeList_base>& retVal);
    virtual result_t createElement(exlib::string tagName, obj_ptr<XmlElement_base>& retVal);
    virtual result_t createElementNS(exlib::string namespaceURI, exlib::string qualifiedName, obj_ptr<XmlElement_base>& retVal);
    virtual result_t createTextNode(exlib::string data, obj_ptr<XmlText_base>& retVal);
    virtual result_t createComment(exlib::string data, obj_ptr<XmlComment_base>& retVal);
    virtual result_t createCDATASection(exlib::string data, obj_ptr<XmlCDATASection_base>& retVal);
    virtual result_t createProcessingInstruction(exlib::string target, exlib::string data, obj_ptr<XmlProcessingInstruction_base>& retVal);

public:
    void setDecl(const char* version, const char* encoding, int32_t standalone)
    {
        m_version = version ? version : "";
        m_encoding = encoding ? encoding : "";
        m_standalone = standalone;
    }

private:
    result_t checkNode(XmlNode_base* newChild);

private:
    bool m_isXml;
    obj_ptr<XmlDocumentType_base> m_doctype;
    obj_ptr<XmlElement_base> m_element;
    exlib::string m_version;
    exlib::string m_encoding;
    int32_t m_standalone;
};

} /* namespace fibjs */
#endif /* XMLDOCUMENT_H_ */
