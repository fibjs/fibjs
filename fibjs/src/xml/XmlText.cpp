/*
 * XmlText.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlText.h"

namespace fibjs
{

result_t XmlText::get_nodeName(qstring &retVal)
{
    retVal = "#text";
    return 0;
}

result_t XmlText::get_nodeValue(qstring &retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlText::set_nodeValue(const char *newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlText::get_nodeType(int32_t &retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlText::get_parentNode(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlText::get_childNodes(obj_ptr<XmlNodeList_base> &retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlText::get_firstChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlText::get_lastChild(obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlText::get_previousSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlText::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlText::get_ownerDocument(obj_ptr<XmlDocument_base> &retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlText::lookupPrefix(const char *namespaceURI, qstring &retVal)
{
    return XmlNodeImpl::lookupPrefix(namespaceURI, retVal);
}

result_t XmlText::lookupNamespaceURI(const char *prefix, qstring &retVal)
{
    return XmlNodeImpl::lookupNamespaceURI(prefix, retVal);
}

result_t XmlText::insertBefore(XmlNode_base *newChild, XmlNode_base *refChild,
                               obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlText::insertAfter(XmlNode_base *newChild, XmlNode_base *refChild,
                              obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlText::replaceChild(XmlNode_base *newChild, XmlNode_base *oldChild,
                               obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlText::removeChild(XmlNode_base *oldChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlText::appendChild(XmlNode_base *newChild, obj_ptr<XmlNode_base> &retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlText::hasChildNodes(bool &retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlText::cloneNode(bool deep, obj_ptr<XmlNode_base> &retVal)
{
    obj_ptr<XmlText> txt = new XmlText(*this);
    return XmlNodeImpl::cloneNode(txt, deep, retVal);
}

result_t XmlText::normalize()
{
    return m_childs->normalize();
}

result_t XmlText::get_data(qstring &retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlText::set_data(const char *newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlText::get_length(int32_t &retVal)
{
    return m_data.get_length(retVal);
}

result_t XmlText::substringData(int32_t offset, int32_t count, qstring &retVal)
{
    return m_data.substringData(offset, count, retVal);
}

result_t XmlText::appendData(const char *arg)
{
    return m_data.appendData(arg);
}

result_t XmlText::insertData(int32_t offset, const char *arg)
{
    return m_data.insertData(offset, arg);
}

result_t XmlText::deleteData(int32_t offset, int32_t count)
{
    return m_data.deleteData(offset, count);
}

result_t XmlText::replaceData(int32_t offset, int32_t count, const char *arg)
{
    return m_data.replaceData(offset, count, arg);
}

result_t XmlText::splitText(int32_t offset, obj_ptr<XmlText_base> &retVal)
{
    qstring ret;
    result_t hr;

    hr = m_data.splitText(offset, ret);
    if (hr < 0)
        return hr;

    retVal = new XmlText(m_document, ret);
    if (m_parent)
    {
        obj_ptr<XmlNode_base> out;
        return m_parent->m_childs->insertAfter(retVal, this, out);
    }

    return 0;
}

result_t XmlText::toString(qstring &retVal)
{
    retVal = m_data.encodedText();
    return 0;
}

}
