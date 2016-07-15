/*
 * XmlDataImpl.h
 *
 *  Created on: Sep 11, 2014
 *      Author: lion
 */

#include "utils.h"
#include <string.h>
#include "utf8.h"

#ifndef XMLDATAIMPL_H_
#define XMLDATAIMPL_H_

namespace fibjs
{

class XmlDataImpl
{
public:
    XmlDataImpl(const char *data) : m_data(data)
    {
    }

    XmlDataImpl(const exlib::string &data) : m_data(data)
    {}

    XmlDataImpl(const XmlDataImpl &data) : m_data(data.m_data)
    {}

public:
    result_t get_data(exlib::string &retVal)
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
        retVal = (int32_t)utf8to16String(m_data).length();
        return 0;
    }

    result_t substringData(int32_t offset, int32_t count, exlib::string &retVal)
    {
        exlib::wstring _data(utf8to16String(m_data));

        if (offset < 0 || offset > (int32_t)_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        if (offset + count > (int32_t)_data.length())
            count = (int32_t)_data.length() - offset;

        retVal = utf16to8String(_data.substr(offset, count));
        return 0;
    }

    result_t appendData(const char *arg)
    {
        m_data.append(arg);
        return 0;
    }

    result_t insertData(int32_t offset, const char *arg)
    {
        exlib::wstring _data(utf8to16String(m_data));

        if (offset < 0 || offset > (int32_t)_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        _data = _data.substr(0, offset) + utf8to16String(arg) + _data.substr(offset);
        m_data = utf16to8String(_data);
        return 0;
    }

    result_t deleteData(int32_t offset, int32_t count)
    {
        exlib::wstring _data(utf8to16String(m_data));

        if (offset < 0 || offset > (int32_t)_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        if (offset + count > (int32_t)_data.length())
            count = (int32_t)_data.length() - offset;

        _data = _data.substr(0, offset) + _data.substr(offset + count);
        m_data = utf16to8String(_data);
        return 0;
    }

    result_t replaceData(int32_t offset, int32_t count, const char *arg)
    {
        exlib::wstring _data(utf8to16String(m_data));

        if (offset < 0 || offset > (int32_t)_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        if (offset + count > (int32_t)_data.length())
            count = (int32_t)_data.length() - offset;

        _data = _data.substr(0, offset) + utf8to16String(arg) + _data.substr(offset + count);
        m_data = utf16to8String(_data);
        return 0;
    }

    result_t splitText(int32_t offset, exlib::string &retVal)
    {
        exlib::wstring _data(utf8to16String(m_data));

        if (offset < 0 || offset > (int32_t)_data.length())
            return CHECK_ERROR(CALL_E_BADINDEX);

        retVal = utf16to8String(_data.substr(offset));
        _data = _data.substr(0, offset);
        m_data = utf16to8String(_data);
        return 0;
    }

    exlib::string encodedText()
    {
        exlib::string str;
        int32_t sz = (int32_t)m_data.length();

        if (!sz)
            return exlib::string();

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

    exlib::string data()
    {
        return m_data;
    }

private:
    exlib::string m_data;
};

} /* namespace fibjs */
#endif /* XMLDATAIMPL_H_ */
