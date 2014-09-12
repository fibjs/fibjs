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
    XmlProcessingInstruction(XmlDocument_base *document, const char *target, const char *data):
        XmlNodeImpl(document, this, xml_base::_PROCESSING_INSTRUCTION_NODE),
        m_target(target), m_data(data)
    {
    }

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
    // XmlProcessingInstruction_base
    virtual result_t get_target(std::string &retVal);
    virtual result_t get_data(std::string &retVal);
    virtual result_t set_data(const char *newVal);

private:
    std::string m_target;
    std::string m_data;
};

} /* namespace fibjs */
#endif /* XMLPROCESSINTINSTRUCTION_H_ */
