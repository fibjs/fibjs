/*
 * XmlAttr.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlAttr.h"

namespace fibjs
{

result_t XmlAttr::get_nodeName(std::string &retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlAttr::get_nodeValue(std::string &retVal)
{
    retVal = m_value;
    return 0;
}

result_t XmlAttr::set_nodeValue(const char *newVal)
{
    m_value = newVal;
    return 0;
}

result_t XmlAttr::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlAttr::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlAttr::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlAttr::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlAttr::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlAttr::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlAttr::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlAttr::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlAttr::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                               obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlAttr::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                               obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlAttr::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlAttr::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlAttr::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlAttr::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::cloneNode(deep, retVal);
}

result_t XmlAttr::normalize()
{
    return XmlNodeImpl::normalize();
}

result_t XmlAttr::get_name(std::string &retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlAttr::get_specified(bool &retVal)
{
    retVal = true;
    return 0;
}

result_t XmlAttr::get_value(std::string &retVal)
{
    retVal = m_value;
    return 0;
}

result_t XmlAttr::set_value(const char *newVal)
{
    m_value = newVal;
    return 0;
}

}
