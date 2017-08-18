/*
 * XmlAttr.cpp
 *
 *  Created on: Sep 9, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/xml.h"
#include "XmlAttr.h"
#include "XmlElement.h"
#include <string.h>

namespace fibjs {

result_t XmlAttr::get_namespaceURI(exlib::string& retVal)
{
    if (m_namespaceURI.empty()) {
        if (m_name == "xmlns") {
            retVal = "http://www.w3.org/2000/xmlns/";
            return 0;
        }

        return CALL_RETURN_UNDEFINED;
    }

    retVal = m_namespaceURI;
    return 0;
}

result_t XmlAttr::get_prefix(exlib::string& retVal)
{
    if (m_prefix.empty())
        return CALL_RETURN_UNDEFINED;

    retVal = m_prefix;
    return 0;
}

result_t XmlAttr::set_prefix(exlib::string newVal)
{
    m_prefix = newVal;
    return 0;
}

result_t XmlAttr::get_localName(exlib::string& retVal)
{
    retVal = m_localName;
    return 0;
}

result_t XmlAttr::get_name(exlib::string& retVal)
{
    retVal = m_name;
    return 0;
}

result_t XmlAttr::get_value(exlib::string& retVal)
{
    retVal = m_value;
    return 0;
}

result_t XmlAttr::set_value(exlib::string newVal)
{
    m_value = newVal;
    return 0;
}

void XmlAttr::fix_prefix()
{
    if (!m_namespaceURI.empty() && m_owner)
        m_owner->fix_prefix(m_namespaceURI, m_prefix);
}

result_t XmlAttr::get_nodeName(exlib::string& retVal)
{
    return get_name(retVal);
}

result_t XmlAttr::get_nodeValue(exlib::string& retVal)
{
    return get_value(retVal);
}

result_t XmlAttr::set_nodeValue(exlib::string newVal)
{
    return set_value(newVal);
}

result_t XmlAttr::toString(exlib::string& retVal)
{
    retVal = " ";

    if (m_prefix.empty())
        retVal.append(m_name);
    else {
        retVal.append(m_prefix);
        retVal += ':';
        retVal.append(m_localName);
    }

    retVal.append("=\"");

    exlib::string str;
    int32_t sz = (int32_t)m_value.length();

    if (sz) {
        const char* data = m_value.c_str();
        int32_t sz1 = 0;
        int32_t i;

        for (i = 0; i < sz; i++) {
            switch (data[i]) {
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
                sz1++;
            }
        }

        if (sz == sz1)
            str = m_value;
        else {
            str.resize(sz1);
            char* data1 = &str[0];

            for (i = 0; i < sz; i++) {
                char ch;
                switch (ch = data[i]) {
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
