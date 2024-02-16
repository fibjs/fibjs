/*
 * StringDecoder.h
 *
 *  Created on: Aug 29, 2017
 *      Author: ngot
 */

#pragma once

#include "ifs/StringDecoder.h"
#include "Buffer.h"

namespace fibjs {

class StringDecoder;

typedef result_t (StringDecoder::*Write)(Buffer_base* buf, exlib::string& retVal);
typedef result_t (StringDecoder::*Text)(Buffer_base* buf, int32_t offset, exlib::string& retVal);
typedef result_t (StringDecoder::*End1)(exlib::string& retVal);
typedef result_t (StringDecoder::*End2)(Buffer_base* buf, exlib::string& retVal);
typedef result_t (StringDecoder::*FillLast)(Buffer_base* buf, exlib::string& retVal);

class StringDecoder : public StringDecoder_base {
public:
    StringDecoder(exlib::string encoding)
        : m_encoding(encoding)
        , m_lastNeed(0)
        , m_lastTotal(0)
    {
        int32_t size = 0;
        if (m_encoding == "utf16le") {
            size = 4;
            m_write = &StringDecoder::defaultWrite;
            m_end1 = &StringDecoder::utf16End1;
            m_end2 = &StringDecoder::utf16End2;
            m_text = &StringDecoder::utf16Text;
            m_fillLast = &StringDecoder::defaultFillLast;
        } else if (m_encoding == "utf8") {
            size = 4;
            m_write = &StringDecoder::defaultWrite;
            m_end1 = &StringDecoder::utf8End1;
            m_end2 = &StringDecoder::utf8End2;
            m_text = &StringDecoder::utf8Text;
            m_fillLast = &StringDecoder::utf8FillLast;
        } else if (m_encoding == "base64") {
            size = 3;
            m_write = &StringDecoder::defaultWrite;
            m_end1 = &StringDecoder::base64End1;
            m_end2 = &StringDecoder::base64End2;
            m_fillLast = &StringDecoder::defaultFillLast;
            m_text = &StringDecoder::base64Text;
        } else {
            m_write = &StringDecoder::simpleWrite;
            m_end1 = &StringDecoder::simpleEnd1;
            m_end2 = &StringDecoder::simpleEnd2;
            m_fillLast = &StringDecoder::defaultFillLast;
            m_text = &StringDecoder::utf8Text;
        }

        m_lastChar = new Buffer(NULL, size);
    }

public:
    virtual result_t end(exlib::string& retVal);
    virtual result_t end(Buffer_base* buf, exlib::string& retVal);
    virtual result_t write(Buffer_base* buf, exlib::string& retVal);
    virtual result_t text(Buffer_base* buf, int32_t i, exlib::string& retVal);
    virtual result_t fillLast(Buffer_base* buf, exlib::string& retVal);
    virtual result_t get_lastNeed(int32_t& retVal);
    virtual result_t set_lastNeed(int32_t newVal);
    virtual result_t get_lastTotal(int32_t& retVal);
    virtual result_t set_lastTotal(int32_t newVal);
    virtual result_t get_lastChar(obj_ptr<Buffer_base>& retVal);
    virtual result_t set_lastChar(Buffer_base* newVal);
    virtual result_t get_encoding(exlib::string& retVal);
    virtual result_t set_encoding(exlib::string newVal);

private:
    result_t simpleWrite(Buffer_base* buf, exlib::string& retVal);
    result_t defaultWrite(Buffer_base* buf, exlib::string& retVal);
    result_t utf16End1(exlib::string& retVal);
    result_t utf16End2(Buffer_base* buf, exlib::string& retVal);
    result_t base64End1(exlib::string& retVal);
    result_t base64End2(Buffer_base* buf, exlib::string& retVal);
    result_t simpleEnd1(exlib::string& retVal);
    result_t simpleEnd2(Buffer_base* buf, exlib::string& retVal);
    result_t utf8End1(exlib::string& retVal);
    result_t utf8End2(Buffer_base* buf, exlib::string& retVal);
    result_t utf8FillLast(Buffer_base* buf, exlib::string& retVal);
    result_t defaultFillLast(Buffer_base* buf, exlib::string& retVal);
    result_t utf16Text(Buffer_base* buf, int32_t offset, exlib::string& retVal);
    result_t base64Text(Buffer_base* buf, int32_t offset, exlib::string& retVal);
    result_t utf8Text(Buffer_base* buf, int32_t offset, exlib::string& retVal);

private:
    exlib::string m_encoding;
    int32_t m_lastNeed;
    int32_t m_lastTotal;
    obj_ptr<Buffer_base> m_lastChar;

    Write m_write;
    Text m_text;
    End1 m_end1;
    End2 m_end2;
    FillLast m_fillLast;
};

inline int32_t utf8CheckByte(int32_t byte)
{
    if (byte <= 0x7F)
        return 0;
    else if (byte >> 5 == 0x06)
        return 2;
    else if (byte >> 4 == 0x0E)
        return 3;
    else if (byte >> 3 == 0x1E)
        return 4;
    return (byte >> 6 == 0x02 ? -1 : -2);
}

inline int32_t utf8CheckIncomplete(StringDecoder* decoder, Buffer_base* buf, int32_t i)
{
    obj_ptr<Buffer> data = Buffer::Cast(buf);
    int32_t bufLen = data->length();
    const uint8_t* _data = data->data();

    int32_t j = bufLen - 1;
    if (j < i)
        return 0;

    int32_t nb = utf8CheckByte(_data[j]);
    if (nb >= 0) {
        if (nb > 0)
            decoder->set_lastNeed(nb - 1);
        return nb;
    }
    if (--j < i || nb == -2)
        return 0;

    nb = utf8CheckByte(_data[j]);
    if (nb >= 0) {
        if (nb > 0)
            decoder->set_lastNeed(nb - 2);
        return nb;
    }
    if (--j < i || nb == -2)
        return 0;

    nb = utf8CheckByte(_data[j]);
    if (nb >= 0) {
        if (nb > 0) {
            if (nb == 2)
                nb = 0;
            else
                decoder->set_lastNeed(nb - 3);
        }
        return nb;
    }
    return 0;
}

inline exlib::string normalizeEncoding(exlib::string encoding)
{
    bool retried = false;
    while (true) {
        if (encoding == "utf8" || encoding == "utf-8") {
            return "utf8";
        } else if (encoding == "ucs2"
            || encoding == "ucs-2"
            || encoding == "utf16le"
            || encoding == "utf-16le") {
            return "utf16le";
        } else if (encoding == "latin1"
            || encoding == "binary") {
            return "latin1";
        } else if (encoding == "base64"
            || encoding == "ascii"
            || encoding == "hex") {
            return encoding;
        } else {
            if (retried)
                return "$not valid$";
            exlib::qstrlwr(encoding);
            retried = true;
        }
    }
}

} /* namespace fibjs */
