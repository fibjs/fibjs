/*
 * StringBuffer.h
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#pragma once

#include "QuickArray.h"
#include <string>
#include <string.h>

namespace fibjs {

class StringBuffer {
public:
    StringBuffer()
        : m_size(0)
    {
    }

public:
    exlib::string str()
    {
        int32_t i, p = 0;
        exlib::string s;

        if (m_size > 0) {
            if (m_array.size() == 1)
                s = m_array[0];
            else {
                s.resize(m_size);
                char* _s = s.data();

                for (i = 0; i < (int32_t)m_array.size(); i++) {
                    exlib::string& s1 = m_array[i];
                    size_t len = s1.length();

                    memcpy(&_s[p], s1.c_str(), len);
                    p += (int32_t)len;
                }
            }

            m_size = 0;
            m_array.resize(0);
        }

        return s;
    }

    size_t size()
    {
        return m_size;
    }

    void append(exlib::string s)
    {
        if (s.length() > 0) {
            m_size += s.length();
            m_array.append(s);
        }
    }

    void append(const char* s, int32_t sz = -1)
    {
        if (sz < 0)
            sz = (int32_t)qstrlen(s);

        if (sz > 0) {
            m_size += sz;
            m_array.append(exlib::string(s, sz));
        }
    }

    void append(char ch)
    {
        if (m_size == 0)
            m_array.resize(1);

        m_array[m_array.size() - 1] += ch;
        m_size++;
    }

private:
    QuickArray<exlib::string> m_array;
    size_t m_size;
};
}
