/*
 * X509Req.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: lion
 */

#include "ifs/fs.h"
#include "ssl.h"
#include "X509Req.h"
#include "X509Cert.h"
#include "PKey.h"
#include <mbedtls/polarssl/pem.h>
#include <mbedtls/polarssl/oid.h>
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

result_t X509Req::create(const char *subject, PKey_base *key, int32_t hash)
{
    clear();

    x509write_csr csr;
    int ret;

    x509write_csr_init(&csr);

    if (hash < POLARSSL_MD_MD2 || hash > POLARSSL_MD_RIPEMD160)
        return CHECK_ERROR(CALL_E_INVALIDARG);

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
        return CHECK_ERROR(_ssl::setError(ret));

    ret = x509_csr_parse(&m_csr, (const unsigned char *)buf.c_str(), qstrlen(buf.c_str()));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
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
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Req::load(const char *pemReq)
{
    int ret;

    clear();

    ret = x509_csr_parse(&m_csr, (const unsigned char *)pemReq,
                         qstrlen(pemReq));
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

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
        return CHECK_ERROR(_ssl::setError(ret));

    std::string buf;
    buf.resize(8192);

    return 0;
}

#define PEM_BEGIN_CSR           "-----BEGIN CERTIFICATE REQUEST-----\n"
#define PEM_END_CSR             "-----END CERTIFICATE REQUEST-----\n"

result_t X509Req::exportPem(std::string &retVal)
{
    if (m_csr.raw.len == 0)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    std::string buf;
    size_t olen;
    int ret;

    buf.resize(m_csr.raw.len * 2 + 64);
    ret = pem_write_buffer(PEM_BEGIN_CSR, PEM_END_CSR,
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

result_t X509Req::toString(std::string &retVal)
{
    return exportPem(retVal);
}

result_t X509Req::parseString(v8::Local<v8::Value> v, const X509Cert::_name *pNames)
{
    int num = 0;

    if (!IsEmpty(v))
    {
        v8::String::Utf8Value str(v);
        const char *ptr = *str;

        if (!ptr)
            return CHECK_ERROR(_ssl::setError(POLARSSL_ERR_MPI_BAD_INPUT_DATA));

        _parser p(ptr, (int)qstrlen(ptr));

        while (!p.end())
        {
            std::string word;

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
                       exlib::AsyncEvent *ac)
{
    result_t hr;
    bool priv;

    hr = key->isPrivate(priv);
    if (hr < 0)
        return hr;

    if (!priv)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int ret;
    std::string subject;
    pk_context *pk;
    int32_t hash;
    std::string buf;
    obj_ptr<X509Cert> cert;

    if (!ac)
    {
        Isolate &isolate = Isolate::now();
        mpi serial;
        v8::Local<v8::Value> v;

        x509write_crt_init(&m_crt);

        hr = GetConfigValue(opts, "hash", hash);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            hash = m_csr.sig_md;
        else if (hr < 0)
            goto exit;

        if (hash < POLARSSL_MD_MD2 || hash > POLARSSL_MD_RIPEMD160)
        {
            hr = CALL_E_INVALIDARG;
            goto exit;
        }

        x509write_crt_set_md_alg(&m_crt, POLARSSL_MD_SHA1);

        v = opts->Get(v8::String::NewFromUtf8(isolate.isolate, "serial",
                                              v8::String::kNormalString, 6));
        if (!IsEmpty(v))
        {
            v8::String::Utf8Value str(v);

            if (!*str)
            {
                hr = CHECK_ERROR(_ssl::setError(POLARSSL_ERR_MPI_BAD_INPUT_DATA));
                goto exit;
            }

            mpi_init(&serial);
            ret = mpi_read_string(&serial, 10, *str);
            if (ret != 0)
            {
                mpi_free(&serial);
                hr = CHECK_ERROR(_ssl::setError(ret));
                goto exit;
            }
        }
        else
        {
            mpi_init(&serial);
            mpi_lset(&serial, 1);
        }

        ret = x509write_crt_set_serial(&m_crt, &serial);
        if (ret != 0)
        {
            mpi_free(&serial);
            hr = CHECK_ERROR(_ssl::setError(ret));
            goto exit;
        }

        mpi_free(&serial);

        date_t d1, d2;
        std::string s1, s2;

        hr = GetConfigValue(opts, "notBefore", d1);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            d1.now();
        else if (hr < 0)
            goto exit;
        d1.toX509String(s1);


        hr = GetConfigValue(opts, "notAfter", d2);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
        {
            d2 = d1;
            d2.add(1, date_t::_YEAR);
        }
        else if (hr < 0)
            goto exit;
        d2.toX509String(s2);

        ret = x509write_crt_set_validity(&m_crt, s1.c_str(), s2.c_str());
        if (ret != 0)
        {
            hr = CHECK_ERROR(_ssl::setError(ret));
            goto exit;
        }

        bool is_ca = false;
        hr = GetConfigValue(opts, "ca", is_ca);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            goto exit;

        int32_t pathlen = -1;
        hr = GetConfigValue(opts, "pathlen", pathlen);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            goto exit;

        if (pathlen < -1 || pathlen > 127)
        {
            hr = CALL_E_INVALIDARG;
            goto exit;
        }

        ret = x509write_crt_set_basic_constraints(&m_crt, is_ca ? 1 : 0, pathlen);
        if (ret != 0)
        {
            hr = CHECK_ERROR(_ssl::setError(ret));
            goto exit;
        }

        int key_usage = parseString(opts->Get(v8::String::NewFromUtf8(isolate.isolate, "usage",
                                              v8::String::kNormalString, 5)), X509Cert::g_usages);
        if (key_usage < 0)
        {
            hr = key_usage;
            goto exit;
        }
        else if (key_usage)
        {
            ret = x509write_crt_set_key_usage(&m_crt, key_usage);
            if (ret != 0)
            {
                hr = CHECK_ERROR(_ssl::setError(ret));
                goto exit;
            }
        }

        int cert_type = parseString(opts->Get(v8::String::NewFromUtf8(isolate.isolate, "type",
                                              v8::String::kNormalString, 4)), X509Cert::g_types);
        if (cert_type < 0)
        {
            hr = cert_type;
            goto exit;
        }
        else if (cert_type)
        {
            ret = x509write_crt_set_ns_cert_type(&m_crt, cert_type);
            if (ret != 0)
            {
                hr = CHECK_ERROR(_ssl::setError(ret));
                goto exit;
            }
        }

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    pk = &((PKey *)key)->m_key;

    x509write_crt_set_subject_key(&m_crt, &m_csr.pk);
    x509write_crt_set_issuer_key(&m_crt, pk);

    hr = X509Req::get_subject(subject);
    if (hr < 0)
        goto exit;

    ret = x509write_crt_set_subject_name(&m_crt, subject.c_str());
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    ret = x509write_crt_set_issuer_name(&m_crt, issuer);
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    ret = x509write_crt_set_subject_key_identifier(&m_crt);
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    ret = x509write_crt_set_authority_key_identifier(&m_crt);
    if (ret != 0)
    {
        hr = CHECK_ERROR(_ssl::setError(ret));
        goto exit;
    }

    buf.resize(pk_get_size(pk) * 8 + 128);

    ret = x509write_crt_pem(&m_crt, (unsigned char *)&buf[0], buf.length(),
                            ctr_drbg_random, &g_ssl.ctr_drbg);
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
    x509write_crt_free(&m_crt);

    return hr;
}

result_t X509Req::get_subject(std::string &retVal)
{
    int ret;
    std::string buf;

    buf.resize(1024);

    ret = x509_dn_gets(&buf[0], buf.length(), &m_csr.subject);
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
