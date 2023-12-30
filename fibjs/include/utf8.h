/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include <string>
#include "qstring.h"

namespace fibjs {

char32_t utf_getchar(const char*& src, const char* end);
ssize_t utf_putchar(char32_t ch, char*& dst, const char* end);

char32_t utf_getchar(const char16_t*& src, const char16_t* end);
ssize_t utf_putchar(char32_t ch, char16_t*& dst, const char16_t* end);

char32_t utf_getchar(const char32_t*& src, const char32_t* end);
ssize_t utf_putchar(char32_t ch, char32_t*& dst, const char32_t* end);

ssize_t utf_convert(const char* src, ssize_t srclen, char16_t* dst, ssize_t dstlen);
ssize_t utf_convert(const char16_t* src, ssize_t srclen, char* dst, ssize_t dstlen);
ssize_t utf_convert(const char* src, ssize_t srclen, char32_t* dst, ssize_t dstlen);
ssize_t utf_convert(const char32_t* src, ssize_t srclen, char* dst, ssize_t dstlen);

inline ssize_t utf8_strlen(const char* src, ssize_t srclen)
{
    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    return utf_convert(src, srclen, (char16_t*)NULL, 0);
}

inline ssize_t utf8_strlen(const char16_t* src, ssize_t srclen)
{
    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    return utf_convert(src, srclen, (char*)NULL, 0);
}

inline exlib::wstring utf8to16String(const char* src, ssize_t srclen = -1)
{
    exlib::wstring str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert(src, srclen, (char16_t*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::wstring utf8to16String(exlib::string src)
{
    return utf8to16String(src.c_str(), (ssize_t)src.length());
}

#define UTF8_W(s) utf8to16String(s).c_str()

inline exlib::string utf16to8String(const char16_t* src, ssize_t srclen = -1)
{
    exlib::string str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert(src, srclen, (char*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::string utf16to8String(exlib::wstring src)
{
    return utf16to8String(src.c_str(), (ssize_t)src.length());
}

#define UTF8_A(s) utf16to8String(s).c_str()

inline exlib::wstring32 utf8to32String(const char* src, ssize_t srclen = -1)
{
    exlib::wstring32 str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert(src, srclen, (char32_t*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::wstring32 utf8to32String(exlib::string src)
{
    return utf8to32String(src.c_str(), (ssize_t)src.length());
}

inline exlib::string utf32to8String(const char32_t* src, ssize_t srclen = -1)
{
    exlib::string str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert(src, srclen, (char*)NULL, 0);
        str.resize(n);

        utf_convert(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::string utf32to8String(exlib::wstring32 src)
{
    return utf32to8String(src.c_str(), (ssize_t)src.length());
}

ssize_t utf_convert_s(const char* src, ssize_t srclen, char16_t* dst, ssize_t dstlen);
ssize_t utf_convert_s(const char16_t* src, ssize_t srclen, char* dst, ssize_t dstlen);
ssize_t utf_convert_s(const char* src, ssize_t srclen, char32_t* dst, ssize_t dstlen);
ssize_t utf_convert_s(const char32_t* src, ssize_t srclen, char* dst, ssize_t dstlen);

inline exlib::wstring utf8to16String_s(const char* src, ssize_t srclen = -1)
{
    exlib::wstring str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert_s(src, srclen, (char16_t*)NULL, 0);
        str.resize(n);

        utf_convert_s(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::wstring utf8to16String_s(exlib::string src)
{
    return utf8to16String_s(src.c_str(), (ssize_t)src.length());
}

inline exlib::string utf16to8String_s(const char16_t* src, ssize_t srclen = -1)
{
    exlib::string str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert_s(src, srclen, (char*)NULL, 0);
        str.resize(n);

        utf_convert_s(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::string utf16to8String_s(exlib::wstring src)
{
    return utf16to8String_s(src.c_str(), (ssize_t)src.length());
}

inline exlib::wstring32 utf8to32String_s(const char* src, ssize_t srclen = -1)
{
    exlib::wstring32 str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert_s(src, srclen, (char32_t*)NULL, 0);
        str.resize(n);

        utf_convert_s(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::wstring32 utf8to32String_s(exlib::string src)
{
    return utf8to32String_s(src.c_str(), (ssize_t)src.length());
}

inline exlib::string utf32to8String_s(const char32_t* src, ssize_t srclen = -1)
{
    exlib::string str;

    if (srclen == -1)
        srclen = (ssize_t)qstrlen(src);

    if (srclen) {
        ssize_t n = utf_convert_s(src, srclen, (char*)NULL, 0);
        str.resize(n);

        utf_convert_s(src, srclen, str.data(), n);
    }

    return str;
}

inline exlib::string utf32to8String_s(exlib::wstring32 src)
{
    return utf32to8String_s(src.c_str(), (ssize_t)src.length());
}
}
