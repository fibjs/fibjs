/*
 * encoding_iconv.h
 *
 *  Created on: Sep 11, 2015
 *      Author: lion
 */

#include "Buffer.h"
#include "utf8.h"
#include "ifs/encoding.h"

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

inline result_t base64Decode(exlib::string data,
    exlib::string& retVal)
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
    int32_t i, pos, len1;

    i = (int32_t)data.length() * 2;
    retVal.resize(i);

    len1 = 0;
    pos = 0;

    for (i = 0; i < (int32_t)data.length(); i++) {
        retVal[pos * 2] = HexChar[(unsigned char)data[i] >> 4];
        retVal[pos * 2 + 1] = HexChar[(unsigned char)data[i] & 0xf];
        pos++;
        len1 += 2;
    }

    return 0;
}

inline result_t hexDecode(exlib::string data,
    exlib::string& retVal)
{
    const char* _data = data.c_str();
    int32_t pos, len = (int32_t)data.length();
    const char* end = _data + len;
    exlib::string strBuf;
    uint32_t ch1, ch2;

    strBuf.resize(len / 2);

    pos = 0;
    while ((ch1 = utf8_getchar(_data, end)) != 0) {
        if (qisxdigit(ch1))
            ch1 = qhex(ch1);
        else
            continue;

        ch2 = utf8_getchar(_data, end);
        if (ch2 == 0)
            break;

        if (qisxdigit(ch2))
            ch2 = qhex(ch2);
        else {
            ch2 = ch1;
            ch1 = 0;
        }

        strBuf[pos++] = (ch1 << 4) + ch2;
    }
    retVal = strBuf;
    return 0;
}

} /* namespace fibjs */
#endif /* ENCODING_H_ */
