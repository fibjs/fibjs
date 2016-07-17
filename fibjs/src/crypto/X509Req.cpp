/*
 * X509Req.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/fs.h"
#include "ssl.h"
#include "X509Req.h"
#include "X509Cert.h"
#include "PKey.h"
#include <mbedtls/mbedtls/pem.h>
#include <mbedtls/mbedtls/oid.h>
#include "Buffer.h"
#include "parse.h"

namespace fibjs
{

result_t X509Req_base::_new(obj_ptr<X509Req_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new X509Req();
    return 0;
}

result_t X509Req_base::_new(const char *subject, PKey_base *key,
                            int32_t hash, obj_ptr<X509Req_base> &retVal,
                            v8::Local<v8::Object> This)
{
    obj_ptr<X509Req> req = new X509Req();
    result_t hr;

    hr = req->create(subject, key, hash);
    if (hr < 0)
        return hr;

    retVal = req;

    return 0;
}

X509Req::X509Req()
{
    mbedtls_x509_csr_init(&m_csr);
}

X509Req::~X509Req()
{
    mbedtls_x509_csr_free(&m_csr);
}

void X509Req::clear()
{
    mbedtls_x509_csr_free(&m_csr);
    mbedtls_x509_csr_init(&m_csr);
}

result_t X509Req::create(const char *subject, PKey_base *key, int32_t hash)
{
    clear();

    mbedtls_x509write_csr csr;
    int32_t ret;
    mbedtls_x509write_csr_init(&csr);

    if (hash < MBEDTLS_MD_MD2 || hash > MBEDTLS_MD_RIPEMD160)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    mbedtls_x509write_csr_set_md_alg(&csr, (mbedtls_md_type_t)hash);
    mbedtls_x509write_csr_set_subject_name(&csr, subject);

    mbedtls_pk_context *k = &((PKey *)(PKey_base *)key)->m_key;
    mbedtls_x509write_csr_set_key(&csr, k);

    exlib::string buf;
    buf.resize(mbedtls_pk_get_bitlen(k) * 8 + 128);

    ret = mbedtls_x509write_csr_pem(&csr, (unsigned char *)&buf[0], buf.length(),
                                    mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    mbedtls_x509write_csr_free(&csr);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    ret = mbedtls_x509_csr_parse(&m_csr, (const unsigned char *)buf.c_str(),
                                 qstrlen(buf.c_str()) + 1);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Req::load(Buffer_base *derReq)
{
    int32_t ret;

    clear();

    exlib::string csr;
    derReq->toString(csr);

    ret = mbedtls_x509_csr_parse(&m_csr, (const unsigned char *)csr.c_str(),
                                 csr.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Req::load(const char *pemReq)
{
    int32_t ret;

    clear();

    ret = mbedtls_x509_csr_parse(&m_csr, (const unsigned char *)pemReq,
                                 qstrlen(pemReq) + 1);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Req::loadFile(const char *filename)
{
    result_t hr;
    exlib::string data;
    int32_t ret;

    clear();

    hr = fs_base::ac_readFile(filename, data);
    if (hr < 0)
        return hr;

    ret = mbedtls_x509_csr_parse(&m_csr, (const unsigned char *)data.c_str(),
                                 data.length() + 1);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    exlib::string buf;
    buf.resize(8192);

    return 0;
}

#define PEM_BEGIN_CSR           "-----BEGIN CERTIFICATE REQUEST-----\n"
#define PEM_END_CSR             "-----END CERTIFICATE REQUEST-----\n"

result_t X509Req::exportPem(exlib::string &retVal)
{
    if (m_csr.raw.len == 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    exlib::string buf;
    size_t olen;
    int32_t ret;

    buf.resize(m_csr.raw.len * 2 + 64);
    ret = mbedtls_pem_write_buffer(PEM_BEGIN_CSR, PEM_END_CSR,
                                   m_csr.raw.p, m_csr.raw.len,
                                   (unsigned char *)&buf[0], buf.length(), &olen);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(olen - 1);
    retVal = buf;

    return 0;
}

result_t X509Req::exportDer(obj_ptr<Buffer_base> &retVal)
{
    if (m_csr.raw.len == 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = new Buffer(m_csr.raw.p, m_csr.raw.len);

    return 0;
}

result_t X509Req::toString(exlib::string &retVal)
{
    return exportPem(retVal);
}

result_t X509Req::parseString(v8::Local<v8::Value> v, const X509Cert::_name *pNames)
{
    int32_t num = 0;

    if (!IsEmpty(v))
    {
        v8::String::Utf8Value str(v);
        const char *ptr = *str;

        if (!ptr)
            return CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_MPI_BAD_INPUT_DATA));

        _parser p(ptr, (int32_t)qstrlen(ptr));

        while (!p.end())
        {
            exlib::string word;

            p.skipSpace();
            p.getWord(word, ',');
            p.skipSpace();
            if (p.get() == ',')
                p.skip();
            p.skipSpace();

            if (!word.empty())
            {
                const X509Cert::_name *pItem = pNames;
                while (pItem->id)
                {
                    if (!qstrcmp(word.c_str(), pItem->name))
                    {
                        num |= pItem->id;
                        break;
                    }
                    pItem ++;
                }

                if (!pItem->id)
                    return CHECK_ERROR(CALL_E_INVALIDARG);
            }
        }
    }

    return num;
}

result_t X509Req::sign(const char *issuer, PKey_base *key,
                       v8::Local<v8::Object> opts, obj_ptr<X509Cert_base> &retVal,
                       AsyncEvent *ac)
{
    result_t hr;
    bool priv;

    hr = key->isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t ret;
    exlib::string subject;
    mbedtls_pk_context *pk;
    int32_t hash;
    exlib::string buf;
    obj_ptr<X509Cert> cert;

    if (!ac)
    {
        Isolate* isolate = holder();
        mbedtls_mpi serial;
        v8::Local<v8::Value> v;

        mbedtls_x509write_crt_init(&m_crt);

        hr = GetConfigValue(isolate->m_isolate, opts, "hash", hash);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            hash = m_csr.sig_md;
        else if (hr < 0)
            goto exit;

        if (hash < MBEDTLS_MD_MD2 || hash > MBEDTLS_MD_RIPEMD160)
        {
            hr = CALL_E_INVALIDARG;
            goto exit;
        }

        mbedtls_x509write_crt_set_md_alg(&m_crt, MBEDTLS_MD_SHA1);

        v = opts->Get(isolate->NewFromUtf8("serial", 6));
        if (!IsEmpty(v))
        {
            v8::String::Utf8Value str(v);

            if (!*str)
            {
                hr = CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_MPI_BAD_INPUT_DATA));
                goto exit;
            }

            mbedtls_mpi_init(&serial);
            ret = mbedtls_mpi_read_string(&serial, 10, *str);
            if (ret != 0)
            {
                mbedtls_mpi_free(&serial);
                hr = CHECK_ERROR(_ssl::setError(ret));
                goto exit;
            }
        }
        else
        {
            mbedtls_mpi_init(&serial);
            mbedtls_mpi_lset(&serial, 1);
        }

        ret = mbedtls_x509write_crt_set_serial(&m_crt, &serial);
        if (ret != 0)
        {
            mbedtls_mpi_free(&serial);
            hr = CHECK_ERROR(_ssl::setError(ret));
            goto exit;
        }

        mbedtls_mpi_free(&serial);

        date_t d1, d2;
        exlib::string s1, s2;

        hr = GetConfigValue(isolate->m_isolate, opts, "notBefore", d1);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            d1.now();
        else if (hr < 0)
            goto exit;
        d1.toX509String(s1);


        hr = GetConfigValue(isolate->m_isolate, opts, "notAfter", d2);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
        {
            d2 = d1;
            d2.add(1, date_t::_YEAR);
        }
        else if (hr < 0)
            goto exit;
        d2.toX509String(s2);

        ret = mbedtls_x509write_crt_set_validity(&m_crt, s1.c_str(), s2.c_str());
        if (ret != 0)
        {
            hr = CHECK_ERROR(_ssl::setError(ret));
            goto exit;
        }

        bool is_ca = false;
        hr = GetConfigValue(isolate->m_isolate, opts, "ca", is_ca);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            goto exit;

        int32_t pathlen = -1;
        hr = GetConfigValue(isolate->m_isolate, opts, "pathlen", pathlen);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            goto exit;

        if (pathlen < -1 || pathlen > 127)
        {
            hr = CALL_E_INVALIDARG;
            goto exit;
        }

        ret = mbedtls_x509write_crt_set_basic_constraints(&m_crt, is_ca ? 1 : 0, pathlen);
        if (ret != 0)
        {
            hr = CHECK_ERROR(_ssl::setError(ret));
            goto exit;
        }

        int32_t key_usage = parseString(opts->Get(isolate->NewFromUtf8("usage", 5)), X509Cert::g_usages);
        if (key_usage < 0)
        {
            hr = key_usage;
            goto exit;
        }
        else if (key_usage)
        {
            ret = mbedtls_x509write_crt_set_key_usage(&m_crt, key_usage);
            if (ret != 0)
            {
                hr = CHECK_ERROR(_ssl::setError(ret));
                goto exit;
            }
        }

        int32_t cert_type = parseString(opts->Get(isolate->NewFromUtf8("type", 4)), X509Cert::g_types);
        if (cert_type < 0)
        {
            hr = cert_type;
            goto exit;
        }
        else if (cert_type)
        {
            ret = mbedtls_x509write_crt_set_ns_cert_type(&m_crt, cert_type);
            if (ret != 0)
            {
                hr = CHECK_ERROR(_ssl::setError(ret));
                goto exit;
            }
        }

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    pk = &((PKey *)key)->m_key;

    mbedtls_x509write_crt_set_subject_key(&m_crt, &m_csr.pk);
    mbedtls_x509write_crt_set_issuer_key(&m_crt, pk);

    hr = X509Req::get_subject(subject);
    if (hr < 0)
        goto exit;

    ret = mbedtls_x509write_crt_set_subject_name(&m_crt, subject.c_str());
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    ret = mbedtls_x509write_crt_set_issuer_name(&m_crt, issuer);
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    ret = mbedtls_x509write_crt_set_subject_key_identifier(&m_crt);
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    ret = mbedtls_x509write_crt_set_authority_key_identifier(&m_crt);
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    buf.resize(mbedtls_pk_get_bitlen(pk) * 8 + 128);

    ret = mbedtls_x509write_crt_pem(&m_crt, (unsigned char *)&buf[0], buf.length(),
                                    mbedtls_ctr_drbg_random, &g_ssl.ctr_drbg);
    if (ret < 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    cert = new X509Cert();
    hr = cert->load(buf.c_str());
    if (hr < 0)
        goto exit;

    retVal = cert;

exit:
    mbedtls_x509write_crt_free(&m_crt);

    return hr;
}

result_t X509Req::get_subject(exlib::string &retVal)
{
    int32_t ret;
    exlib::string buf;

    buf.resize(1024);

    ret = mbedtls_x509_dn_gets(&buf[0], buf.length(), &m_csr.subject);
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

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
        return hr;

    retVal = pk1;
    return 0;
}

}
