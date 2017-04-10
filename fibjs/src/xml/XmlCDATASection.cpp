/*
 * XmlCDATASection.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlCDATASection.h"

namespace fibjs {

result_t XmlCDATASection::get_nodeName(exlib::string& retVal)
{
    retVal = "#cdata-section";
    return 0;
}

result_t XmlCDATASection::get_nodeValue(exlib::string& retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlCDATASection::set_nodeValue(exlib::string newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlCDATASection::get_nodeType(int32_t& retVal)
{
    return XmlNodeImpl::get_nodeType(retVal);
}

result_t XmlCDATASection::get_parentNode(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_parentNode(retVal);
}

result_t XmlCDATASection::get_childNodes(obj_ptr<XmlNodeList_base>& retVal)
{
    return XmlNodeImpl::get_childNodes(retVal);
}

result_t XmlCDATASection::get_firstChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->firstChild(retVal);
}

result_t XmlCDATASection::get_lastChild(obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->lastChild(retVal);
}

result_t XmlCDATASection::get_previousSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_previousSibling(retVal);
}

result_t XmlCDATASection::get_nextSibling(obj_ptr<XmlNode_base>& retVal)
{
    return XmlNodeImpl::get_nextSibling(retVal);
}

result_t XmlCDATASection::get_ownerDocument(obj_ptr<XmlDocument_base>& retVal)
{
    return XmlNodeImpl::get_ownerDocument(retVal);
}

result_t XmlCDATASection::lookupPrefix(exlib::string namespaceURI, exlib::string& retVal)
{
    return XmlNodeImpl::lookupPrefix(namespaceURI, retVal);
}

result_t XmlCDATASection::lookupNamespaceURI(exlib::string prefix, exlib::string& retVal)
{
    return XmlNodeImpl::lookupNamespaceURI(prefix, retVal);
}

result_t XmlCDATASection::insertBefore(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertBefore(newChild, refChild, retVal);
}

result_t XmlCDATASection::insertAfter(XmlNode_base* newChild, XmlNode_base* refChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->insertAfter(newChild, refChild, retVal);
}

result_t XmlCDATASection::replaceChild(XmlNode_base* newChild, XmlNode_base* oldChild,
    obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->replaceChild(newChild, oldChild, retVal);
}

result_t XmlCDATASection::removeChild(XmlNode_base* oldChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->removeChild(oldChild, retVal);
}

result_t XmlCDATASection::appendChild(XmlNode_base* newChild, obj_ptr<XmlNode_base>& retVal)
{
    return m_childs->appendChild(newChild, retVal);
}

result_t XmlCDATASection::hasChildNodes(bool& retVal)
{
    return m_childs->hasChildNodes(retVal);
}

result_t XmlCDATASection::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlCDATASection> cds = new XmlCDATASection(*this);
    return XmlNodeImpl::cloneNode(cds, deep, retVal);
}

result_t XmlCDATASection::normalize()
{
    return m_childs->normalize();
}

result_t XmlCDATASection::get_data(exlib::string& retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlCDATASection::set_data(exlib::string newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlCDATASection::get_length(int32_t& retVal)
{
    return m_data.get_length(retVal);
}

result_t XmlCDATASection::substringData(int32_t offset, int32_t count, exlib::string& retVal)
{
    return m_data.substringData(offset, count, retVal);
}

result_t XmlCDATASection::appendData(exlib::string arg)
{
    return m_data.appendData(arg);
}

result_t XmlCDATASection::insertData(int32_t offset, exlib::string arg)
{
    return m_data.insertData(offset, arg);
}

result_t XmlCDATASection::deleteData(int32_t offset, int32_t count)
{
    return m_data.deleteData(offset, count);
}

result_t XmlCDATASection::replaceData(int32_t offset, int32_t count, exlib::string arg)
{
    return m_data.replaceData(offset, count, arg);
}

result_t XmlCDATASection::splitText(int32_t offset, obj_ptr<XmlText_base>& retVal)
{
    exlib::string ret;
    result_t hr;

    hr = m_data.splitText(offset, ret);
    if (hr < 0)
        return hr;

    retVal = new XmlCDATASection(m_document, ret);
    if (m_parent) {
        obj_ptr<XmlNode_base> out;
        return m_parent->m_childs->insertAfter(retVal, this, out);
    }

    return 0;
}

result_t XmlCDATASection::toString(exlib::string& retVal)
{
    retVal = "<![CDATA[";
    retVal.append(m_data.data());
    retVal.append("]]>");

    return 0;
}
}
