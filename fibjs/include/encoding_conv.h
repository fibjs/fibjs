/*
 * encoding_conv.h
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#pragma once

#include "Buffer.h"

namespace fibjs {

class encoding_conv {
public:
    encoding_conv();
    encoding_conv(exlib::string charset);
    ~encoding_conv();

    void open(const char* charset);

    result_t ucs_encode(exlib::string data, exlib::string& retVal);
    result_t encode(exlib::string data, exlib::string& retVal);
    result_t encode(exlib::string data, obj_ptr<Buffer_base>& retVal);

    result_t ucs_decode(const char* data, size_t sz, exlib::string& retVal);
    result_t ucs_decode(exlib::string data, exlib::string& retVal);
    result_t decode(const char* data, size_t sz, exlib::string& retVal);
    result_t decode(const exlib::string& data, exlib::string& retVal);
    result_t decode(Buffer_base* data, exlib::string& retVal);

    static bool is_ucs_encoding(exlib::string charset);
    static bool is_encoding(exlib::string charset);

    // WHATWG encoding label normalization, returns canonical name or nullptr
    static const char* normalizeEncoding(const exlib::string& label);

    exlib::string charset() const
    {
        return m_charset;
    }

private:
    exlib::string m_charset;
};

} /* namespace fibjs */
