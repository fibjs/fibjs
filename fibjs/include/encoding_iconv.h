/*
 * encoding_iconv.h
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#pragma once

#include "Buffer.h"

namespace fibjs {

class encoding_iconv {
public:
    encoding_iconv();
    encoding_iconv(exlib::string charset);
    ~encoding_iconv();

    void open(const char* charset);

    result_t ucs_encode(exlib::string data, exlib::string& retVal);
    result_t encode(exlib::string data, exlib::string& retVal);
    result_t encode(exlib::string data, obj_ptr<Buffer_base>& retVal);

    result_t ucs_decode(exlib::string data, exlib::string& retVal);
    result_t decode(const exlib::string& data, exlib::string& retVal);
    result_t decode(Buffer_base* data, exlib::string& retVal);

    static bool is_ucs_encoding(exlib::string charset);
    static bool is_encoding(exlib::string charset);

    exlib::string charset() const
    {
        return m_charset;
    }

private:
    void* m_iconv_en;
    void* m_iconv_de;
    exlib::string m_charset;
};

} /* namespace fibjs */
