/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _utf_8_H_
#define _utf_8_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <string>
#include "qstring.h"

namespace fibjs
{

int32_t utf8_mbstowcs(const char *src, int32_t srclen, wchar *dst, int32_t dstlen);
int32_t utf8_wcstombs(const wchar *src, int32_t srclen, char *dst, int32_t dstlen);
int32_t utf8_getchar(const char *&src, const char *end);
int32_t utf8_putchar(int32_t ch, char *&dst, const char *end);
int32_t utf16_getchar(const wchar *&src, const wchar *end);
int32_t utf16_putchar(int32_t ch, wchar *&dst, const wchar *end);

inline int32_t utf8_strlen(const char *src, int32_t srclen)
{
    if (srclen == -1)
        srclen = (int32_t) qstrlen(src);

    return utf8_mbstowcs(src, srclen, NULL, 0);
}

inline int32_t utf8_strlen(const wchar *src, int32_t srclen)
{
    if (srclen == -1)
        srclen = (int32_t) qstrlen(src);

    return utf8_wcstombs(src, srclen, NULL, 0);
}

inline wstring utf8to16String(const char *src, int32_t srclen = -1)
{
    wstring str;

    if (srclen == -1)
        srclen = (int32_t) qstrlen(src);

    if (srclen)
    {
        int32_t n = utf8_mbstowcs(src, srclen, NULL, 0);
        str.resize(n);

        utf8_mbstowcs(src, srclen, &str[0], n);
    }

    return str;
}

inline wstring utf8to16String(qstring src)
{
    return utf8to16String(src.c_str(), (int32_t)src.length());
}

#define UTF8_W(s) utf8to16String(s).c_str()

inline qstring utf16to8String(const wchar *src, int32_t srclen = -1)
{
    qstring str;

    if (srclen == -1)
        srclen = (int32_t) qstrlen(src);

    if (srclen)
    {
        int32_t n = utf8_wcstombs(src, srclen, NULL, 0);
        str.resize(n);

        utf8_wcstombs(src, srclen, &str[0], n);
    }

    return str;
}

inline qstring utf16to8String(wstring src)
{
    return utf16to8String(src.c_str(), (int32_t)src.length());
}

#define UTF8_A(s) utf16to8String(s).c_str()

}

#endif
