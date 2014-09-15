/*
 * XmlAttr.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "ifs/xml.h"
#include "XmlAttr.h"
#include <string.h>

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
    return CALL_RETURN_NULL;
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
    return CALL_RETURN_NULL;
}

result_t XmlAttr::get_nextSibling(obj_ptr<XmlNode_base> &retVal)
{
    return CALL_RETURN_NULL;
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

result_t XmlAttr::toString(std::string &retVal)
{
    retVal = " ";
    retVal.append(m_name);
    retVal.append("=\"");

    std::string str;
    int32_t sz = m_value.length();

    if (sz)
    {
        const char *data = m_value.c_str();
        int32_t sz1 = 0;
        int32_t i;

        for (i = 0; i < sz; i ++)
        {
            switch (data[i])
            {
            case '<':
                sz1 += 4;
                break;
            case '>':
                sz1 += 4;
                break;
            case '&':
                sz1 += 5;
                break;
            case '\"':
                sz1 += 6;
                break;
            default:
                sz1 ++;
            }
        }

        if (sz == sz1)
            str = m_value;
        else
        {
            str.resize(sz1);
            char *data1 = &str[0];

            for (i = 0; i < sz; i ++)
            {
                char ch;
                switch (ch = data[i])
                {
                case '<':
                    memcpy(data1, "&lt;", 4);
                    data1 += 4;
                    break;
                case '>':
                    memcpy(data1, "&gt;", 4);
                    data1 += 4;
                    break;
                case '&':
                    memcpy(data1, "&amp;", 5);
                    data1 += 5;
                    break;
                case '\"':
                    memcpy(data1, "&quot;", 6);
                    data1 += 6;
                    break;
                default:
                    *data1++ = ch;
                }
            }
        }
    }

    retVal.append(str);
    retVal += '\"';

    return 0;
}

}
