/*
 * encoding.cpp
 *
 *  Created on: Apr 10, 2012
 *      Author: lion
 */

#include "ifs/encoding.h"
#include "encoding_iconv.h"
#include "Url.h"

namespace fibjs
{

inline void baseEncode(const char *pEncodingTable, int dwBits,
                       Buffer_base *data, std::string &retVal)
{
    std::string strData;
    int i, len = 0, bits = 0;
    int dwData = 0;
    int dwSize = 0;
    char bMask = 0xff >> (8 - dwBits);

    data->toString(strData);

    if (dwBits == 6)
        dwSize = ((int) strData.length() + 2) / 3 * 4;
    else if (dwBits == 5)
        dwSize = ((int) strData.length() + 4) / 5 * 8;

    retVal.resize(dwSize);

    for (i = 0; i < (int) strData.length(); i++)
    {
        dwData <<= 8;
        dwData |= (unsigned char) strData[i];
        bits += 8;

        while (bits >= dwBits)
        {
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

inline void baseDecode(const char *pdecodeTable, int dwBits,
                       const char *baseString, obj_ptr<Buffer_base> &retVal)
{
    int nWritten = 0, len = (int) qstrlen(baseString);
    const char *end = baseString + len;
    std::string strBuf;

    strBuf.resize(len * dwBits / 8);

    int dwCurr = 0;
    int nBits = 0;
    uint32_t ch;

    while ((ch = utf8_getchar(baseString, end)) != 0)
    {
        int nCh = (ch > 0x20 && ch < 0x80) ? pdecodeTable[ch - 0x20] : -1;

        if (nCh != -1)
        {
            dwCurr <<= dwBits;
            dwCurr |= nCh;
            nBits += dwBits;

            while (nBits >= 8)
            {
                strBuf[nWritten++] = (char) (dwCurr >> (nBits - 8));
                nBits -= 8;
            }
        }
    }

    strBuf.resize(nWritten);

    retVal = new Buffer(strBuf);
}

result_t encoding_base::base32Encode(Buffer_base *data, std::string &retVal)
{
    baseEncode("abcdefghijklmnopqrstuvwxyz234567", 5, data, retVal);
    return 0;
}

result_t encoding_base::base32Decode(const char *data,
                                     obj_ptr<Buffer_base> &retVal)
{
    static const char decodeTable[] =
    {
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

result_t encoding_base::base64Encode(Buffer_base *data, std::string &retVal)
{
    baseEncode(
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
        6, data, retVal);
    return 0;
}

result_t encoding_base::base64Decode(const char *data,
                                     obj_ptr<Buffer_base> &retVal)
{
    static const char decodeTable[] =
    {
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

result_t encoding_base::hexEncode(Buffer_base *data, std::string &retVal)
{
    std::string strData;
    static char HexChar[] = "0123456789abcdef";
    int i, pos, len1;

    data->toString(strData);

    i = (int) strData.length() * 2;
    retVal.resize(i);

    len1 = 0;
    pos = 0;

    for (i = 0; i < (int) strData.length(); i++)
    {
        retVal[pos * 2] = HexChar[(unsigned char) strData[i] >> 4];
        retVal[pos * 2 + 1] = HexChar[(unsigned char) strData[i] & 0xf];
        pos++;
        len1 += 2;
    }

    return 0;
}

result_t encoding_base::hexDecode(const char *data,
                                  obj_ptr<Buffer_base> &retVal)
{
    int pos, len = (int) qstrlen(data);
    const char *end = data + len;
    std::string strBuf;
    uint32_t ch1, ch2;

    strBuf.resize(len / 2);

    pos = 0;
    while ((ch1 = utf8_getchar(data, end)) != 0)
    {
        if (qisxdigit(ch1))
            ch1 = qhex(ch1);
        else
            continue;

        ch2 = utf8_getchar(data, end);
        if (ch2 == 0)
            break;

        if (qisxdigit(ch2))
            ch2 = qhex(ch2);
        else
        {
            ch2 = ch1;
            ch1 = 0;
        }

        strBuf[pos++] = (ch1 << 4) + ch2;
    }

    retVal = new Buffer(strBuf);

    return 0;
}

result_t encoding_base::iconvEncode(const char *charset, const char *data,
                                    obj_ptr<Buffer_base> &retVal)
{
    return encoding_iconv(charset).encode(data, retVal);
}

result_t encoding_base::iconvDecode(const char *charset, Buffer_base *data,
                                    std::string &retVal)
{
    return encoding_iconv(charset).decode(data, retVal);
}

result_t encoding_base::jsstr(const char *str, std::string &retVal)
{
    const char *p;
    char *p1;
    int len;
    char ch;
    std::string s;

    if (!*str)
        return 0;

    for (len = 0, p = str; (ch = *p) != 0; p++, len++)
        if (ch == '\\' || ch == '\r' || ch == '\n' || ch == '\t' || ch == '\''
                || ch == '\"')
            len++;

    s.resize(len);

    for (p1 = &s[0], p = str; (ch = *p) != 0; p++)
        switch (ch)
        {
        case '\\':
            *p1++ = '\\';
            *p1++ = '\\';
            break;
        case '\r':
            *p1++ = '\\';
            *p1++ = 'r';
            break;
        case '\n':
            *p1++ = '\\';
            *p1++ = 'n';
            break;
        case '\t':
            *p1++ = '\\';
            *p1++ = 't';
            break;
        case '\'':
            *p1++ = '\\';
            *p1++ = '\'';
            break;
        case '\"':
            *p1++ = '\\';
            *p1++ = '\"';
            break;
        default:
            *p1++ = ch;
            break;
        }

    retVal = s;
    return 0;
}

static const char *URITable =
    " ! #$ &'()*+,-./0123456789:; = ?@ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";
static const char *URIComponentTable =
    " !     '()*  -. 0123456789       ABCDEFGHIJKLMNOPQRSTUVWXYZ    _ abcdefghijklmnopqrstuvwxyz   ~ ";

result_t encoding_base::encodeURI(const char *url, std::string &retVal)
{
    Url::encodeURI(url, -1, retVal, URITable);
    return 0;
}

result_t encoding_base::encodeURIComponent(const char *url, std::string &retVal)
{
    Url::encodeURI(url, -1, retVal, URIComponentTable);
    return 0;
}

result_t encoding_base::decodeURI(const char *url, std::string &retVal)
{
    Url::decodeURI(url, -1, retVal);
    return 0;
}

static v8::Persistent<v8::Object> s_json;
static v8::Persistent<v8::Function> s_stringify;

inline void initJSON()
{
    if (s_json.IsEmpty())
    {
        Isolate &isolate = Isolate::now();
        v8::Local<v8::Object> glob = v8::Local<v8::Object>::New(isolate.isolate, isolate.s_global);
        s_json.Reset(isolate.isolate, glob->Get(v8::String::NewFromUtf8(isolate.isolate, "JSON"))->ToObject());
    }
}

result_t encoding_base::jsonEncode(v8::Local<v8::Value> data,
                                   std::string &retVal)
{
    initJSON();

    Isolate &isolate = Isolate::now();
    v8::Local<v8::Object> _json = v8::Local<v8::Object>::New(isolate.isolate, s_json);

    if (s_stringify.IsEmpty())
        s_stringify.Reset(isolate.isolate,
                          v8::Local<v8::Function>::Cast(_json->Get(v8::String::NewFromUtf8(isolate.isolate, "stringify"))));

    TryCatch try_catch;
    v8::Local<v8::Value> str = v8::Local<v8::Function>::New(isolate.isolate, s_stringify)->Call(_json, 1, &data);
    if (try_catch.HasCaught())
        return CHECK_ERROR(Runtime::setError(*v8::String::Utf8Value(try_catch.Exception())));

    v8::String::Utf8Value v(str);
    retVal.assign(*v, v.length());

    return 0;
}

result_t encoding_base::jsonDecode(const char *data,
                                   v8::Local<v8::Value> &retVal)
{
    TryCatch try_catch;
    retVal = v8::JSON::Parse(v8::String::NewFromUtf8(Isolate::now().isolate, data));
    if (try_catch.HasCaught())
        return CHECK_ERROR(Runtime::setError(*v8::String::Utf8Value(try_catch.Exception())));

    return 0;
}

}

