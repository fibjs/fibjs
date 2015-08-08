/*
 * X509Crl.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ssl.h"
#include "X509Crl.h"
#include <mbedtls/polarssl/pem.h>

namespace fibjs
{

result_t X509Crl_base::_new(obj_ptr<X509Crl_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new X509Crl();
    return 0;
}

X509Crl::X509Crl()
{
    x509_crl_init(&m_crl);
}

X509Crl::~X509Crl()
{
    x509_crl_free(&m_crl);
}

result_t X509Crl::load(Buffer_base *derCrl)
{
    int32_t ret;

    std::string crl;
    derCrl->toString(crl);

    ret = x509_crl_parse(&m_crl, (const unsigned char *)crl.c_str(),
                         crl.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Crl::load(const char *pemCrl)
{
    int32_t ret;

    ret = x509_crl_parse(&m_crl, (const unsigned char *)pemCrl,
                         qstrlen(pemCrl));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Crl::loadFile(const char *filename)
{
    result_t hr;
    std::string data;
    int32_t ret;

    hr = fs_base::ac_readFile(filename, data);
    if (hr < 0)
        return hr;

    ret = x509_crl_parse(&m_crl, (const unsigned char *)data.c_str(),
                         data.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

#define PEM_BEGIN_CRL           "-----BEGIN X509 CRL-----\n"
#define PEM_END_CRL             "-----END X509 CRL-----\n"

result_t X509Crl::dump(v8::Local<v8::Array> &retVal)
{
    Isolate* isolate = Isolate::now();
    retVal = v8::Array::New(isolate->m_isolate);

    const x509_crl *pCrl = &m_crl;
    int32_t ret, n = 0;
    std::string buf;
    size_t olen;

    while (pCrl)
    {
        if (pCrl->raw.len > 0)
        {
            buf.resize(pCrl->raw.len * 2 + 64);
            ret = pem_write_buffer(PEM_BEGIN_CRL, PEM_END_CRL,
                                   pCrl->raw.p, pCrl->raw.len,
                                   (unsigned char *)&buf[0], buf.length(), &olen);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));

            retVal->Set(n ++, v8::String::NewFromUtf8(isolate->m_isolate, buf.c_str(),
                        v8::String::kNormalString, (int32_t) olen - 1));
        }
        pCrl = pCrl->next;
    }

    return 0;
}

result_t X509Crl::clear()
{
    x509_crl_free(&m_crl);
    x509_crl_init(&m_crl);
    return 0;
}

}
