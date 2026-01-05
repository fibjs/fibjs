/*
 * XmlText.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlText.h"

namespace fibjs {

result_t XmlText::get_nodeName(exlib::string& retVal)
{
    retVal = "#text";
    return 0;
}

result_t XmlText::get_nodeValue(exlib::string& retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlText::set_nodeValue(exlib::string newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlText::get_textContent(exlib::string& retVal)
{
    get_nodeValue(retVal);
    return 0;
}

result_t XmlText::set_textContent(exlib::string newVal)
{
    set_nodeValue(newVal);
    return 0;
}

result_t XmlText::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlText> txt = new XmlText(*this);
    return XmlNodeImpl::cloneNode(txt, deep, retVal);
}

result_t XmlText::get_data(exlib::string& retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlText::set_data(exlib::string newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlText::get_length(int32_t& retVal)
{
    return m_data.get_length(retVal);
}

result_t XmlText::substringData(int32_t offset, int32_t count, exlib::string& retVal)
{
    return m_data.substringData(offset, count, retVal);
}

result_t XmlText::appendData(exlib::string arg)
{
    return m_data.appendData(arg);
}

result_t XmlText::insertData(int32_t offset, exlib::string arg)
{
    return m_data.insertData(offset, arg);
}

result_t XmlText::deleteData(int32_t offset, int32_t count)
{
    return m_data.deleteData(offset, count);
}

result_t XmlText::replaceData(int32_t offset, int32_t count, exlib::string arg)
{
    return m_data.replaceData(offset, count, arg);
}

result_t XmlText::splitText(int32_t offset, obj_ptr<XmlText_base>& retVal)
{
    exlib::string ret;
    result_t hr;

    hr = m_data.splitText(offset, ret);
    if (hr < 0)
        return hr;

    retVal = new XmlText(m_document, ret);
    if (m_parent) {
        obj_ptr<XmlNode_base> out;
        return m_parent->m_childs->insertAfter(retVal, this, out);
    }

    return 0;
}

result_t XmlText::toString(exlib::string& retVal)
{
    retVal = m_data.encodedText();
    return 0;
}

} /* namespace fibjs */
