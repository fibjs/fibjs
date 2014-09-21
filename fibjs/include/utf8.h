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

int utf8_mbstowcs(const char *src, int srclen, wchar *dst, int dstlen);
int utf8_wcstombs(const wchar *src, int srclen, char *dst, int dstlen);
int utf8_getchar(const char *&src, const char *end);
int utf8_putchar(int ch, char *&dst, const char *end);
int utf16_getchar(const wchar *&src, const wchar *end);
int utf16_putchar(int ch, wchar *&dst, const wchar *end);

inline int utf8_strlen(const char *src, int srclen)
{
    if (srclen == -1)
        srclen = (int) qstrlen(src);

    return utf8_mbstowcs(src, srclen, NULL, 0);
}

inline int utf8_strlen(const wchar *src, int srclen)
{
    if (srclen == -1)
        srclen = (int) qstrlen(src);

    return utf8_wcstombs(src, srclen, NULL, 0);
}

inline wstring utf8to16String(const char *src, int srclen = -1)
{
    wstring str;

    if (srclen == -1)
        srclen = (int) qstrlen(src);

    if (srclen)
    {
        int n = utf8_mbstowcs(src, srclen, NULL, 0);
        str.resize(n);

        utf8_mbstowcs(src, srclen, &str[0], n);
    }

    return str;
}

inline wstring utf8to16String(std::string src)
{
    return utf8to16String(src.c_str(), (int)src.length());
}

#define UTF8_W(s) utf8to16String(s).c_str()

inline std::string utf16to8String(const wchar *src, int srclen = -1)
{
    std::string str;

    if (srclen == -1)
        srclen = (int) qstrlen(src);

    if (srclen)
    {
        int n = utf8_wcstombs(src, srclen, NULL, 0);
        str.resize(n);

        utf8_wcstombs(src, srclen, &str[0], n);
    }

    return str;
}

inline std::string utf16to8String(wstring src)
{
    return utf16to8String(src.c_str(), (int)src.length());
}

#define UTF8_A(s) utf16to8String(s).c_str()

}

#endif
