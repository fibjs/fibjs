/*
 * XmlElement.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlElement.h"

namespace fibjs
{

result_t XmlElement::get_nodeName(std::string &retVal)
{
    retVal = m_tagName;
    return 0;
}

result_t XmlElement::get_nodeValue(std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::set_nodeValue(const char *newVal)
{
    return 0;
}

result_t XmlElement::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlElement::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlElement::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlElement::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlElement::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlElement::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlElement::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlElement::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlElement::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                                  obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlElement::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                                  obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlElement::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlElement::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlElement::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlElement::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::cloneNode(deep, retVal);
}

result_t XmlElement::normalize()
{
    return XmlNodeImpl::normalize();
}

result_t XmlElement::get_tagName(std::string &retVal)
{
    retVal = m_tagName;
    return 0;
}

result_t XmlElement::get_attributes(obj_ptr<XmlNamedNodeMap_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::getAttribute(const char *name, std::string &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::setAttribute(const char *name, const char *value)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::removeAttribute(const char *name)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::getAttributeNode(const char *name, obj_ptr<XmlAttr_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::setAttributeNode(XmlAttr_base *newAttr, obj_ptr<XmlAttr_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::removeAttributeNode(XmlAttr_base *oldAttr, obj_ptr<XmlAttr_base> &retVal)
{
    return CALL_RETURN_NULL;
}

result_t XmlElement::getElementsByTagName(const char *tagName, obj_ptr<XmlNodeList_base> &retVal)
{
    obj_ptr<XmlNodeList> ret = new XmlNodeList(NULL);
    getElementsByTagName(tagName, ret);

    retVal = ret;
    return 0;
}

result_t XmlElement::hasAttribute(bool &retVal)
{
    return CALL_RETURN_NULL;
}

}
