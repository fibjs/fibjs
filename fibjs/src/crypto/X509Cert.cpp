/*
 * X509Cert.cpp
 *
 *  Created on: Apr 15, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/fs.h"
#include "ssl.h"
#include "X509Cert.h"
#include <mbedtls/mbedtls/pem.h>
#include "PKey.h"
#include "StringBuffer.h"
#include "Buffer.h"
#include <map>

namespace fibjs {

X509Cert::_name X509Cert::g_usages[] = {
    { MBEDTLS_X509_KU_DIGITAL_SIGNATURE, "digitalSignature" },
    { MBEDTLS_X509_KU_NON_REPUDIATION, "nonRepudiation" },
    { MBEDTLS_X509_KU_KEY_ENCIPHERMENT, "keyEncipherment" },
    { MBEDTLS_X509_KU_DATA_ENCIPHERMENT, "dataEncipherment" },
    { MBEDTLS_X509_KU_KEY_AGREEMENT, "keyAgreement" },
    { MBEDTLS_X509_KU_KEY_CERT_SIGN, "keyCertSign" },
    { MBEDTLS_X509_KU_CRL_SIGN, "cRLSign" }
};

X509Cert::_name X509Cert::g_types[] = {
    { MBEDTLS_X509_NS_CERT_TYPE_SSL_CLIENT, "client" },
    { MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER, "server" },
    { MBEDTLS_X509_NS_CERT_TYPE_EMAIL, "email" },
    { MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING, "objsign" },
    { MBEDTLS_X509_NS_CERT_TYPE_RESERVED, "reserved" },
    { MBEDTLS_X509_NS_CERT_TYPE_SSL_CA, "sslCA" },
    { MBEDTLS_X509_NS_CERT_TYPE_EMAIL_CA, "emailCA" },
    { MBEDTLS_X509_NS_CERT_TYPE_OBJECT_SIGNING_CA, "objCA" }
};

result_t X509Cert_base::_new(obj_ptr<X509Cert_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new X509Cert();
    return 0;
}

result_t X509Cert_base::_new(Buffer_base* derCert, obj_ptr<X509Cert_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<X509Cert> crt = new X509Cert();
    result_t hr = crt->import(derCert);
    if (hr < 0)
        return hr;

    retVal = crt;
    return 0;
}

result_t X509Cert_base::_new(exlib::string txtCert, obj_ptr<X509Cert_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<X509Cert> crt = new X509Cert();
    result_t hr = crt->import(txtCert);
    if (hr < 0)
        return hr;

    retVal = crt;
    return 0;
}

X509Cert::X509Cert()
    : m_rootLoaded(false)
{
    mbedtls_x509_crt_init(&m_crt);
}

X509Cert::X509Cert(X509Cert* root, int32_t no)
{
    m_root = root;
    m_no = no;
}

X509Cert::~X509Cert()
{
    if (!m_root)
        mbedtls_x509_crt_free(&m_crt);
}

result_t X509Cert::import(Buffer_base* derCert)
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;

    exlib::string crt;
    derCert->toString(crt);

    ret = mbedtls_x509_crt_parse_der(&m_crt, (const unsigned char*)crt.c_str(),
        crt.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Cert::import(const mbedtls_x509_crt* crt)
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;

    ret = mbedtls_x509_crt_parse_der(&m_crt, crt->raw.p, crt->raw.len);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Cert::import(exlib::string txtCert)
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;

    if (!qstrstr(txtCert.c_str(), "BEGIN CERTIFICATE"))
        return CHECK_ERROR(_ssl::setError(MBEDTLS_ERR_X509_INVALID_FORMAT));

    ret = mbedtls_x509_crt_parse(&m_crt, (const unsigned char*)txtCert.c_str(),
        txtCert.length() + 1);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Cert::loadFile(exlib::string filename)
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    result_t hr;
    exlib::string data;
    obj_ptr<Buffer> buf;

    hr = fs_base::ac_readTextFile(filename, data);
    if (hr < 0)
        return hr;

    if (qstrstr(data.c_str(), "BEGIN CERTIFICATE") || qstrstr(data.c_str(), "CKO_CERTIFICATE"))
        return import(data);

    buf = new Buffer(data);
    return import(buf);
}

result_t X509Cert::loadRootCerts()
{
    if (m_rootLoaded)
        return 0;
    m_rootLoaded = true;

    _cert* pca = g_root_ca;
    int32_t ret;

    while (pca->size) {
        ret = mbedtls_x509_crt_parse_der(&m_crt,
            (const unsigned char*)pca->data, pca->size);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        pca++;
    }

    return 0;
}

result_t X509Cert::verify(X509Cert_base* cert, bool& retVal, AsyncEvent* ac)
{
    int32_t ret;
    uint32_t flags;

    ret = mbedtls_x509_crt_verify(&(((X509Cert*)cert)->m_crt), &m_crt, NULL, NULL, &flags,
        NULL, NULL);
    if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED) {
        retVal = false;
        return 0;
    } else if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal = true;
    return 0;
}

#define PEM_BEGIN_CRT "-----BEGIN CERTIFICATE-----\n"
#define PEM_END_CRT "-----END CERTIFICATE-----\n"

result_t X509Cert::pem(bool all, exlib::string& retVal)
{
    StringBuffer sb;

    const mbedtls_x509_crt* pCert = &m_crt;
    int32_t ret;
    exlib::string buf;
    size_t olen;

    while (pCert) {
        if (pCert->raw.len > 0) {
            buf.resize(pCert->raw.len * 2 + 64);
            ret = mbedtls_pem_write_buffer(PEM_BEGIN_CRT, PEM_END_CRT, pCert->raw.p, pCert->raw.len,
                (unsigned char*)buf.c_buffer(), buf.length(), &olen);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));

            sb.append(buf.c_str(), (int32_t)olen - 1);
        }

        if (!all)
            break;

        pCert = pCert->next;
    }

    retVal = sb.str();

    return 0;
}

result_t X509Cert::der(obj_ptr<Buffer_base>& retVal)
{
    const mbedtls_x509_crt* pCert = &m_crt;
    retVal = new Buffer(pCert->raw.p, pCert->raw.len);

    return 0;
}

result_t X509Cert::clear()
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_x509_crt_free(&m_crt);
    mbedtls_x509_crt_init(&m_crt);
    m_rootLoaded = false;

    return 0;
}

mbedtls_x509_crt* X509Cert::get_crt()
{
    if (!m_root)
        return &m_crt;

    int32_t n = m_no;
    mbedtls_x509_crt* crt = &m_root->m_crt;

    while (n && (crt = crt->next))
        n--;

    return crt;
}

result_t X509Cert::get_version(int32_t& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = crt->version;
    return 0;
}

result_t X509Cert::get_serial(exlib::string& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    mbedtls_mpi serial;

    mbedtls_mpi_init(&serial);
    ret = mbedtls_mpi_read_binary(&serial, crt->serial.p, crt->serial.len);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal.resize(8192);
    size_t sz = retVal.length();

    ret = mbedtls_mpi_write_string(&serial, 10, retVal.c_buffer(), sz, &sz);
    mbedtls_mpi_free(&serial);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    retVal.resize(sz - 1);

    return 0;
}

result_t X509Cert::get_issuer(exlib::string& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string buf;

    buf.resize(1024);

    ret = mbedtls_x509_dn_gets(buf.c_buffer(), buf.length(), &crt->issuer);
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(ret);
    retVal = buf;

    return 0;
}

result_t X509Cert::get_subject(exlib::string& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string buf;

    buf.resize(1024);

    ret = mbedtls_x509_dn_gets(buf.c_buffer(), buf.length(), &crt->subject);
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(ret);
    retVal = buf;

    return 0;
}

result_t X509Cert::get_notBefore(date_t& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal.create(crt->valid_from.year, crt->valid_from.mon,
        crt->valid_from.day, crt->valid_from.hour,
        crt->valid_from.min, crt->valid_from.sec, 0);

    return 0;
}

result_t X509Cert::get_notAfter(date_t& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal.create(crt->valid_to.year, crt->valid_to.mon,
        crt->valid_to.day, crt->valid_to.hour,
        crt->valid_to.min, crt->valid_to.sec, 0);

    return 0;
}

result_t X509Cert::get_ca(bool& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = crt->ca_istrue != 0;
    return 0;
}

result_t X509Cert::get_pathlen(int32_t& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = crt->max_pathlen;
    return 0;
}

result_t X509Cert::get_usage(exlib::string& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t key_usage = crt->key_usage;

    int32_t i;
    for (i = 0; i < (int32_t)ARRAYSIZE(g_usages); i++) {
        if (key_usage & g_usages[i].id) {
            if (!retVal.empty())
                retVal.append(", ", 2);
            retVal.append(g_usages[i].name);
        }
    }

    return 0;
}

result_t X509Cert::get_type(exlib::string& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t cert_type = crt->ns_cert_type;

    int32_t i;
    for (i = 0; i < (int32_t)ARRAYSIZE(g_types); i++) {
        if (cert_type & g_types[i].id) {
            if (!retVal.empty())
                retVal.append(", ", 2);
            retVal.append(g_types[i].name);
        }
    }

    return 0;
}

result_t X509Cert::get_sig_md(int32_t& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = crt->sig_md;
    return 0;
}

result_t X509Cert::get_sig_pk(int32_t& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = crt->sig_pk;
    return 0;
}

result_t X509Cert::get_publicKey(obj_ptr<PKey_base>& retVal)
{
    mbedtls_x509_crt* crt = get_crt();
    if (!crt)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!crt->sig_pk)
        return CALL_RETURN_NULL;

    retVal = PKey::create(crt->pk, true);
    return 0;
}

result_t X509Cert::get_next(obj_ptr<X509Cert_base>& retVal)
{
    if (m_root) {
        mbedtls_x509_crt* crt = get_crt();
        if (!crt || !crt->next)
            return CALL_RETURN_NULL;

        retVal = new X509Cert(m_root, m_no + 1);
    } else {
        if (!m_crt.next)
            return CALL_RETURN_NULL;

        retVal = new X509Cert(this, 1);
    }

    return 0;
}
}
