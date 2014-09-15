/*
 * XmlAttr.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlAttr.h"
#include "XmlNodeImpl.h"

#ifndef XMLATTR_H_
#define XMLATTR_H_

namespace fibjs
{

class XmlAttr: public XmlAttr_base, public XmlNodeImpl
{
public:
    XmlAttr(XmlDocument_base *document, const char *name):
        XmlNodeImpl(document, this, xml_base::_ATTRIBUTE_NODE), m_name(name)
    {
    }

    XmlAttr(XmlDocument_base *document, const char *name, const char *value):
        XmlNodeImpl(document, this, xml_base::_ATTRIBUTE_NODE),
        m_name(name), m_value(value)
    {
    }

    XmlAttr(const XmlAttr &from):
        XmlNodeImpl(from.m_document, this, xml_base::_ATTRIBUTE_NODE),
        m_name(from.m_name), m_value(from.m_value)
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
    // XmlAttr_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t get_specified(bool &retVal);
    virtual result_t get_value(std::string &retVal);
    virtual result_t set_value(const char *newVal);

private:
    std::string m_name;
    std::string m_value;
};

} /* namespace fibjs */
#endif /* XMLATTR_H_ */
