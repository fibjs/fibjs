/*
 * X509Req.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ssl.h"
#include "X509Req.h"
#include "PKey.h"
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

    std::string buf;
    buf.resize(8192);

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

    buf.resize(olen - 1);
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

result_t X509Req::create(const char *subject, PKey_base *key, int32_t hash)
{
    clear();

    x509write_csr csr;
    int ret;

    x509write_csr_init(&csr);

    if (hash < POLARSSL_MD_MD2 || hash > POLARSSL_MD_RIPEMD160)
        return CALL_E_INVALIDARG;

    x509write_csr_set_md_alg(&csr, (md_type_t)hash);
    x509write_csr_set_subject_name(&csr, subject);

    pk_context *k = &((PKey *)(PKey_base *)key)->m_key;
    x509write_csr_set_key(&csr, k);

    std::string buf;
    buf.resize(pk_get_size(k) * 8 + 128);

    ret = x509write_csr_pem(&csr, (unsigned char *)&buf[0], buf.length(),
                            ctr_drbg_random, &g_ssl.ctr_drbg);
    x509write_csr_free(&csr);
    if (ret != 0)
        return _ssl::setError(ret);

    ret = x509_csr_parse(&m_csr, (const unsigned char *)buf.c_str(), qstrlen(buf.c_str()));
    if (ret != 0)
        return _ssl::setError(ret);

    return 0;
}

result_t X509Req::get_subject(std::string &retVal)
{
    int ret;
    std::string buf;

    buf.resize(1024);

    ret = x509_dn_gets(&buf[0], buf.length(), &m_csr.subject);
    if (ret < 0)
        return _ssl::setError(ret);

    buf.resize(ret);
    retVal = buf;

    return 0;
}

result_t X509Req::get_publicKey(obj_ptr<PKey_base> &retVal)
{
    obj_ptr<PKey> pk1 = new PKey();
    result_t hr;

    hr = pk1->copy(m_csr.pk);
    if (hr < 0)
        return 0;

    retVal = pk1;
    return 0;
}

}
