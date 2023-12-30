/*
 * StringDecoder.cpp
 *
 *  Created on: Aug 29, 2017
 *      Author: ngot
 */

#include "StringDecoder.h"
#include "ifs/string_decoder.h"

namespace fibjs {

DECLARE_MODULE(string_decoder);

result_t StringDecoder_base::_new(exlib::string encoding,
    obj_ptr<StringDecoder_base>& retVal, v8::Local<v8::Object> This)
{
    exlib::string new_encoding = normalizeEncoding(encoding);
    bool isValid = false;
    Buffer_base::isEncoding(new_encoding, isValid);
    if (!isValid) {
        return CHECK_ERROR(Runtime::setError("string_decoder: Unknown encoding: " + encoding));
    }
    retVal = new StringDecoder(new_encoding);
    return 0;
}

result_t StringDecoder::end(exlib::string& retVal)
{
    return (this->*m_end1)(retVal);
}

result_t StringDecoder::end(Buffer_base* buf, exlib::string& retVal)
{
    return (this->*m_end2)(buf, retVal);
}

result_t StringDecoder::write(Buffer_base* buf, exlib::string& retVal)
{
    return (this->*m_write)(buf, retVal);
}

result_t StringDecoder::text(Buffer_base* buf, int32_t i, exlib::string& retVal)
{
    return (this->*m_text)(buf, i, retVal);
}

result_t StringDecoder::fillLast(Buffer_base* buf, exlib::string& retVal)
{
    return (this->*m_fillLast)(buf, retVal);
}

result_t StringDecoder::get_lastNeed(int32_t& retVal)
{
    retVal = this->m_lastNeed;
    return 0;
}

result_t StringDecoder::set_lastNeed(int32_t newVal)
{
    this->m_lastNeed = newVal;
    return 0;
}

result_t StringDecoder::get_lastTotal(int32_t& retVal)
{
    retVal = this->m_lastTotal;
    return 0;
}

result_t StringDecoder::set_lastTotal(int32_t newVal)
{
    this->m_lastTotal = newVal;
    return 0;
}

result_t StringDecoder::get_lastChar(obj_ptr<Buffer_base>& retVal)
{
    retVal = this->m_lastChar;
    return 0;
}

result_t StringDecoder::set_lastChar(Buffer_base* newVal)
{
    this->m_lastChar = newVal;
    return 0;
}

result_t StringDecoder::get_encoding(exlib::string& retVal)
{
    retVal = m_encoding;
    return 0;
}

result_t StringDecoder::set_encoding(exlib::string newVal)
{
    m_encoding = newVal;
    return 0;
}

result_t StringDecoder::utf16Text(Buffer_base* buf, int32_t offset, exlib::string& retVal)
{
    uint8_t* chars_data = Buffer::Cast(m_lastChar)->data();

    Buffer* buff = Buffer::Cast(buf);
    const uint8_t* buff_data = buff->data();
    char16_t* ws = (char16_t*)(buff_data + offset);

    int32_t bufLen = buff->length();
    int32_t len = (bufLen - offset) / 2;

    if ((bufLen - offset) % 2 == 0) {
        if (len > 0) {
            int32_t c = ws[len - 1];
            if (c >= 0xD800 && c <= 0xDBFF) {
                m_lastNeed = 2;
                m_lastTotal = 4;

                chars_data[0] = buff_data[bufLen - 2];
                chars_data[1] = buff_data[bufLen - 1];

                retVal = utf16to8String(ws, len - 1);
                return 0;
            }
        }
        retVal = utf16to8String(ws, len);
        return 0;
    }

    m_lastNeed = 1;
    m_lastTotal = 2;

    chars_data[0] = buff_data[bufLen - 1];
    retVal = utf16to8String(ws, len);

    return 0;
}

result_t StringDecoder::utf16End1(exlib::string& retVal)
{
    if (m_lastNeed != 0) {
        int32_t end = m_lastTotal - m_lastNeed;
        m_lastChar->toString("utf16le", 0, end, retVal);
    } else
        retVal = "";
    return 0;
}

result_t StringDecoder::utf16End2(Buffer_base* buf, exlib::string& retVal)
{
    exlib::string t;
    write(buf, t);
    return utf16End1(retVal);
}

result_t StringDecoder::utf8FillLast(Buffer_base* buf, exlib::string& retVal)
{
    Buffer* buff = Buffer::Cast(buf);
    const uint8_t* buff_data = buff->data();

    int32_t b;
    int32_t bufLen = buff->length();
    int32_t len;

    b = buff_data[0];
    int32_t p = m_lastTotal - m_lastNeed;

    if ((b & 0xC0) != 0x80) {
        m_lastNeed = 0;
        retVal = "\xef\xbf\xbd";
        return 0;
    } else if (m_lastNeed > 1 && bufLen > 1) {
        b = buff_data[1];
        if ((b & 0xC0) != 0x80) {
            m_lastNeed = 1;
            retVal = "\xef\xbf\xbd";
            return 0;
        } else if (m_lastNeed > 2 && bufLen > 2) {
            b = buff_data[2];
            if ((b & 0xC0) != 0x80) {
                m_lastNeed = 2;
                retVal = "\xef\xbf\xbd";
                return 0;
            }
        }
    }

    if (bufLen >= m_lastNeed) {
        buf->copy(m_lastChar, p, 0, m_lastNeed, len);
        m_lastChar->toString(m_encoding, 0, m_lastTotal, retVal);
        return 0;
    }
    buf->copy(m_lastChar, p, 0, bufLen, len);
    m_lastNeed -= bufLen;
    return CALL_RETURN_NULL;
}

result_t StringDecoder::defaultFillLast(Buffer_base* buf, exlib::string& retVal)
{
    int32_t bufLen = Buffer::Cast(buf)->length();
    int32_t len;

    if (bufLen >= m_lastNeed) {
        buf->copy(m_lastChar, m_lastTotal - m_lastNeed, 0, m_lastNeed, len);
        m_lastChar->toString(m_encoding, 0, m_lastTotal, retVal);
        return 0;
    }
    buf->copy(m_lastChar, m_lastTotal - m_lastNeed, 0, bufLen, len);
    m_lastNeed -= bufLen;
    return CALL_RETURN_NULL;
}

result_t StringDecoder::base64Text(Buffer_base* buf, int32_t offset, exlib::string& retVal)
{
    Buffer* buff = Buffer::Cast(buf);
    const uint8_t* buff_data = buff->data();
    uint8_t* chars_data = Buffer::Cast(m_lastChar)->data();

    int32_t bufLen = buff->length();
    int32_t n = (bufLen - offset) % 3;

    if (n == 0) {
        buf->toString("base64", offset, retVal);
        return 0;
    }
    m_lastNeed = 3 - n;
    m_lastTotal = 3;

    if (n == 1) {
        chars_data[0] = buff_data[bufLen - 1];
    } else {
        chars_data[0] = buff_data[bufLen - 2];
        chars_data[1] = buff_data[bufLen - 1];
    }

    buf->toString("base64", offset, bufLen - n, retVal);
    return 0;
}

result_t StringDecoder::utf8Text(Buffer_base* buf, int32_t offset, exlib::string& retVal)
{
    int32_t total = utf8CheckIncomplete(this, buf, offset);
    if (m_lastNeed == 0) {
        buf->toString("utf8", offset, retVal);
        return 0;
    }

    m_lastTotal = total;

    int32_t bufLen = Buffer::Cast(buf)->length();
    int32_t len;

    int32_t end = bufLen - (total - m_lastNeed);
    buf->copy(m_lastChar, 0, end, -1, len);
    buf->toString("utf8", offset, end, retVal);
    return 0;
}

result_t StringDecoder::base64End1(exlib::string& retVal)
{
    if (m_lastNeed != 0)
        m_lastChar->toString("base64", 0, 3 - m_lastNeed, retVal);
    else
        retVal = "";
    return 0;
}

result_t StringDecoder::base64End2(Buffer_base* buf, exlib::string& retVal)
{
    exlib::string t;
    write(buf, t);
    return base64End1(retVal);
}

result_t StringDecoder::simpleWrite(Buffer_base* buf, exlib::string& retVal)
{
    buf->toString(this->m_encoding, 0, retVal);
    return 0;
}

result_t StringDecoder::defaultWrite(Buffer_base* buf, exlib::string& retVal)
{
    int32_t bufLen = Buffer::Cast(buf)->length();
    int32_t hr = 0;
    int32_t i;
    exlib::string r = "";

    if (bufLen == 0) {
        retVal = "";
        return 0;
    }

    if (m_lastNeed != 0) {
        hr = fillLast(buf, r);
        if (hr == CALL_RETURN_NULL) {
            retVal = "";
            return 0;
        }
        i = m_lastNeed;
        m_lastNeed = 0;
    } else {
        i = 0;
    }

    if (i < bufLen) {
        exlib::string tmp;
        text(buf, i, tmp);
        if (r.length() > 0)
            retVal = r + tmp;
        else
            retVal = tmp;
        return 0;
    }

    if (r.length() > 0)
        retVal = r;
    else
        retVal = "";
    return 0;
}

result_t StringDecoder::simpleEnd1(exlib::string& retVal)
{
    retVal = "";
    return 0;
}

result_t StringDecoder::simpleEnd2(Buffer_base* buf, exlib::string& retVal) // ??
{
    exlib::string t;
    write(buf, t);
    return simpleEnd1(retVal);
}

result_t StringDecoder::utf8End1(exlib::string& retVal)
{
    if (m_lastNeed != 0)
        retVal = "\xef\xbf\xbd";
    else
        retVal = "";
    return 0;
}

result_t StringDecoder::utf8End2(Buffer_base* buf, exlib::string& retVal)
{
    exlib::string t;
    write(buf, t);
    return utf8End1(retVal);
}

} /* namespace fibjs */
