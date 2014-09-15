/*
 * XmlComment.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlComment.h"

namespace fibjs
{

result_t XmlComment::get_nodeName(std::string &retVal)
{
    retVal = "#comment";
    return 0;
}

result_t XmlComment::get_nodeValue(std::string &retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlComment::set_nodeValue(const char *newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlComment::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlComment::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlComment::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlComment::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlComment::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlComment::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlComment::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlComment::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlComment::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                                  obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlComment::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                                  obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlComment::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlComment::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlComment::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlComment::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::cloneNode(deep, retVal);
}

result_t XmlComment::normalize()
{
    return XmlNodeImpl::normalize();
}

result_t XmlComment::get_data(std::string &retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlComment::set_data(const char *newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlComment::get_length(int32_t &retVal)
{
    return m_data.get_length(retVal);
}

result_t XmlComment::substringData(int32_t offset, int32_t count, std::string &retVal)
{
    return m_data.substringData(offset, count, retVal);
}

result_t XmlComment::appendData(const char *arg)
{
    return m_data.appendData(arg);
}

result_t XmlComment::insertData(int32_t offset, const char *arg)
{
    return m_data.insertData(offset, arg);
}

result_t XmlComment::deleteData(int32_t offset, int32_t count)
{
    return m_data.deleteData(offset, count);
}

result_t XmlComment::replaceData(int32_t offset, int32_t count, const char *arg)
{
    return m_data.replaceData(offset, count, arg);
}

result_t XmlComment::toString(std::string &retVal)
{
    retVal = "<!--";
    retVal.append(m_data.m_data);
    retVal.append("-->");

    return 0;
}

}
