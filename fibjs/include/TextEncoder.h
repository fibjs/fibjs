/*
 * TextEncoder.h
 *
 *  Created on: Jun 2, 2022
 *      Author: lion
 */

#pragma once

#include "ifs/TextEncoder.h"
#include "ifs/TextDecoder.h"

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
    virtual result_t get_encoding(exlib::string& retVal);

private:
    exlib::string m_codec;
};

class TextDecoder : public TextDecoder_base {
public:
    TextDecoder(exlib::string codec)
        : m_codec(codec)
    {
    }

public:
    // TextDecoder_base
    virtual result_t decode(Buffer_base* data, v8::Local<v8::Object> opts, exlib::string& retVal);
    virtual result_t get_encoding(exlib::string& retVal);

private:
    exlib::string m_codec;
};

} /* namespace fibjs */
