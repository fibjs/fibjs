/*
 * encoding_iconv.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "Buffer.h"
#include "utf8.h"
#include "ifs/encoding.h"
#include "encoding_iconv.h"

#ifndef ENCODING_H_
#define ENCODING_H_

namespace fibjs {

inline void baseEncode(const char* pEncodingTable, int32_t dwBits,
    const char* data, int32_t sz, exlib::string& retVal)
{
    int32_t i, len = 0, bits = 0;
    int32_t dwData = 0;
    int32_t dwSize = 0;
    char bMask = 0xff >> (8 - dwBits);

    if (dwBits == 6)
        dwSize = (sz + 2) / 3 * 4;
    else if (dwBits == 5)
        dwSize = (sz + 4) / 5 * 8;

    retVal.resize(dwSize);

    for (i = 0; i < sz; i++) {
        dwData <<= 8;
        dwData |= (unsigned char)data[i];
        bits += 8;

        while (bits >= dwBits) {
            retVal[len++] = pEncodingTable[(dwData >> (bits - dwBits)) & bMask];
            bits -= dwBits;
        }
    }

    if (bits)
        retVal[len++] = pEncodingTable[(dwData << (dwBits - bits)) & bMask];

    while (len < dwSize)
        retVal[len++] = '=';

    retVal.resize(len);
}

inline void baseEncode(const char* pEncodingTable, int32_t dwBits,
    exlib::string& data, exlib::string& retVal)
{
    baseEncode(pEncodingTable, dwBits, data.c_str(),
        (int32_t)data.length(), retVal);
}

inline void baseEncode(const char* pEncodingTable, int32_t dwBits,
    Buffer_base* data, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);
    baseEncode(pEncodingTable, dwBits, strData, retVal);
}

inline void baseDecode(const char* pdecodeTable, int32_t dwBits,
    exlib::string& baseString, exlib::string& retVal)
{
    const char* _baseString = baseString.c_str();
    int32_t len = (int32_t)baseString.length();
    int32_t nWritten = 0;
    const char* end = _baseString + len;

    retVal.resize(len * dwBits / 8);

    int32_t dwCurr = 0;
    int32_t nBits = 0;
    uint32_t ch;

    while ((ch = utf8_getchar(_baseString, end)) != 0) {
        int32_t nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

        if (nCh != -1) {
            dwCurr <<= dwBits;
            dwCurr |= nCh;
            nBits += dwBits;

            while (nBits >= 8) {
                retVal[nWritten++] = (char)(dwCurr >> (nBits - 8));
                nBits -= 8;
            }
        }
    }

    retVal.resize(nWritten);
}

inline void baseDecode(const char* pdecodeTable, int32_t dwBits,
    exlib::string& baseString, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    baseDecode(pdecodeTable, dwBits, baseString, strBuf);
    retVal = new Buffer(strBuf);
}

inline result_t base64Encode(exlib::string data,
    exlib::string& retVal)
{
    baseEncode(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
        6, data, retVal);
    return 0;
}

inline result_t hexEncode(exlib::string data, exlib::string& retVal)
{
    static char HexChar[] = "0123456789abcdef";
    int32_t i, pos;
    int32_t len = (int32_t)data.length();

    if (len > 0) {
        retVal.resize(len * 3 - 1);

        pos = 0;
        for (i = 0; i < len; i++) {
            retVal[pos * 3] = HexChar[(unsigned char)data[i] >> 4];
            retVal[pos * 3 + 1] = HexChar[(unsigned char)data[i] & 0xf];
            if (i < len)
                retVal[pos * 3 + 2] = ' ';
            pos++;
        }
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
            hr = base64Encode(data, retVal);
        else if ((codec == "ascii")) {
            int32_t len, i;

            len = (int32_t)data.length();
            retVal.resize(len);
            const char* _data = data.c_str();
            for (i = 0; i < len; i++)
                retVal[i] = _data[i] & 0x7f;

            hr = 0;
        } else if ((codec == "ucs2") || (codec == "ucs-2") || (codec == "utf16le") || (codec == "utf-16le")) {
            retVal = utf16to8String((const exlib::wchar*)data.c_str(), (int32_t)data.length() / 2);
            hr = 0;
        } else
            hr = encoding_iconv(codec).decode(data, retVal);
    }
    return hr;
}

} /* namespace fibjs */
#endif /* ENCODING_H_ */
