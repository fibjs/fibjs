/*
 * XmlProcessingInstruction.h
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/XmlProcessingInstruction.h"
#include "XmlNodeImpl.h"

#ifndef XMLPROCESSINTINSTRUCTION_H_
#define XMLPROCESSINTINSTRUCTION_H_

namespace fibjs
{

class XmlProcessingInstruction: public XmlProcessingInstruction_base, public XmlNodeImpl
{
public:
    XmlProcessingInstruction(XmlDocument_base *document, exlib::string target, exlib::string data):
        XmlNodeImpl(document, this, xml_base::_PROCESSING_INSTRUCTION_NODE),
        m_target(target), m_data(data)
    {
    }

    XmlProcessingInstruction(const XmlProcessingInstruction &from):
        XmlNodeImpl(from.m_document, this, xml_base::_PROCESSING_INSTRUCTION_NODE),
        m_target(from.m_target), m_data(from.m_data)
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
    // XmlProcessingInstruction_base
    virtual result_t get_target(exlib::string &retVal);
    virtual result_t get_data(exlib::string &retVal);
    virtual result_t set_data(exlib::string newVal);

private:
    exlib::string m_target;
    exlib::string m_data;
};

} /* namespace fibjs */
#endif /* XMLPROCESSINTINSTRUCTION_H_ */
