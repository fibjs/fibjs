/*
 * qstring.h
 *
 *  Created on: Jul 26, 2012
 *      Author: lion
 */

#ifndef QSTRING_H_
#define QSTRING_H_

namespace fibjs
{

#ifdef _WIN32
typedef wchar_t wchar;
#else
typedef uint16_t wchar;
#endif
typedef std::basic_string<wchar> wstring;

template<typename T>
bool qisspace(T ch)
{
    return ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t';
}

template<typename T>
bool qisascii(T ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

template<typename T>
bool qisdigit(T ch)
{
    return ch >= '0' && ch <= '9';
}

template<typename T>
bool qisxdigit(T ch)
{
    return qisdigit(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'Z');
}

template<typename T>
int qhex(T ch)
{
    return (ch >= '0' && ch <= '9' ? ch - '0' :
            ch >= 'a' && ch <= 'f' ? ch - 'a' + 10 :
            ch >= 'A' && ch <= 'F' ? ch - 'A' + 10 : 0);
}

template<typename T>
bool qisupper(T ch)
{
    return ch >= 'A' && ch <= 'Z';
}

template<typename T>
bool qislower(T ch)
{
    return ch >= 'a' && ch <= 'z';
}

template<typename T>
T qtolower(T c)
{
    if (qisupper(c))
        return c + 'a' - 'A';
    return c;
}

template<typename T>
int qchricmp(T ch1, T ch2)
{
    return qtolower(ch1) - qtolower(ch2);
}

template<typename T>
int qstricmp(const T *s1, const T *s2, int sz = -1)
{
    int n = 0;

    while (*s1 && !(n = qchricmp(*s1++, *s2++)))
        if ((sz > 0) && (!--sz))
            return 0;

    return n ? n : -*s2;
}

template<typename T>
int qstrcmp(const T *s1, const T *s2, int sz = -1)
{
    int n = 0;

    while (*s1 && !(n = *s1++ - *s2++))
        if ((sz > 0) && (!--sz))
            return 0;

    return n ? n : -*s2;
}

template<typename T>
const T *qstrichr(const T *s, T c)
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
const T *qstrchr(const T *s, T c)
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
size_t qstrlen(const T *pStr)
{
    const T *pEnd;

    for (pEnd = pStr; *pEnd != 0; pEnd++)
        continue;

    return pEnd - pStr;
}

template<typename T>
const T *qstristr(const T *in, const T *str)
{
    T c;

    c = *str;
    if (!c)
        return NULL;

    int len = (int) qstrlen(str);

    while ((in = qstrichr(in, c)) && qstricmp(in, str, len))
        in++;
    return in;
}

template<typename T>
const T *qstrstr(const T *in, const T *str)
{
    T c;

    c = *str;
    if (!c)
        return NULL;

    int len = (int) qstrlen(str);

    while ((in = qstrchr(in, c)) && qstrcmp(in, str, len))
        in++;
    return in;
}

template<typename T>
void qstrlwr(T *s)
{
    T c;

    while ((c = *s) != 0)
        * s++ = qtolower(c);
}

}

#endif /* QSTRING_H_ */
