/*
 * XmlProcessingInstruction.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlProcessingInstruction.h"

namespace fibjs {

result_t XmlProcessingInstruction::get_nodeName(exlib::string& retVal)
{
    retVal = m_target;
    return 0;
}

result_t XmlProcessingInstruction::get_nodeValue(exlib::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t XmlProcessingInstruction::set_nodeValue(exlib::string newVal)
{
    m_data = newVal;
    return 0;
}

result_t XmlProcessingInstruction::get_textContent(exlib::string& retVal)
{
    return 0;
}

result_t XmlProcessingInstruction::set_textContent(exlib::string newVal)
{
    return 0;
}

result_t XmlProcessingInstruction::cloneNode(bool deep, obj_ptr<XmlNode_base>& retVal)
{
    obj_ptr<XmlProcessingInstruction> pi = new XmlProcessingInstruction(*this);
    return XmlNodeImpl::cloneNode(pi, deep, retVal);
}

result_t XmlProcessingInstruction::get_target(exlib::string& retVal)
{
    retVal = m_target;
    return 0;
}

result_t XmlProcessingInstruction::get_data(exlib::string& retVal)
{
    retVal = m_data;
    return 0;
}

result_t XmlProcessingInstruction::set_data(exlib::string newVal)
{
    m_data = newVal;
    return 0;
}

result_t XmlProcessingInstruction::toString(exlib::string& retVal)
{
    retVal = "<?";
    retVal.append(m_target);
    retVal.append(1, ' ');
    retVal.append(m_data);
    retVal.append("?>");

    return 0;
}
}
