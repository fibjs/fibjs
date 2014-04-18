/*
 * X509Req.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ssl.h"
#include "X509Req.h"
#include <polarssl/pem.h>
#include "Buffer.h"

namespace fibjs
{

result_t X509Req_base::_new(obj_ptr<X509Req_base> &retVal)
{
    retVal = new X509Req();
    return 0;
}

X509Req::X509Req()
{
    x509_csr_init(&m_csr);
}

X509Req::~X509Req()
{
    x509_csr_free(&m_csr);
}

void X509Req::clear()
{
    x509_csr_free(&m_csr);
    x509_csr_init(&m_csr);
}

result_t X509Req::load(Buffer_base *derReq)
{
    int ret;

    clear();

    std::string csr;
    derReq->toString(csr);

    ret = x509_csr_parse(&m_csr, (const unsigned char *)csr.c_str(),
                         csr.length());
    if (ret != 0)
        return _ssl::setError(ret);

    return 0;
}

result_t X509Req::load(const char *pemReq)
{
    int ret;

    clear();

    ret = x509_csr_parse(&m_csr, (const unsigned char *)pemReq,
                         qstrlen(pemReq));
    if (ret != 0)
        return _ssl::setError(ret);

    return 0;
}

result_t X509Req::loadFile(const char *filename)
{
    result_t hr;
    std::string data;
    int ret;

    clear();

    hr = fs_base::ac_readFile(filename, data);
    if (hr < 0)
        return hr;

    ret = x509_csr_parse(&m_csr, (const unsigned char *)data.c_str(),
                         data.length());
    if (ret != 0)
        return _ssl::setError(ret);

    return 0;
}

#define PEM_BEGIN_CSR           "-----BEGIN CERTIFICATE REQUEST-----\n"
#define PEM_END_CSR             "-----END CERTIFICATE REQUEST-----\n"

result_t X509Req::exportPem(std::string &retVal)
{
    if (m_csr.raw.len == 0)
        return CALL_E_INVALID_CALL;

    std::string buf;
    size_t olen;
    int ret;

    buf.resize(m_csr.raw.len * 2 + 64);
    ret = pem_write_buffer(PEM_BEGIN_CSR, PEM_END_CSR,
                           m_csr.raw.p, m_csr.raw.len,
                           (unsigned char *)&buf[0], buf.length(), &olen);
    if (ret != 0)
        return _ssl::setError(ret);

    buf.resize(olen);
    retVal = buf;

    return 0;
}

result_t X509Req::exportDer(obj_ptr<Buffer_base> &retVal)
{
    if (m_csr.raw.len == 0)
        return CALL_E_INVALID_CALL;

    retVal = new Buffer(m_csr.raw.p, m_csr.raw.len);

    return 0;
}

result_t X509Req::toString(std::string &retVal)
{
    return exportPem(retVal);
}

}
