/*
 * XmlDataImpl.h
 *
 *  Created on: Sep 11, 2014
 *      Author: lion
 */

#include "utils.h"

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

public:
    std::string m_data;
};

} /* namespace fibjs */
#endif /* XMLDATAIMPL_H_ */
