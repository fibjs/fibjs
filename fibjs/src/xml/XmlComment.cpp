/*
 * XmlComment.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlComment.h"

namespace fibjs {

result_t XmlComment::get_nodeName(exlib::string& retVal)
{
    retVal = "#comment";
    return 0;
}

result_t XmlComment::get_nodeValue(exlib::string& retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlComment::set_nodeValue(exlib::string newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlComment::get_textContent(exlib::string& retVal)
{
    return 0;
}

result_t XmlComment::set_textContent(exlib::string newVal)
{
    return 0;
}

result_t XmlComment::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlComment> comment = new XmlComment(*this);
    return XmlNodeImpl::cloneNode(comment, deep, retVal);
}

result_t XmlComment::get_data(exlib::string& retVal)
{
    return m_data.get_data(retVal);
}

result_t XmlComment::set_data(exlib::string newVal)
{
    return m_data.set_data(newVal);
}

result_t XmlComment::get_length(int32_t& retVal)
{
    return m_data.get_length(retVal);
}

result_t XmlComment::substringData(int32_t offset, int32_t count, exlib::string& retVal)
{
    return m_data.substringData(offset, count, retVal);
}

result_t XmlComment::appendData(exlib::string arg)
{
    return m_data.appendData(arg);
}

result_t XmlComment::insertData(int32_t offset, exlib::string arg)
{
    return m_data.insertData(offset, arg);
}

result_t XmlComment::deleteData(int32_t offset, int32_t count)
{
    return m_data.deleteData(offset, count);
}

result_t XmlComment::replaceData(int32_t offset, int32_t count, exlib::string arg)
{
    return m_data.replaceData(offset, count, arg);
}

result_t XmlComment::toString(exlib::string& retVal)
{
    retVal = "<!--";
    retVal.append(m_data.data());
    retVal.append("-->");

    return 0;
}

} /* namespace fibjs */
