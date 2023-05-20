/*
 * encoding_iconv_ucs.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: lion
 */

#include "config.h"

#include <stddef.h>

#include "object.h"
#include "encoding_iconv.h"
#include "ifs/encoding.h"
#include "utf8.h"

namespace fibjs {

inline bool is_ucs2(exlib::string codec)
{
    if ((codec == "ucs2") || (codec == "ucs-2")
        || (codec == "utf16") || (codec == "utf-16"))
        return true;

    if (is_big_endian()) {
        if ((codec == "ucs2be") || (codec == "ucs-2be")
            || (codec == "utf16be") || (codec == "utf-16be"))
            return true;
    } else {
        if ((codec == "ucs2le") || (codec == "ucs-2le")
            || (codec == "utf16le") || (codec == "utf-16le"))
            return true;
    }

    return false;
}

inline bool is_ucs2_s(exlib::string codec)
{
    if (!is_big_endian()) {
        if ((codec == "ucs2be") || (codec == "ucs-2be")
            || (codec == "utf16be") || (codec == "utf-16be"))
            return true;
    } else {
        if ((codec == "ucs2le") || (codec == "ucs-2le")
            || (codec == "utf16le") || (codec == "utf-16le"))
            return true;
    }

    return false;
}

inline bool is_ucs4(exlib::string codec)
{
    if ((codec == "ucs4") || (codec == "ucs-4")
        || (codec == "utf32") || (codec == "utf-32"))
        return true;

    if (is_big_endian()) {
        if ((codec == "ucs4be") || (codec == "ucs-4be")
            || (codec == "utf32be") || (codec == "utf-32be"))
            return true;
    } else {
        if ((codec == "ucs4le") || (codec == "ucs-4le")
            || (codec == "utf32le") || (codec == "utf-32le"))
            return true;
    }

    return false;
}

inline bool is_ucs4_s(exlib::string codec)
{
    if (!is_big_endian()) {
        if ((codec == "ucs4be") || (codec == "ucs-4be")
            || (codec == "utf32be") || (codec == "utf-32be"))
            return true;
    } else {
        if ((codec == "ucs4le") || (codec == "ucs-4le")
            || (codec == "utf32le") || (codec == "utf-32le"))
            return true;
    }

    return false;
}

bool encoding_iconv::is_ucs_encoding(exlib::string charset)
{
    if ((charset == "utf8") || (charset == "utf-8")

        || (charset == "ucs2") || (charset == "ucs-2")
        || (charset == "utf16") || (charset == "utf-16")

        || (charset == "ucs2le") || (charset == "ucs-2le")
        || (charset == "utf16le") || (charset == "utf-16le")

        || (charset == "ucs2be") || (charset == "ucs-2be")
        || (charset == "utf16be") || (charset == "utf-16be")

        || (charset == "ucs4") || (charset == "ucs-4")
        || (charset == "utf32") || (charset == "utf-32")

        || (charset == "ucs4le") || (charset == "ucs-4le")
        || (charset == "utf32le") || (charset == "utf-32le")

        || (charset == "ucs4be") || (charset == "ucs-4be")
        || (charset == "utf32be") || (charset == "utf-32be")

        || (charset == "binary") || (charset == "latin1")) {
        return true;
    }

    return false;
}

result_t encoding_iconv::ucs_encode(exlib::string data, exlib::string& retVal)
{
    if ((m_charset == "utf8") || (m_charset == "utf-8")) {
        retVal = data;
        return 0;
    }

    if ((m_charset == "binary") || (m_charset == "latin1")) {
        exlib::wstring wdata = utf8to16String(data);
        size_t sz = wdata.length();
        const exlib::wchar* s = wdata.c_str();

        retVal.resize(sz);
        char* s1 = retVal.c_buffer();
        for (size_t i = 0; i < sz; i++)
            *s1++ = (char)*s++;
        return 0;
    }

    if (is_ucs2(m_charset)) {
        ssize_t n = utf_convert(data.c_str(), data.length(), (exlib::wchar*)NULL, 0);
        retVal.resize(n * sizeof(exlib::wchar));
        utf_convert(data.c_str(), data.length(), (exlib::wchar*)retVal.c_buffer(), n);

        return 0;
    }

    if (is_ucs4(m_charset)) {
        ssize_t n = utf_convert(data.c_str(), data.length(), (exlib::wchar32*)NULL, 0);
        retVal.resize(n * sizeof(exlib::wchar32));
        utf_convert(data.c_str(), data.length(), (exlib::wchar32*)retVal.c_buffer(), n);

        return 0;
    }

    if (is_ucs2_s(m_charset)) {
        ssize_t n = utf_convert_s(data.c_str(), data.length(), (exlib::wchar*)NULL, 0);
        retVal.resize(n * sizeof(exlib::wchar));
        utf_convert_s(data.c_str(), data.length(), (exlib::wchar*)retVal.c_buffer(), n);

        return 0;
    }

    if (is_ucs4_s(m_charset)) {
        ssize_t n = utf_convert_s(data.c_str(), data.length(), (exlib::wchar32*)NULL, 0);
        retVal.resize(n * sizeof(exlib::wchar32));
        utf_convert_s(data.c_str(), data.length(), (exlib::wchar32*)retVal.c_buffer(), n);

        return 0;
    }

    return -1;
}

result_t encoding_iconv::ucs_decode(const char* data, size_t sz, exlib::string& retVal)
{
    if ((m_charset == "utf8") || (m_charset == "utf-8")) {
        retVal.assign(data, sz);
        return 0;
    }

    if ((m_charset == "binary") || (m_charset == "latin1")) {
        exlib::wstring wdata;
        wdata.resize(sz);
        exlib::wchar* s1 = wdata.c_buffer();
        for (size_t i = 0; i < sz; i++)
            *s1++ = *data++;

        retVal = utf16to8String(wdata);
        return 0;
    }

    if (is_ucs2(m_charset)) {
        retVal = utf16to8String((const exlib::wchar*)data, sz / sizeof(exlib::wchar));
        return 0;
    }

    if (is_ucs4(m_charset)) {
        retVal = utf32to8String((const exlib::wchar32*)data, sz / sizeof(exlib::wchar32));
        return 0;
    }

    if (is_ucs2_s(m_charset)) {
        retVal = utf16to8String_s((const exlib::wchar*)data, sz / sizeof(exlib::wchar));
        return 0;
    }

    if (is_ucs4_s(m_charset)) {
        retVal = utf32to8String_s((const exlib::wchar32*)data, sz / sizeof(exlib::wchar32));
        return 0;
    }

    return -1;
}

result_t encoding_iconv::ucs_decode(exlib::string data, exlib::string& retVal)
{
    return ucs_decode(data.c_str(), data.length(), retVal);
}
}
