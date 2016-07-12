/*
 * qstring.h
 *
 *  Created on: Jul 26, 2012
 *      Author: lion
 */

#include <string>
#include <stdint.h>
#include <string.h>

#ifndef QSTRING_H_
#define QSTRING_H_

namespace fibjs
{

template<typename T>
inline bool qisspace(T ch)
{
    return ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t';
}

template<typename T>
inline bool qisascii(T ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

template<typename T>
inline bool qisdigit(T ch)
{
    return ch >= '0' && ch <= '9';
}

template<typename T>
inline bool qisxdigit(T ch)
{
    return qisdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'Z');
}

template<typename T>
inline int32_t qhex(T ch)
{
    return (ch >= '0' && ch <= '9' ? ch - '0' :
            ch >= 'a' && ch <= 'f' ? ch - 'a' + 10 :
            ch >= 'A' && ch <= 'F' ? ch - 'A' + 10 : 0);
}

template<typename T>
inline bool qisupper(T ch)
{
    return ch >= 'A' && ch <= 'Z';
}

template<typename T>
inline bool qislower(T ch)
{
    return ch >= 'a' && ch <= 'z';
}

template<typename T>
inline T qtolower(T c)
{
    if (qisupper(c))
        return c + ('a' - 'A');
    return c;
}

template<typename T>
inline T qtoupper(T c)
{
    if (qislower(c))
        return c + ('A' - 'a');
    return c;
}

template<typename T>
inline int32_t qchricmp(T ch1, T ch2)
{
    return qtolower(ch1) - qtolower(ch2);
}

template<typename T>
inline int32_t qstricmp(const T* s1, const T* s2, int32_t sz = -1)
{
    if (s1 == s2)
        return 0;

    int32_t n = 0;

    while (*s1 && !(n = qchricmp(*s1++, *s2++)))
        if ((sz > 0) && (!--sz))
            return 0;

    return n ? n : -*s2;
}

template<typename T>
inline int32_t qstrcmp(const T* s1, const T* s2, int32_t sz = -1)
{
    if (s1 == s2)
        return 0;

    int32_t n = 0;

    while (*s1 && !(n = *s1++ - *s2++))
        if ((sz > 0) && (!--sz))
            return 0;

    return n ? n : -*s2;
}

template<typename T>
inline const T* qstrichr(const T* s, T c)
{
    do
    {
        if (!qchricmp(*s, c))
            return s;
    }
    while (*s++);
    return (0);
}

template<typename T>
inline const T* qstrchr(const T* s, T c)
{
    do
    {
        if (*s == c)
            return s;
    }
    while (*s++);

    return (0);
}

template<typename T>
inline const T* qstrrchr(const T* s, T c)
{
    const T* s1 = NULL;
    do
    {
        if (*s == c)
            s1 = s;
    }
    while (*s++);

    return s1;
}

template<typename T>
inline size_t qstrlen(const T* pStr)
{
    const T* pEnd;

    for (pEnd = pStr; *pEnd != 0; pEnd++)
        continue;

    return pEnd - pStr;
}

template<typename T>
inline const T* qstristr(const T* in, const T* str)
{
    T c;

    c = *str;
    if (!c)
        return NULL;

    int32_t len = (int32_t) qstrlen(str);

    while ((in = qstrichr(in, c)) && qstricmp(in, str, len))
        in++;
    return in;
}

template<typename T>
inline const T* qstrstr(const T* in, const T* str)
{
    T c;

    c = *str;
    if (!c)
        return NULL;

    int32_t len = (int32_t) qstrlen(str);

    while ((in = qstrchr(in, c)) && qstrcmp(in, str, len))
        in++;
    return in;
}

template<typename T>
inline void qstrlwr(T* s)
{
    T c;

    while ((c = *s) != 0)
        * s++ = qtolower(c);
}

template<typename T>
inline void qstrupr(T* s)
{
    T c;

    while ((c = *s) != 0)
        * s++ = qtoupper(c);
}

template<typename T>
inline void qmemset(T* ptr, int value, size_t num)
{
    while (num--)
        *ptr++ = value;
}

template<typename T>
inline void qmemcpy(T* dest, const T* src, size_t count)
{
    if (dest != src)
        while (count--)
            *dest++ = *src++;
}

template<typename T>
inline int32_t qmemcmp(const T* s1, const T* s2, size_t count)
{
    if (s1 == s2)
        return 0;

    int32_t n;

    while (count--)
    {
        n = *s1++ - *s2++;
        if (n != 0)
            return n;
    }

    return 0;
}

template<typename T>
inline const T* qmemfind(const T* s1, size_t sz1, const T* s2, size_t sz2)
{
    while (sz1 >= sz2)
    {
        if (qmemcmp(s1, s2, sz2) == 0)
            return s1;
        s1 ++;
        sz1 --;
    }

    return NULL;
}

inline void qmemset(char* ptr, int value, size_t num)
{
    memset(ptr, value, num);
}

inline void qmemcpy(char* dest, const char* src, size_t count)
{
    if (dest != src)
        memcpy(dest, src, count);
}

inline int32_t qmemcmp(const char* s1, const char* s2, size_t count)
{
    if (s1 == s2)
        return 0;

    return memcmp(s1, s2, count);
}

#ifdef _WIN32
typedef wchar_t wchar;
#else
typedef uint16_t wchar;
#endif

template<typename T>
class basic_string
{
public:
    basic_string()
    {}

    basic_string(size_t n, T ch)
    {
        append(n, ch);
    }

    basic_string(const T* str)
    {
        append(str);
    }

    basic_string(const T* str, size_t sz)
    {
        append(str, sz);
    }

    basic_string(const std::basic_string<T>& v)
    {
        append(v);
    }

    basic_string(const basic_string<T>& v)
    {
        append(v);
    }

public:
    static const size_t npos = std::string::npos;

public:
    const T* c_str() const
    {
        return m_str.c_str();
    }

    T* c_buffer()
    {
        return &m_str[0];
    }

    size_t length() const
    {
        return m_str.length();
    }

    bool empty() const
    {
        return m_str.empty();
    }

    void resize(size_t sz)
    {
        m_str.resize(sz);
    }

    void clear()
    {
        return resize(0);
    }

    int32_t compare(const basic_string<T>& str) const
    {
        const T* s1 = c_str();
        size_t sz1 = length();
        const T* s2 = str.c_str();
        size_t sz2 = str.length();

        if (s1 == s2)
            return 0;

        size_t sz = sz1 > sz2 ? sz2 : sz1;
        int32_t r = qmemcmp(s1, s2, sz);
        if (r != 0 || sz1 == sz2)
            return r;

        return sz1 > sz2 ? 1 : -1;
    }

public:
    basic_string<T>& append(size_t n, T ch)
    {
        if (n > 0)
        {
            size_t len = length();
            resize(len + n);
            qmemset(c_buffer() + len, ch, n);
        }
        return *this;
    }

    basic_string<T>& append(const T* str, size_t sz)
    {
        if (sz > 0)
        {
            size_t len = length();
            const T* data = c_str();

            if (str >= data && str < data + len)
            {
                basic_string<T> temp(data, len);
                temp.append(str, sz);
                *this = temp;
            } else
            {
                resize(len + sz);
                qmemcpy(c_buffer() + len, str, sz);
            }
        }
        return *this;
    }

    basic_string<T>& append(const T* str)
    {
        append(str, qstrlen(str));
        return *this;
    }

    basic_string<T>& append(const std::basic_string<T>& str)
    {
        append(str.c_str(), str.length());
        return *this;
    }

    basic_string<T>& append(const basic_string<T>& str)
    {
        append(str.m_str);
        return *this;
    }

public:
    basic_string<T>& assign(size_t n, T ch)
    {
        clear();
        return append(n, ch);
    }

    basic_string<T>& assign(const T* str)
    {
        clear();
        return append(str);
    }

    basic_string<T>& assign(const T* str, size_t sz)
    {
        clear();
        return append(str, sz);
    }

    basic_string<T>& assign(const std::basic_string<T>& str)
    {
        clear();
        return append(str);
    }

    basic_string<T>& assign(const basic_string<T>& str)
    {
        clear();
        return append(str);
    }

public:
    basic_string<T> substr(size_t pos = 0, size_t len = npos) const
    {
        size_t sz = length();

        if (pos >= sz)
            return basic_string<T>();

        if (len == npos || pos + len > sz)
            len = sz - pos;

        if (len == sz)
            return basic_string<T>(*this);

        return basic_string<T>(c_str() + pos, len);
    }

    basic_string<char> hex() const
    {
        basic_string<char> retVal;
        static char HexChar[] = "0123456789abcdef";
        size_t i, pos, len1;
        size_t len = length();
        const T* ptr = c_str();

        i = len * (sizeof(T) * 2 + 1);
        retVal.resize(i);
        char* data = retVal.c_buffer();

        len1 = 0;
        pos = 0;

        for (i = 0; i < len; i++)
        {
            if (sizeof(T) == 2)
            {
                data[pos] = HexChar[(unsigned char) ptr[i] >> 12];
                data[pos + 1] = HexChar[(unsigned char) (ptr[i] >> 8) & 0xf];
                pos += 2;
                len1 += 2;
            }
            data[pos] = HexChar[(unsigned char) (ptr[i] >> 4) & 0x0f];
            data[pos + 1] = HexChar[(unsigned char) ptr[i] & 0xf];
            pos += 2;
            len1 += 2;

            data[pos++] = ' ';
            len1 ++;
        }

        return retVal;
    }

public:
    T &operator[](size_t i)
    {
        return c_buffer()[i];
    }

    basic_string<T> operator+=(T ch)
    {
        return append(1, ch);
    }

    basic_string<T> operator+=(const T* rhs)
    {
        return append(rhs);
    }

    basic_string<T> operator+=(const basic_string<T>& rhs)
    {
        return append(rhs);
    }

    basic_string<T>& operator=(const T* str)
    {
        return assign(str);
    }

    basic_string<T>& operator=(const basic_string<T>& str)
    {
        return assign(str);
    }

private:
    std::basic_string<T> m_str;
};

template<typename T>
inline basic_string <T> operator+(const basic_string<T>& lhs,
                                  const basic_string<T>& rhs)
{
    basic_string<T> str(lhs);
    str.append(rhs);
    return str;
}

template<typename T>
inline basic_string <T> operator+(const T* lhs, const basic_string<T>& rhs)
{
    basic_string<T> str(lhs);
    str.append(rhs);
    return str;
}

template<typename T>
inline basic_string <T> operator+(const basic_string<T>& lhs, const T* rhs)
{
    basic_string<T> str(lhs);
    str.append(rhs);
    return str;
}

template<typename T>
inline basic_string <T> operator+(T lhs, const basic_string<T>& rhs)
{
    basic_string<T> str(1, lhs);
    str.append(rhs);
    return str;
}

template<typename T>
inline basic_string <T> operator+(const basic_string<T>& lhs, T rhs)
{
    basic_string<T> str(lhs);
    str.append(1, rhs);
    return str;
}

template<typename T>
bool operator<(const basic_string<T>& lhs,
               const basic_string<T>& rhs)
{
    return lhs.compare(rhs) < 0;
}

template<typename T>
bool operator>(const basic_string<T>& lhs,
               const basic_string<T>& rhs)
{
    return lhs.compare(rhs) > 0;
}

template<typename T>
bool operator<=(const basic_string<T>& lhs,
                const basic_string<T>& rhs)
{
    return lhs.compare(rhs) <= 0;
}

template<typename T>
bool operator>=(const basic_string<T>& lhs,
                const basic_string<T>& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template<typename T>
bool operator==(const basic_string<T>& lhs,
                const basic_string<T>& rhs)
{
    return lhs.compare(rhs) == 0;
}

template<typename T>
bool operator!=(const basic_string<T>& lhs,
                const basic_string<T>& rhs)
{
    return lhs.compare(rhs) != 0;
}


typedef basic_string<char> qstring;
typedef basic_string<wchar> wstring;

}

#endif /* QSTRING_H_ */
