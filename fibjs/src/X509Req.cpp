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

result_t X509Req::create(v8::Local<v8::Object> opts)
{
    clear();

    x509write_csr csr;
    v8::Local<v8::Value> v;
    int ret;

    x509write_csr_init(&csr);

    v = opts->Get(v8::String::NewFromUtf8(isolate, "hash",
                                          v8::String::kNormalString, 4));
    if (!IsEmpty(v))
    {
        int n = (int)v->NumberValue();
        if (n < POLARSSL_MD_MD2 || n > POLARSSL_MD_RIPEMD160)
            return CALL_E_INVALIDARG;

        x509write_csr_set_md_alg(&csr, (md_type_t)n);
    }
    else
        x509write_csr_set_md_alg(&csr, POLARSSL_MD_SHA1);

    v = opts->Get(v8::String::NewFromUtf8(isolate, "subject",
                                          v8::String::kNormalString, 7));
    if (IsEmpty(v))
        return CALL_E_INVALIDARG;

    v8::String::Utf8Value s(v);
    if (!*s)
        return CALL_E_INVALIDARG;

    x509write_csr_set_subject_name(&csr, *s);

    v = opts->Get(v8::String::NewFromUtf8(isolate, "key",
                                          v8::String::kNormalString, 3));
    obj_ptr<PKey_base> pk = PKey_base::getInstance(v);
    if (!pk)
        return CALL_E_INVALIDARG;

    pk_context *k = &((PKey *)(PKey_base *)pk)->m_key;
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

}
