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
#include "StringBuffer.h"
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
    result_t hr = crl->import(derCrl);
    if (hr < 0)
        return hr;

    retVal = crl;
    return 0;
}

result_t X509Crl_base::_new(exlib::string pemCrl, obj_ptr<X509Crl_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<X509Crl> crl = new X509Crl();
    result_t hr = crl->import(pemCrl);
    if (hr < 0)
        return hr;

    retVal = crl;
    return 0;
}

X509Crl::X509Crl()
{
    mbedtls_x509_crl_init(&m_crl);
}

X509Crl::X509Crl(X509Crl* root, int32_t no)
{
    m_root = root;
    m_no = no;
}

X509Crl::~X509Crl()
{
    if (!m_root)
        mbedtls_x509_crl_free(&m_crl);
}

result_t X509Crl::import(Buffer_base* derCrl)
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;

    exlib::string crl;
    derCrl->toString(crl);

    ret = mbedtls_x509_crl_parse(&m_crl, (const unsigned char*)crl.c_str(),
        crl.length());
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Crl::import(exlib::string pemCrl)
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;

    ret = mbedtls_x509_crl_parse(&m_crl, (const unsigned char*)pemCrl.c_str(),
        pemCrl.length() + 1);
    if (ret != 0)
        return CHECK_ERROR(_ssl::setError(ret));

    return 0;
}

result_t X509Crl::loadFile(exlib::string filename, obj_ptr<X509Crl_base>& retVal)
{
    retVal = new X509Crl();

    result_t hr;
    exlib::string data;
    obj_ptr<Buffer> buf;

    hr = fs_base::ac_readTextFile(filename, data);
    if (hr < 0)
        return hr;

    if (qstrstr(data.c_str(), "BEGIN"))
        return retVal->import(data);

    buf = new Buffer(data.c_str(), data.length());
    return retVal->import(buf);
}

#define PEM_BEGIN_CRL "-----BEGIN X509 CRL-----\n"
#define PEM_END_CRL "-----END X509 CRL-----\n"

result_t X509Crl::pem(bool all, exlib::string& retVal)
{
    StringBuffer sb;

    const mbedtls_x509_crl* pCrl = &m_crl;
    int32_t ret;
    exlib::string buf;
    size_t olen;

    while (pCrl) {
        if (pCrl->raw.len > 0) {
            buf.resize(pCrl->raw.len * 2 + 64);
            ret = mbedtls_pem_write_buffer(PEM_BEGIN_CRL, PEM_END_CRL, pCrl->raw.p, pCrl->raw.len,
                (unsigned char*)buf.data(), buf.length(), &olen);
            if (ret != 0)
                return CHECK_ERROR(_ssl::setError(ret));

            sb.append(buf.c_str(), (int32_t)olen - 1);
        }

        if (!all)
            break;

        pCrl = pCrl->next;
    }

    retVal = sb.str();

    return 0;
}

result_t X509Crl::der(obj_ptr<Buffer_base>& retVal)
{
    const mbedtls_x509_crl* pCrl = &m_crl;
    retVal = new Buffer(pCrl->raw.p, pCrl->raw.len);

    return 0;
}

result_t X509Crl::clear()
{
    if (m_root)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    mbedtls_x509_crl_free(&m_crl);
    mbedtls_x509_crl_init(&m_crl);

    return 0;
}

mbedtls_x509_crl* X509Crl::get_crl()
{
    if (!m_root)
        return &m_crl;

    int32_t n = m_no;
    mbedtls_x509_crl* crl = &m_root->m_crl;

    while (n && (crl = crl->next))
        n--;

    return crl;
}

result_t X509Crl::get_version(int32_t& retVal)
{
    mbedtls_x509_crl* crl = get_crl();
    if (!crl)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = crl->version;
    return 0;
}

result_t X509Crl::get_issuer(exlib::string& retVal)
{
    mbedtls_x509_crl* crl = get_crl();
    if (!crl)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t ret;
    exlib::string buf;

    buf.resize(1024);

    ret = mbedtls_x509_dn_gets(buf.data(), buf.length(), &crl->issuer);
    if (ret < 0)
        return CHECK_ERROR(_ssl::setError(ret));

    buf.resize(ret);
    retVal = buf;

    return 0;
}

result_t X509Crl::get_serials(v8::Local<v8::Array>& retVal)
{
    mbedtls_x509_crl* crl = get_crl();
    if (!crl)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    const mbedtls_x509_crl_entry* cur = &crl->entry;
    int32_t n = 0;
    exlib::string str;
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();

    retVal = v8::Array::New(isolate->m_isolate);
    str.resize(8192);

    while (cur != NULL && cur->serial.len != 0) {
        int32_t ret;
        mbedtls_mpi serial;

        mbedtls_mpi_init(&serial);
        ret = mbedtls_mpi_read_binary(&serial, cur->serial.p, cur->serial.len);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        size_t sz = str.length();

        ret = mbedtls_mpi_write_string(&serial, 10, str.data(), sz, &sz);
        mbedtls_mpi_free(&serial);
        if (ret != 0)
            return CHECK_ERROR(_ssl::setError(ret));

        retVal->Set(context, n++, isolate->NewString(str.c_str(), (int32_t)sz - 1)).IsJust();

        cur = cur->next;
    }

    return 0;
}

result_t X509Crl::get_thisUpdate(date_t& retVal)
{
    mbedtls_x509_crl* crl = get_crl();
    if (!crl)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal.create(crl->this_update.year, crl->this_update.mon,
        crl->this_update.day, crl->this_update.hour,
        crl->this_update.min, crl->this_update.sec, 0);

    return 0;
}

result_t X509Crl::get_nextUpdate(date_t& retVal)
{
    mbedtls_x509_crl* crl = get_crl();
    if (!crl)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal.create(crl->next_update.year, crl->next_update.mon,
        crl->next_update.day, crl->next_update.hour,
        crl->next_update.min, crl->next_update.sec, 0);

    return 0;
}

result_t X509Crl::get_next(obj_ptr<X509Crl_base>& retVal)
{
    if (m_root) {
        mbedtls_x509_crl* crl = get_crl();
        if (!crl || !crl->next)
            return CALL_RETURN_NULL;

        retVal = new X509Crl(m_root, m_no + 1);
    } else {
        if (!m_crl.next)
            return CALL_RETURN_NULL;

        retVal = new X509Crl(this, 1);
    }
    return 0;
}
}
