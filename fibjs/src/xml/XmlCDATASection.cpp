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

result_t XmlCDATASection::get_textContent(exlib::string& retVal)
{
    return 0;
}

result_t XmlCDATASection::set_textContent(exlib::string newVal)
{
    return 0;
}

result_t XmlCDATASection::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlCDATASection> cds = new XmlCDATASection(*this);
    return XmlNodeImpl::cloneNode(cds, deep, retVal);
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

} /* namespace fibjs */
