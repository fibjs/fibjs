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

namespace fibjs {

DECLARE_MODULE(encoding);
DECLARE_MODULE(base32);
DECLARE_MODULE(base64);
DECLARE_MODULE(base64vlq);
DECLARE_MODULE(hex);

result_t base32_base::encode(Buffer_base* data, exlib::string& retVal)
{
    baseEncode("abcdefghijklmnopqrstuvwxyz234567", 5, data, retVal);
    return 0;
}

result_t base32_base::decode(exlib::string data,
    obj_ptr<Buffer_base>& retVal)
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
    return base64Encode(strData, url, retVal);
}

result_t base64_base::decode(exlib::string data,
    obj_ptr<Buffer_base>& retVal)
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

result_t base64vlq_base::encode(int32_t data, exlib::string& retVal)
{
    static const char encodeTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if (data < 0)
        data = ((-data) << 1) | 1;
    else
        data <<= 1;

    do {
        int32_t byte = data & 0x1f;
        data >>= 5;
        if (data)
            byte |= 0x20;

        retVal.append(1, encodeTable[byte]);
    } while (data > 0);

    return 0;
}

result_t base64vlq_base::encode(v8::Local<v8::Array> data, exlib::string& retVal)
{
    int32_t len = data->Length();
    result_t hr;

    for (int32_t i = 0; i < len; i++) {
        int32_t num;
        hr = GetArgumentValue(JSValue(data->Get(i)), num);
        if (hr < 0)
            return CHECK_ERROR(hr);

        hr = encode(num, retVal);
        if (hr < 0)
            return hr;
    }

    return 0;
}

result_t base64vlq_base::decode(exlib::string data, v8::Local<v8::Array>& retVal)
{
    static const char decodeTable[] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, /* 2x  !"#$%&'()*+,-./   */
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, /* 3x 0123456789:;<=>?   */
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, /* 4x @ABCDEFGHIJKLMNO   */
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, 63, /* 5X PQRSTUVWXYZ[\]^_   */
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 6x `abcdefghijklmno   */
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1 /* 7X pqrstuvwxyz{\}~DEL */
    };

    const char* _data = data.c_str();
    const char* end = _data + data.length();

    int32_t cnt = 0;

    Isolate* isolate = Isolate::current();
    retVal = v8::Array::New(isolate->m_isolate);

    while (_data < end) {
        uint32_t ch;
        int32_t bits = 0;
        int32_t num = 0;

        while ((ch = utf_getchar(_data, end)) != 0) {
            int32_t byte = (ch > 0x20 && ch < 0x80) ? decodeTable[ch - 0x20] : -1;

            if (byte != -1) {
                num += (byte & 0x1f) << (bits * 5);
                bits++;

                if (!(byte & 0x20))
                    break;
            } else
                break;
        }

        if (num & 1)
            num = -(num >> 1);
        else
            num = num >> 1;

        retVal->Set(cnt++, v8::Number::New(isolate->m_isolate, num));
    }

    return 0;
}

result_t hex_base::encode(Buffer_base* data, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);
    return hexEncode(strData, retVal);
}

result_t hex_base::decode(exlib::string data,
    obj_ptr<Buffer_base>& retVal)
{
    const char* _data = data.c_str();
    int32_t pos, len = (int32_t)data.length();
    const char* end = _data + len;
    exlib::string strBuf;
    uint32_t ch1, ch2;

    strBuf.resize(len / 2);

    pos = 0;
    while ((ch1 = utf_getchar(_data, end)) != 0) {
        if (qisxdigit(ch1))
            ch1 = qhex(ch1);
        else
            continue;

        ch2 = utf_getchar(_data, end);
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
