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
    XmlDocumentType(): XmlNodeImpl(NULL, this, xml_base::_DOCUMENT_TYPE_NODE)
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
    // XmlDocumentType_base
    virtual result_t get_name(std::string &retVal);
    virtual result_t get_systemId(std::string &retVal);

};

} /* namespace fibjs */
#endif /* XMLDOCUMENTTYPE_H_ */
