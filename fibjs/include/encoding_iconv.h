/*
 * encoding_iconv.h
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#include "Buffer.h"
#include "utf8.h"

#ifndef ENCODING_ICONV_H_
#define ENCODING_ICONV_H_

namespace fibjs {

class encoding_iconv {
public:
    encoding_iconv();
    encoding_iconv(exlib::string charset);
    ~encoding_iconv();

    void open(const char* charset);

    result_t encode(exlib::string data, exlib::string& retVal);
    result_t encode(exlib::string data, obj_ptr<Buffer_base>& retVal);

    result_t decode(const exlib::string& data, exlib::string& retVal);
    result_t decode(Buffer_base* data, exlib::string& retVal);

    result_t isEncoding(bool& retVal);

    exlib::string charset() const
    {
        return m_charset;
    }

private:
    void* m_iconv_en;
    void* m_iconv_de;
    void* m_iconv_ec;
    exlib::string m_charset;
};

} /* namespace fibjs */
#endif /* ENCODING_ICONV_H_ */
