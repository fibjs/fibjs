/*
 * TextEncoder.h
 *
 *  Created on: Jun 2, 2022
 *      Author: lion
 */

#pragma once

#include "ifs/TextEncoder.h"
#include "ifs/TextDecoder.h"
#include <unicode/include/unicode/ucnv.h>

namespace fibjs {

class TextEncoder : public TextEncoder_base {
public:
    TextEncoder(exlib::string codec)
        : m_codec(codec)
    {
    }

public:
    // TextEncoder_base
    virtual result_t encode(exlib::string data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal);
    virtual result_t encodeInto(exlib::string source, Buffer_base* destination, v8::Local<v8::Object>& retVal);
    virtual result_t get_encoding(exlib::string& retVal);

private:
    exlib::string m_codec;
};

class TextDecoder : public TextDecoder_base {
public:
    TextDecoder(exlib::string codec, bool fatal, bool ignoreBOM)
        : m_codec(codec)
        , m_fatal(fatal)
        , m_ignoreBOM(ignoreBOM)
        , m_bomSeen(false)
        , m_cnv(nullptr)
    {
    }

    ~TextDecoder()
    {
        if (m_cnv)
            ucnv_close(m_cnv);
    }

public:
    // TextDecoder_base
    virtual result_t decode(Buffer_base* data, v8::Local<v8::Object> opts, exlib::string& retVal);
    virtual result_t decode(exlib::string& retVal);
    virtual result_t get_encoding(exlib::string& retVal);
    virtual result_t get_fatal(bool& retVal);
    virtual result_t get_ignoreBOM(bool& retVal);

private:
    result_t ensureConverter();

private:
    exlib::string m_codec;
    bool m_fatal;
    bool m_ignoreBOM;
    bool m_bomSeen;
    UConverter* m_cnv;
};

} /* namespace fibjs */
