/*
 * X509Crl.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/fs.h"
#include "ssl.h"
#include "X509Crl.h"
#include "Buffer.h"
#include <mbedtls/mbedtls/pem.h>

namespace fibjs {

result_t X509Crl_base::_new(obj_ptr<X509Crl_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new X509Crl();
    return 0;
}

result_t X509Crl_base::_new(Buffer_base* derCrl, obj_ptr<X509Crl_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<X509Crl> crl = new X509Crl();
    result_t hr = crl->load(derCrl);
    if (hr < 0)
        return hr;

    retVal = crl;
    return 0;
}

result_t X509Crl_base::_new(exlib::string pemCrl, obj_ptr<X509Crl_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<X509Crl> crl = new X509Crl();
    result_t hr = crl->load(pemCrl);
    if (hr < 0)
        return hr;

    retVal = crl;
    return 0;
}

X509Crl::X509Crl()
{
    mbedtls_x509_crl_init(&m_crl);
}

X509Crl::~X509Crl()
{
    mbedtls_x509_crl_free(&m_crl);
}

result_t X509Crl::load(Buffer_base* derCrl)
{
    int32_t ret;

    exlib::string crl;
    derCrl->toString(crl);

    ret = mbedtls_x509_crl_parse(&m_crl, (const unsigned char*)crl.c_str(),
        crl.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Crl::load(exlib::string pemCrl)
{
    int32_t ret;

    ret = mbedtls_x509_crl_parse(&m_crl, (const unsigned char*)pemCrl.c_str(),
        pemCrl.length() + 1);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Crl::loadFile(exlib::string filename)
{
    result_t hr;
    exlib::string data;
    obj_ptr<Buffer> buf;

    hr = fs_base::ac_readTextFile(filename, data);
    if (hr < 0)
        return hr;

    if (qstrstr(data.c_str(), "BEGIN"))
        return load(data);

    buf = new Buffer(data);
    return load(buf);
}

#define PEM_BEGIN_CRL "-----BEGIN X509 CRL-----\n"
#define PEM_END_CRL "-----END X509 CRL-----\n"

result_t X509Crl::dump(bool pem, v8::Local<v8::Array>& retVal)
{
    Isolate* isolate = holder();
    retVal = v8::Array::New(isolate->m_isolate);

    const mbedtls_x509_crl* pCrl = &m_crl;
    int32_t ret, n = 0;
    exlib::string buf;
    size_t olen;

    while (pCrl) {
        if (pCrl->raw.len > 0) {
            if (pem) {
                buf.resize(pCrl->raw.len * 2 + 64);
                ret = mbedtls_pem_write_buffer(PEM_BEGIN_CRL, PEM_END_CRL,
                    pCrl->raw.p, pCrl->raw.len,
                    (unsigned char*)&buf[0], buf.length(), &olen);
                if (ret != 0)
                    return CHECK_ERROR(_ssl::setError(ret));

                retVal->Set(n++, isolate->NewString(buf.c_str(), (int32_t)olen - 1));
            } else {
                obj_ptr<Buffer> data = new Buffer(pCrl->raw.p, pCrl->raw.len);
                retVal->Set(n++, data->wrap());
            }
        }
        pCrl = pCrl->next;
    }

    return 0;
}

result_t X509Crl::clear()
{
    mbedtls_x509_crl_free(&m_crl);
    mbedtls_x509_crl_init(&m_crl);
    return 0;
}
}
