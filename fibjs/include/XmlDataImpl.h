/*
 * XmlDataImpl.h
 *
 *  Created on: Sep 11, 2014
 *      Author: lion
 */

#include "utils.h"
#include <string.h>

#ifndef XMLDATAIMPL_H_
#define XMLDATAIMPL_H_

namespace fibjs
{

class XmlDataImpl
{
public:
    XmlDataImpl(const char *data) : m_data(data)
    {}

    XmlDataImpl(std::string &data) : m_data(data)
    {}

public:
    result_t get_data(std::string &retVal)
    {
        retVal = m_data;
        return 0;
    }

    result_t set_data(const char *newVal)
    {
        m_data = newVal;
        return 0;
    }

    result_t get_length(int32_t &retVal)
    {
        retVal = (int32_t)m_data.length();
        return 0;
    }

    result_t substringData(int32_t offset, int32_t count, std::string &retVal)
    {
        if (offset < 0 || offset > (int32_t)m_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);
        retVal = m_data.substr(offset, count);
        return 0;
    }

    result_t appendData(const char *arg)
    {
        m_data.append(arg);
        return 0;
    }

    result_t insertData(int32_t offset, const char *arg)
    {
        if (offset < 0 || offset > (int32_t)m_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        m_data = m_data.substr(0, offset) + arg + m_data.substr(offset);
        return 0;
    }

    result_t deleteData(int32_t offset, int32_t count)
    {
        if (offset < 0 || offset > (int32_t)m_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        if (offset + count > (int32_t)m_data.length())
            count = (int32_t)m_data.length() - offset;

        m_data = m_data.substr(0, offset) + m_data.substr(offset + count);
        return 0;
    }

    result_t replaceData(int32_t offset, int32_t count, const char *arg)
    {
        if (offset < 0 || offset > (int32_t)m_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        if (offset + count > (int32_t)m_data.length())
            count = (int32_t)m_data.length() - offset;

        m_data = m_data.substr(0, offset) + arg + m_data.substr(offset + count);
        return 0;
    }

    result_t splitText(int32_t offset, std::string &retVal)
    {
        if (offset < 0 || offset > (int32_t)m_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        retVal = m_data.substr(offset);
        m_data = m_data.substr(0, offset);
        return 0;
    }

    std::string encodedText()
    {
        std::string str;
        int32_t sz = (int32_t)m_data.length();

        if (!sz)
            return str;

        const char *data = m_data.c_str();
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
            default:
                sz1 ++;
            }
        }

        if (sz == sz1)
            return m_data;

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
            default:
                *data1++ = ch;
            }
        }

        return str;
    }

public:
    std::string m_data;
};

} /* namespace fibjs */
#endif /* XMLDATAIMPL_H_ */
