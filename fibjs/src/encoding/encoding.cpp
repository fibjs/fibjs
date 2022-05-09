/*
 * encoding.cpp
 *
 *  Created on: Apr 10, 2012
 *      Author: lion
 */

#include "object.h"
#include "encoding.h"
#include "encoding_iconv.h"
#include "Url.h"
#include "libbase58.h"
#include <math.h>

namespace fibjs {

DECLARE_MODULE(encoding);
DECLARE_MODULE(base32);
DECLARE_MODULE(base58);
DECLARE_MODULE(base64);
DECLARE_MODULE(hex);

static void baseEncode(const char* pEncodingTable, size_t dwBits,
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

static void baseDecode(const char* pdecodeTable, size_t dwBits,
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

static void baseDecode(const char* pdecodeTable, size_t dwBits,
    exlib::string& baseString, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;
    baseDecode(pdecodeTable, dwBits, baseString, strBuf);
    retVal = new Buffer(strBuf);
}

static result_t base32Encode(const char* data, size_t sz, exlib::string& retVal)
{
    baseEncode("abcdefghijklmnopqrstuvwxyz234567", 5, data, sz, retVal, true);
    return 0;
}

result_t base64Encode(const char* data, size_t sz, bool url, exlib::string& retVal)
{
    if (url)
        baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_",
            6, data, sz, retVal, false);
    else
        baseEncode("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
            6, data, sz, retVal, true);

    return 0;
}

result_t base64Decode(exlib::string data, exlib::string& retVal)
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

static result_t hexEncode(exlib::string data, exlib::string& retVal)
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

static result_t base58Encode(const char* data, size_t sz, exlib::string& retVal)
{
    size_t b58sz = (size_t)(sz * 8 / log2l(58) + 2);

    retVal.resize(b58sz);
    b58enc(retVal.c_buffer(), &b58sz, data, sz);
    retVal.resize(b58sz - 1);
    return 0;
}

result_t commonEncode(exlib::string codec, exlib::string data, exlib::string& retVal)
{
    result_t hr;
    if ((codec == "utf8") || (codec == "utf-8") || (codec == "undefined")) {
        retVal = data;
        hr = 0;
    } else {
        if ((codec == "hex"))
            hr = hexEncode(data, retVal);
        else if ((codec == "base32"))
            hr = base32Encode(data.c_str(), data.length(), retVal);
        else if ((codec == "base58"))
            hr = base58Encode(data.c_str(), data.length(), retVal);
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

result_t base32_base::encode(Buffer_base* data, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);
    base32Encode(strData.c_str(), strData.length(), retVal);
    return 0;
}

result_t base32_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    static const char decodeTable[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, /* 2x  !"#$%&'()*+,-./   */
        14, 11, 26, 27, 28, 29, 30, 31, -1, 6, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 6x `abcdefghijklmno   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    baseDecode(decodeTable, 5, data, retVal);
    return 0;
}

result_t base64_base::encode(Buffer_base* data, bool url, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);
    return base64Encode(strData.c_str(), strData.length(), url, retVal);
}

result_t base64_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
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

result_t base58_base::encode(Buffer_base* data, exlib::string& retVal)
{
    exlib::string buffer;

    data->toString(buffer);
    base58Encode(buffer.c_str(), buffer.length(), retVal);

    return 0;
}

result_t base58_base::encode(Buffer_base* data, int32_t chk_ver, exlib::string& retVal)
{
    exlib::string buffer;

    data->toString(buffer);
    size_t b58sz = (size_t)((buffer.length() + 5) * 8 / log2l(58) + 2);

    retVal.resize(b58sz);
    b58check_enc(retVal.c_buffer(), &b58sz, chk_ver, buffer.c_str(), buffer.length());
    retVal.resize(b58sz - 1);

    return 0;
}

result_t base58_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    size_t binsz = (size_t)(data.length() * log2l(58) / 8 + 3);
    exlib::string buffer;

    buffer.resize(binsz);
    if (!b58tobin(buffer.c_buffer(), &binsz, data.c_str(), data.length()))
        return CHECK_ERROR(Runtime::setError("base58: encode error."));
    if (binsz < buffer.length())
        buffer = buffer.substr(buffer.length() - binsz);

    retVal = new Buffer(buffer);

    return 0;
}

result_t base58_base::decode(exlib::string data, int32_t chk_ver, obj_ptr<Buffer_base>& retVal)
{
    size_t binsz = (size_t)(data.length() * log2l(58) / 8 + 3);
    exlib::string buffer;

    buffer.resize(binsz);
    if (!b58tobin(buffer.c_buffer(), &binsz, data.c_str(), data.length()))
        return CHECK_ERROR(Runtime::setError("base58: decode error."));
    if (b58check(buffer.c_str() + buffer.length() - binsz, binsz, data.c_str(), data.length()) != chk_ver)
        return CHECK_ERROR(Runtime::setError("base58: check error."));

    if (binsz < buffer.length())
        buffer = buffer.substr(buffer.length() - binsz + 1, binsz - 5);

    retVal = new Buffer(buffer);

    return 0;
}

result_t hex_base::encode(Buffer_base* data, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);
    return hexEncode(strData, retVal);
}

result_t hex_base::decode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    const char* _data = data.c_str();
    int32_t pos, len = (int32_t)data.length();
    const char* end = _data + len;
    exlib::string strBuf;
    unsigned char ch1, ch2;

    strBuf.resize(len / 2);
    char* _strBuf = strBuf.c_buffer();

    pos = 0;
    while ((_data < end - 1) && (ch1 = (unsigned char)*_data++)) {
        if (qisxdigit(ch1))
            ch1 = qhex(ch1);
        else
            continue;

        ch2 = *_data++;
        if (ch2 == 0)
            break;

        if (qisxdigit(ch2))
            ch2 = qhex(ch2);
        else {
            ch2 = ch1;
            ch1 = 0;
        }

        _strBuf[pos++] = (ch1 << 4) + ch2;
    }

    strBuf.resize(pos);
    retVal = new Buffer(strBuf);

    return 0;
}

static const char* URITable = " ! #$ &'()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";
static const char* URIComponentTable = " !     '()*  -. 0123456789       ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";

result_t encoding_base::encodeURI(exlib::string url, exlib::string& retVal)
{
    Url::encodeURI(url, retVal, URITable);
    return 0;
}

result_t encoding_base::encodeURIComponent(exlib::string url, exlib::string& retVal)
{
    Url::encodeURI(url, retVal, URIComponentTable);
    return 0;
}

result_t encoding_base::decodeURI(exlib::string url, exlib::string& retVal)
{
    Url::decodeURI(url, retVal);
    return 0;
}
}
