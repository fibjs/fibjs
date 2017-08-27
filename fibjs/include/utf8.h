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

namespace fibjs {

exlib::wchar32 utf_getchar(const char*& src, const char* end);
int32_t utf_putchar(exlib::wchar32 ch, char*& dst, const char* end);

exlib::wchar32 utf_getchar(const exlib::wchar*& src, const exlib::wchar* end);
int32_t utf_putchar(exlib::wchar32 ch, exlib::wchar*& dst, const exlib::wchar* end);

exlib::wchar32 utf_getchar(const exlib::wchar32*& src, const exlib::wchar32* end);
int32_t utf_putchar(exlib::wchar32 ch, exlib::wchar32*& dst, const exlib::wchar32* end);

template <typename T1, typename T2>
inline int32_t utf_convert(const T1* src, int32_t srclen, T2* dst, int32_t dstlen)
{
    int32_t count = 0;
    const T1* src_end = src + srclen;
    const T2* dst_end = dst + dstlen;

    while (src < src_end)
        count += utf_putchar(utf_getchar(src, src_end), dst, dst_end);

    return count;
}

inline int32_t utf8_strlen(const char* src, int32_t srclen)
{
    if (srclen == -1)
        srclen = (int32_t)qstrlen(src);

    return utf_convert(src, srclen, (exlib::wchar*)NULL, 0);
}

inline int32_t utf8_strlen(const exlib::wchar* src, int32_t srclen)
{
    if (srclen == -1)
        srclen = (int32_t)qstrlen(src);

    return utf_convert(src, srclen, (char*)NULL, 0);
}

inline exlib::wstring utf8to16String(const char* src, int32_t srclen = -1)
{
    exlib::wstring str;

    if (srclen == -1)
        srclen = (int32_t)qstrlen(src);

    if (srclen) {
        int32_t n = utf_convert(src, srclen, (exlib::wchar*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, &str[0], n);
    }

    return str;
}

inline exlib::wstring utf8to16String(exlib::string src)
{
    return utf8to16String(src.c_str(), (int32_t)src.length());
}

#define UTF8_W(s) utf8to16String(s).c_str()

inline exlib::string utf16to8String(const exlib::wchar* src, int32_t srclen = -1)
{
    exlib::string str;

    if (srclen == -1)
        srclen = (int32_t)qstrlen(src);

    if (srclen) {
        int32_t n = utf_convert(src, srclen, (char*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, &str[0], n);
    }

    return str;
}

inline exlib::string utf16to8String(exlib::wstring src)
{
    return utf16to8String(src.c_str(), (int32_t)src.length());
}

#define UTF8_A(s) utf16to8String(s).c_str()

inline exlib::wstring32 utf8to32String(const char* src, int32_t srclen = -1)
{
    exlib::wstring32 str;

    if (srclen == -1)
        srclen = (int32_t)qstrlen(src);

    if (srclen) {
        int32_t n = utf_convert(src, srclen, (exlib::wchar32*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, &str[0], n);
    }

    return str;
}

inline exlib::wstring32 utf8to32String(exlib::string src)
{
    return utf8to32String(src.c_str(), (int32_t)src.length());
}

inline exlib::string utf32to8String(const exlib::wchar32* src, int32_t srclen = -1)
{
    exlib::string str;

    if (srclen == -1)
        srclen = (int32_t)qstrlen(src);

    if (srclen) {
        int32_t n = utf_convert(src, srclen, (char*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, &str[0], n);
    }

    return str;
}

inline exlib::string utf32to8String(exlib::wstring32 src)
{
    return utf32to8String(src.c_str(), (int32_t)src.length());
}
}

#endif
