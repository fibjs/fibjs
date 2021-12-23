/*
 * encoding_iconv.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#pragma once

#include "Buffer.h"
#include "utf8.h"
#include "ifs/encoding.h"
#include "encoding_iconv.h"

namespace fibjs {

inline void baseEncode(const char* pEncodingTable, size_t dwBits,
    const char* data, size_t sz, exlib::string& retVal, bool fill)
{
    size_t i, len = 0, bits = 0;
    size_t dwData = 0;
    size_t dwSize = 0;
    char bMask = 0xff >> (8 - dwBits);

    if (dwBits == 6)
        dwSize = (sz + 2) / 3 * 4;
    else if (dwBits == 5)
        dwSize = (sz + 4) / 5 * 8;

    retVal.resize(dwSize);
    char* _retVal = retVal.c_buffer();

    for (i = 0; i < sz; i++) {
        dwData <<= 8;
        dwData |= (unsigned char)data[i];
        bits += 8;

        while (bits >= dwBits) {
            _retVal[len++] = pEncodingTable[(dwData >> (bits - dwBits)) & bMask];
            bits -= dwBits;
        }
    }

    if (bits)
        _retVal[len++] = pEncodingTable[(dwData << (dwBits - bits)) & bMask];

    if (fill)
        while (len < dwSize)
            _retVal[len++] = '=';

    retVal.resize(len);
}

inline void baseDecode(const char* pdecodeTable, size_t dwBits,
    exlib::string& baseString, exlib::string& retVal)
{
    const char* _baseString = baseString.c_str();
    size_t len = baseString.length();
    size_t nWritten = 0;
    const char* end = _baseString + len;

    retVal.resize(len * dwBits / 8);
    char* _retVal = retVal.c_buffer();

    size_t dwCurr = 0;
    size_t nBits = 0;
    unsigned char ch;

    while ((_baseString < end) && (ch = (unsigned char)*_baseString++)) {
        int32_t nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

        if (nCh != -1) {
            dwCurr <<= dwBits;
            dwCurr |= nCh;
            nBits += dwBits;

            while (nBits >= 8) {
                _retVal[nWritten++] = (char)(dwCurr >> (nBits - 8));
                nBits -= 8;
            }
        }
    }

    retVal.resize(nWritten);
}

inline void baseDecode(const char* pdecodeTable, size_t dwBits,
    exlib::string& baseString, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    baseDecode(pdecodeTable, dwBits, baseString, strBuf);
    retVal = new Buffer(strBuf);
}

inline result_t base64Encode(const char* data, size_t sz, bool url, exlib::string& retVal)
{
    if (url)
        baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_",
            6, data, sz, retVal, false);
    else
        baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
            6, data, sz, retVal, true);

    return 0;
}

inline result_t base64Decode(exlib::string data, exlib::string& retVal)
{
    static const char decodeTable[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, /* 2x  !"#$%&'()*+,-./   */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x `abcdefghijklmno   */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    baseDecode(decodeTable, 6, data, retVal);
    return 0;
}

inline result_t hexEncode(exlib::string data, exlib::string& retVal)
{
    static char HexChar[] = "0123456789abcdef";
    size_t i;
    size_t sz = data.length();

    retVal.resize(sz * 2);
    char* _retVal = retVal.c_buffer();
    const char* _data = data.c_str();

    for (i = 0; i < sz; i++) {
        unsigned char ch = (unsigned char)_data[i];

        _retVal[i * 2] = HexChar[ch >> 4];
        _retVal[i * 2 + 1] = HexChar[ch & 0xf];
    }

    return 0;
}

/**
 * hex & base64 use encode: binary -> hex ,binary -> base64
 * 
 * iconv use decode: local encoding string -> unicode
 * 
 */
inline result_t commonEncode(exlib::string codec, exlib::string data, exlib::string& retVal)
{
    result_t hr;
    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined")) {
        retVal = data;
        hr = 0;
    } else {
        if ((codec == "hex"))
            hr = hexEncode(data, retVal);
        else if ((codec == "base64"))
            hr = base64Encode(data.c_str(), data.length(), false, retVal);
        else if ((codec == "base64url"))
            hr = base64Encode(data.c_str(), data.length(), true, retVal);
        else if ((codec == "ascii")) {
            size_t len, i;

            len = data.length();
            retVal.resize(len);

            char* _retVal = retVal.c_buffer();
            const char* _data = data.c_str();

            for (i = 0; i < len; i++)
                _retVal[i] = _data[i] & 0x7f;

            hr = 0;
        } else
            hr = encoding_iconv(codec).decode(data, retVal);
    }
    return hr;
}

} /* namespace fibjs */
