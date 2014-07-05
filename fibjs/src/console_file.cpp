/*
 * console_file.cpp
 *
 *  Created on: Jul 4, 2014
 *      Author: lion
 */

#include "console.h"
#include "File.h"
#include "Buffer.h"

namespace fibjs
{

result_t file_logger::config(v8::Local<v8::Object> o)
{
    result_t hr = logger::config(o);
    if (hr < 0)
        return hr;

    hr = GetConfigValue(o, "path", m_path);
    if (hr < 0)
        return hr;

    m_split_size = 0;
    m_split_mode = 0;

    std::string split;
    hr = GetConfigValue(o, "split", split);
    if (hr >= 0)
    {
        if (!qstrcmp(split.c_str(), "day"))
            m_split_mode =  24 * 60 * 60 * 1000;
        else if (!qstrcmp(split.c_str(), "hour"))
            m_split_mode =  60 * 60 * 1000;
        else if (!qstrcmp(split.c_str(), "minute"))
            m_split_mode =  60 * 1000;
        else
        {
            int32_t l = (int32_t)split.length();
            int32_t i;

            if (l > 4 || l < 2)
                return Runtime::setError("Unknown split mode.");

            for (i = 0; i < l - 1; i ++)
                if (!qisdigit(split[i]))
                    return Runtime::setError("Unknown split mode.");
                else
                    m_split_size = m_split_size * 10 + split[i] - '0';

            if (split[i] == 'k')
                m_split_size <<= 10;
            else if (split[i] == 'm')
                m_split_size <<= 20;
            else if (split[i] == 'g')
                m_split_size <<= 30;
            else
                return Runtime::setError("Unknown split mode.");
        }
    }
    else if (hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    hr = GetConfigValue(o, "count", m_count);
    if (hr == CALL_E_PARAMNOTOPTIONAL)
        m_count = INT_MAX;
    else if (hr < 0)
        return hr;
    else
    {
        if (m_split_size == 0 && m_split_mode == 0)
            return Runtime::setError("Missing split mode.");

        if (m_count < 2 || m_count > 128)
            return Runtime::setError("Count must between 2 to 128.");
    }

    return 0;
}

result_t file_logger::initFile()
{
    if (m_split_mode && m_file)
    {
        date_t d;

        d.now();
        if (d.diff(m_date) >= m_split_mode)
            m_file.Release();
    }

    result_t hr;
    exlib::AsyncEvent ac;

    if (!m_file)
    {
        obj_ptr<File> f = new File();
        std::string name(m_path);

        if (m_count > 1)
        {
            std::string tm;

            m_date.now();
            m_date.stamp(tm);
            name.append(tm);
        }

        hr = f->open(name.c_str(), "a+", &ac);
        if (hr < 0)
            return hr;

        hr = f->tell(m_size);
        if (hr < 0)
            return hr;

        m_file = f;
    }

    return 0;
}

void file_logger::write(item *pn)
{
    item *p1;
    exlib::AsyncEvent ac;

    while (pn)
    {
        std::string outBuffer;
        result_t hr;

        hr = initFile();
        if (hr < 0)
        {
            while (pn)
            {
                p1 = pn;
                pn = (logger::item *) p1->m_next;
                delete p1;
            }

            break;
        }

        while (pn)
        {
            p1 = pn;

            outBuffer.append(p1->full());

#ifdef _WIN32
            outBuffer.append("\r\n", 2);
#else
            outBuffer.append("\n", 1);
#endif

            pn = (logger::item *) p1->m_next;
            delete p1;

            if (m_split_size && m_size + outBuffer.length() >= m_split_size)
                break;
        }

        if (m_file)
        {
            obj_ptr<Buffer_base> buf = new Buffer(outBuffer);

            hr = m_file->write(buf, &ac);
            if (hr < 0)
                m_file.Release();

            m_size += outBuffer.length();
            if (m_size >= m_split_size)
                m_file.Release();
        }
    }

    Sleep(200);
}

}
