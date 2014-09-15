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

namespace fibjs
{

class XmlDocument: public XmlDocument_base, public XmlNodeImpl
{
public:
    XmlDocument(): XmlNodeImpl(NULL, this, xml_base::_DOCUMENT_NODE), m_standalone(-1)
    {
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeName(std::string &retVal);
    virtual result_t get_nodeValue(std::string &retVal);
    virtual result_t set_nodeValue(const char *newVal);
    virtual result_t get_nodeType(int32_t &retVal);
    virtual result_t get_parentNode(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_childNodes(obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t get_firstChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_lastChild(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_previousSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_nextSibling(obj_ptr<XmlNode_base> &retVal);
    virtual result_t get_ownerDocument(obj_ptr<XmlDocument_base> &retVal);
    virtual result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t hasChildNodes(bool &retVal);
    virtual result_t cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal);
    virtual result_t normalize();

public:
    // XmlDocument_base
    virtual result_t loadXML(const char *source);
    virtual result_t saveHTML(std::string &retVal);
    virtual result_t get_doctype(obj_ptr<XmlDocumentType_base> &retVal);
    virtual result_t get_documentElement(obj_ptr<XmlElement_base> &retVal);
    virtual result_t createElement(const char *tagName, obj_ptr<XmlElement_base> &retVal);
    virtual result_t createTextNode(const char *data, obj_ptr<XmlText_base> &retVal);
    virtual result_t createComment(const char *data, obj_ptr<XmlComment_base> &retVal);
    virtual result_t createCDATASection(const char *data, obj_ptr<XmlCDATASection_base> &retVal);
    virtual result_t createProcessingInstruction(const char *target, const char *data, obj_ptr<XmlProcessingInstruction_base> &retVal);
    virtual result_t createAttribute(const char *name, obj_ptr<XmlAttr_base> &retVal);
    virtual result_t getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList_base> &retVal);
    virtual result_t getElementById(const char *elementId, obj_ptr<XmlElement_base> &retVal);
    virtual result_t get_inputEncoding(std::string &retVal);
    virtual result_t get_xmlStandalone(bool &retVal);
    virtual result_t set_xmlStandalone(bool newVal);
    virtual result_t get_xmlVersion(std::string &retVal);
    virtual result_t set_xmlVersion(const char *newVal);

public:
    void setDecl(const char *version, const char *encoding, int32_t standalone)
    {
        m_version = version ? version : "";
        m_encoding = encoding ? encoding : "";
        m_standalone = standalone;
    }

private:
    result_t checkElement(XmlNode_base *newChild);

private:
    obj_ptr<XmlElement_base> m_Element;
    std::string m_version;
    std::string m_encoding;
    int32_t m_standalone;
};

} /* namespace fibjs */
#endif /* XMLDOCUMENT_H_ */
