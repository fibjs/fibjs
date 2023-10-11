/*
 * encoding_iconv.cpp
 *
 *  Created on: Jul 13, 2014
 *      Author: lion
 */

#ifdef _WIN32
#include <winiconv/win_iconv.h>
#else

#include "config.h"

#include <stddef.h>

typedef void* iconv_t;

#ifdef HAVE_ICONV_H
extern "C" {
iconv_t iconv_open(const char* tocode, const char* fromcode);
int iconv_close(iconv_t cd);
size_t iconv(iconv_t cd, const char** inbuf, size_t* inbytesleft,
    char** outbuf, size_t* outbytesleft);
}
#else
static iconv_t iconv_open(const char* tocode, const char* fromcode)
{
    return (iconv_t)-1;
}

static int iconv_close(iconv_t cd)
{
    return 0;
}

static size_t iconv(iconv_t cd, const char** inbuf, size_t* inbytesleft,
    char** outbuf, size_t* outbytesleft)
{
    return 0;
}
#endif

#include <dlfcn.h>

#endif

#include "object.h"
#include "encoding_iconv.h"
#include "ifs/encoding.h"
#include <unicode/include/unicode/ucnv.h>
namespace fibjs {

DECLARE_MODULE(iconv);

encoding_iconv::encoding_iconv()
    : m_iconv_en(NULL)
    , m_iconv_de(NULL)
{
    m_charset = "utf-8";
}

encoding_iconv::encoding_iconv(exlib::string charset)
    : m_iconv_en(NULL)
    , m_iconv_de(NULL)
{
    m_charset = (charset == "gb2312") ? "gbk" : charset;
}

encoding_iconv::~encoding_iconv()
{
    if (m_iconv_en)
        iconv_close((iconv_t)m_iconv_en);

    if (m_iconv_de)
        iconv_close((iconv_t)m_iconv_de);
}

void encoding_iconv::open(const char* charset)
{
    if (m_iconv_en) {
        iconv_close((iconv_t)m_iconv_en);
        m_iconv_en = NULL;
    }

    if (m_iconv_de) {
        iconv_close((iconv_t)m_iconv_de);
        m_iconv_de = NULL;
    }

    m_charset = charset;
}

result_t encoding_iconv::encode(exlib::string data, exlib::string& retVal)
{
    if (data.empty()) {
        retVal.clear();
        return 0;
    }

    if (ucs_encode(data, retVal) == 0)
        return 0;

    int32_t _sz;
    UErrorCode errorCode = U_ZERO_ERROR;

    _sz = ucnv_convert(m_charset.c_str(), "utf-8", NULL, 0, data.c_str(), data.length(), &errorCode);
    if (_sz) {
        retVal.resize(_sz);
        errorCode = U_ZERO_ERROR;
        ucnv_convert(m_charset.c_str(), "utf-8", retVal.c_buffer(), _sz, data.c_str(), data.length(), &errorCode);
        return 0;
    }

    if (!m_iconv_en) {
        m_iconv_en = iconv_open(m_charset.c_str(), "utf-8");
        if (m_iconv_en == (iconv_t)(-1)) {
            m_iconv_en = NULL;
            return CHECK_ERROR(Runtime::setError("encoding: Unknown charset."));
        }
    }

    const char* _data = data.c_str();
    size_t sz = data.length();

    retVal.resize(sz * 2);
    char* output_buf = retVal.c_buffer();
    size_t output_size = retVal.length();

    size_t n = iconv((iconv_t)m_iconv_en, &_data, &sz, &output_buf, &output_size);

    if (n == (size_t)-1)
        return CHECK_ERROR(Runtime::setError("encoding: convert error."));

    retVal.resize(retVal.length() - output_size);

    return 0;
}

result_t encoding_iconv::encode(exlib::string data, obj_ptr<Buffer_base>& retVal)
{
    exlib::string strBuf;

    result_t hr = encode(data, strBuf);
    if (hr < 0)
        return hr;

    retVal = new Buffer(strBuf.c_str(), strBuf.length());

    return 0;
}

result_t encoding_iconv::decode(const char* data, size_t sz, exlib::string& retVal)
{
    if (sz == 0) {
        retVal.clear();
        return 0;
    }

    if (ucs_decode(data, sz, retVal) == 0)
        return 0;

    if (!m_iconv_de) {
        m_iconv_de = iconv_open("utf-8", m_charset.c_str());
        if (m_iconv_de == (iconv_t)(-1)) {
            m_iconv_de = NULL;
            return CHECK_ERROR(Runtime::setError("encoding: Unknown charset."));
        }
    }

    int32_t _sz;
    UErrorCode errorCode = U_ZERO_ERROR;

    _sz = ucnv_convert("utf-8", m_charset.c_str(), NULL, 0, data, sz, &errorCode);
    if (_sz) {
        retVal.resize(_sz);
        errorCode = U_ZERO_ERROR;
        ucnv_convert("utf-8", m_charset.c_str(), retVal.c_buffer(), _sz, data, sz, &errorCode);
        return 0;
    }

    exlib::string strBuf;

    strBuf.resize(sz * 2);
    char* output_buf = strBuf.c_buffer();
    size_t output_size = strBuf.length();

    size_t n = iconv((iconv_t)m_iconv_de, &data, &sz, &output_buf, &output_size);

    if (n == (size_t)-1)
        return CHECK_ERROR(Runtime::setError("encoding: convert error."));

    strBuf.resize(strBuf.length() - output_size);

    retVal = strBuf;

    return 0;
}

result_t encoding_iconv::decode(const exlib::string& data, exlib::string& retVal)
{
    if (ucs_decode(data, retVal) == 0)
        return 0;

    if (!m_iconv_de) {
        m_iconv_de = iconv_open("utf-8", m_charset.c_str());
        if (m_iconv_de == (iconv_t)(-1)) {
            m_iconv_de = NULL;
            return CHECK_ERROR(Runtime::setError("encoding: Unknown charset."));
        }
    }

    size_t sz = data.length();
    const char* ptr = data.c_str();
    exlib::string strBuf;

    strBuf.resize(sz * 2);
    char* output_buf = strBuf.c_buffer();
    size_t output_size = strBuf.length();

    size_t n = iconv((iconv_t)m_iconv_de, &ptr, &sz, &output_buf, &output_size);

    if (n == (size_t)-1)
        return CHECK_ERROR(Runtime::setError("encoding: convert error."));

    strBuf.resize(strBuf.length() - output_size);

    retVal = strBuf;

    return 0;
}

result_t encoding_iconv::decode(Buffer_base* data, exlib::string& retVal)
{
    exlib::string strData;
    data->toString(strData);

    return decode(strData, retVal);
}

bool encoding_iconv::is_encoding(exlib::string charset)
{
    if (is_ucs_encoding(charset))
        return true;

    UErrorCode err = U_ZERO_ERROR;
    UConverter* icu_ec = ucnv_open(charset.c_str(), &err);
    if (icu_ec) {
        ucnv_close(icu_ec);
        return true;
    }

    void* iconv_ec = iconv_open(charset.c_str(), "utf-8");
    if (iconv_ec != (iconv_t)(-1)) {
        iconv_close((iconv_t)iconv_ec);
        return true;
    }

    return false;
}

result_t iconv_base::encode(exlib::string charset, exlib::string data,
    obj_ptr<Buffer_base>& retVal)
{
    return encoding_iconv(charset).encode(data, retVal);
}

result_t iconv_base::decode(exlib::string charset, Buffer_base* data,
    exlib::string& retVal)
{
    return encoding_iconv(charset).decode(data, retVal);
}

result_t iconv_base::isEncoding(exlib::string charset, bool& retVal)
{
    retVal = encoding_iconv::is_encoding(charset);
    return 0;
}
}
