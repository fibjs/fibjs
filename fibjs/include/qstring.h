/*
 * qstring.h
 *
 *  Created on: Jul 26, 2012
 *      Author: lion
 */

#include <string>
#include <exlib/include/qstring.h>

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

inline size_t qstrlen(const char* pStr)
{
    return strlen(pStr);
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

#ifdef _WIN32
typedef wchar_t wchar;
#else
typedef uint16_t wchar;
#endif

typedef exlib::basic_string<char> qstring;
typedef exlib::basic_string<wchar> wstring;

}

#endif /* QSTRING_H_ */
