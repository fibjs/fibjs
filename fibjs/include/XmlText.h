/*
 * XmlText.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlText.h"
#include "XmlNodeImpl.h"
#include "XmlDataImpl.h"

#ifndef XMLTEXT_H_
#define XMLTEXT_H_

namespace fibjs
{

class XmlText: public XmlText_base, public XmlNodeImpl
{
public:
    XmlText(XmlDocument_base *document, const char *data):
        XmlNodeImpl(document, this, xml_base::_TEXT_NODE), m_data(data)
    {
    }

    XmlText(XmlDocument_base *document, std::string &data):
        XmlNodeImpl(document, this, xml_base::_TEXT_NODE), m_data(data)
    {
    }

    XmlText(const XmlText &from):
        XmlNodeImpl(from.m_document, this, xml_base::_TEXT_NODE),
        m_data(from.m_data)
    {
    }

public:
    // object_base
    virtual result_t toString(std::string &retVal);

public:
    // XmlNode_base
    virtual result_t get_nodeType(int32_t &retVal);
    virtual result_t get_nodeName(std::string &retVal);
    virtual result_t get_nodeValue(std::string &retVal);
    virtual result_t set_nodeValue(const char *newVal);
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
    virtual result_t lookupPrefix(const char *namespaceURI, std::string &retVal);
    virtual result_t lookupNamespaceURI(const char *prefix, std::string &retVal);
    virtual result_t insertBefore(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t insertAfter(XmlNode_base *newChild, XmlNode_base *refChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);
    virtual result_t removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal);

public:
    // XmlCharacterData_base
    virtual result_t get_data(std::string &retVal);
    virtual result_t set_data(const char *newVal);
    virtual result_t get_length(int32_t &retVal);
    virtual result_t substringData(int32_t offset, int32_t count, std::string &retVal);
    virtual result_t appendData(const char *arg);
    virtual result_t insertData(int32_t offset, const char *arg);
    virtual result_t deleteData(int32_t offset, int32_t count);
    virtual result_t replaceData(int32_t offset, int32_t count, const char *arg);

public:
    // XmlText_base
    virtual result_t splitText(int32_t offset, obj_ptr<XmlText_base> &retVal);

private:
    XmlDataImpl m_data;
};

} /* namespace fibjs */
#endif /* XMLTEXT_H_ */
