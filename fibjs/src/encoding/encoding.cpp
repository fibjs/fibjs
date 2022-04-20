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

result_t base32_base::encode(Buffer_base* data, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);
    baseEncode("abcdefghijklmnopqrstuvwxyz234567", 5, strData.c_str(), strData.length(), retVal, true);
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
    size_t b58sz = (size_t)(buffer.length() * 8 / log2l(58) + 2);

    retVal.resize(b58sz);
    b58enc(retVal.c_buffer(), &b58sz, buffer.c_str(), buffer.length());
    retVal.resize(b58sz - 1);

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
