/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "ifs/ssl.h"
#include "SslSocket.h"
#include <polarssl/pem.h>
#include "parse.h"
#include <map>

namespace fibjs
{

SslSocket::_ssl SslSocket::g_ssl;

result_t ssl_base::loadCert(Buffer_base *DerCert)
{
    int ret;

    std::string crt;
    DerCert->toString(crt);

    ret = x509_crt_parse_der(&SslSocket::g_ssl.m_crt, (const unsigned char *)crt.c_str(), crt.length());
    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

result_t ssl_base::loadCert(const char *txtCert)
{
    int ret;

    if (qstrstr(txtCert, "BEGIN CERTIFICATE"))
    {
        ret = x509_crt_parse(&SslSocket::g_ssl.m_crt, (const unsigned char *)txtCert, qstrlen(txtCert));
        if (ret != 0)
            return Cipher::setError(ret);

        return 0;
    }

    _parser p(txtCert, (int)qstrlen(txtCert));
    bool is_loaded = false;

    while (!p.end())
    {
        std::string cka_value;
        bool in_multiline = false, in_obj = false;
        bool is_cert = false;
        bool is_value = false;

        while (!p.end())
        {
            std::string line;
            std::string cmd, type, value;

            p.getLine(line);
            _parser p1(line);

            p1.skipSpace();
            if (p1.get() == '#')
                continue;

            if (in_multiline)
            {
                if (p1.get() == '\\')
                {
                    if (is_value)
                    {
                        while (p1.get() == '\\')
                        {
                            char ch1, ch2, ch3;

                            p1.skip();

                            ch1 = p1.getChar();
                            if (ch1 < '0' || ch1 > '7')
                                break;

                            ch2 = p1.getChar();
                            if (ch2 < '0' || ch2 > '7')
                                break;

                            ch3 = p1.getChar();
                            if (ch3 < '0' || ch3 > '7')
                                break;

                            ch1 = (ch1 - '0') * 64 + (ch2 - '0') * 8 + (ch3 - '0');
                            cka_value.append(&ch1, 1);
                        }
                    }
                    continue;
                }

                p1.getWord(cmd);
                if (!qstrcmp(cmd.c_str(), "END"))
                    in_multiline = false;

                continue;
            }

            p1.getWord(cmd);

            p1.skipSpace();
            p1.getWord(type);
            if (!qstrcmp(type.c_str(), "MULTILINE_OCTAL"))
            {
                is_value = is_cert && !qstrcmp(cmd.c_str(), "CKA_VALUE");
                if (is_value)
                    cka_value.resize(0);
                in_multiline = true;
                continue;
            }

            p1.skipSpace();
            p1.getLeft(value);

            if (!in_obj)
            {
                if (!qstrcmp(cmd.c_str(), "CKA_CLASS"))
                {
                    in_obj = true;
                    is_cert = !qstrcmp(value.c_str(), "CKO_CERTIFICATE");
                }
                continue;
            }

            if (cmd.empty())
                break;
        }

        if (!cka_value.empty())
        {
            ret = x509_crt_parse_der(&SslSocket::g_ssl.m_crt,
                                     (const unsigned char *)cka_value.c_str(),
                                     cka_value.length());
            if (ret != 0)
                return Cipher::setError(ret);

            is_loaded = true;
        }
    }

    if (!is_loaded)
        return Cipher::setError(POLARSSL_ERR_X509_INVALID_FORMAT);

    return 0;
}

#define PEM_BEGIN_CRT           "-----BEGIN CERTIFICATE-----\n"
#define PEM_END_CRT             "-----END CERTIFICATE-----\n"

result_t ssl_base::exportCert(v8::Local<v8::Array> &retVal)
{
    retVal = v8::Array::New(isolate);

    const x509_crt *pCert = &SslSocket::g_ssl.m_crt;
    int ret, n = 0;

    while (pCert)
    {
        std::string buf;
        size_t olen;

        if (pCert->raw.len > 0)
        {
            buf.resize(pCert->raw.len * 2 + 64);
            ret = pem_write_buffer(PEM_BEGIN_CRT, PEM_END_CRT,
                                   pCert->raw.p, pCert->raw.len,
                                   (unsigned char *)&buf[0], buf.length(), &olen);
            if (ret != 0)
                return Cipher::setError(ret);

            retVal->Set(n ++, v8::String::NewFromUtf8(isolate, buf.c_str(),
                        v8::String::kNormalString, (int) olen - 1));
        }
        pCert = pCert->next;
    }

    return 0;
}

result_t ssl_base::clearCert()
{
    x509_crt_free(&SslSocket::g_ssl.m_crt);
    x509_crt_init(&SslSocket::g_ssl.m_crt);
    return 0;
}

result_t ssl_base::loadCrl(const char *pemCrl)
{
    int ret;

    ret = x509_crl_parse(&SslSocket::g_ssl.m_crl, (const unsigned char *)pemCrl, qstrlen(pemCrl));
    if (ret != 0)
        return Cipher::setError(ret);

    return 0;
}

#define PEM_BEGIN_CRL           "-----BEGIN X509 CRL-----\n"
#define PEM_END_CRL             "-----END X509 CRL-----\n"

result_t ssl_base::exportCrl(v8::Local<v8::Array> &retVal)
{
    retVal = v8::Array::New(isolate);

    const x509_crl *pCrl = &SslSocket::g_ssl.m_crl;
    int ret, n = 0;

    while (pCrl)
    {
        std::string buf;
        size_t olen;

        if (pCrl->raw.len > 0)
        {
            buf.resize(pCrl->raw.len * 2 + 64);
            ret = pem_write_buffer(PEM_BEGIN_CRL, PEM_END_CRL,
                                   pCrl->raw.p, pCrl->raw.len,
                                   (unsigned char *)&buf[0], buf.length(), &olen);
            if (ret != 0)
                return Cipher::setError(ret);

            retVal->Set(n ++, v8::String::NewFromUtf8(isolate, buf.c_str(),
                        v8::String::kNormalString, (int) olen - 1));
        }
        pCrl = pCrl->next;
    }

    return 0;
}

result_t ssl_base::clearCrl()
{
    x509_crl_free(&SslSocket::g_ssl.m_crl);
    x509_crl_init(&SslSocket::g_ssl.m_crl);
    return 0;
}

result_t ssl_base::connect(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                           exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new SslSocket();
    return retVal->connect(s, ac);
}

result_t ssl_base::accept(Stream_base *s, obj_ptr<SslSocket_base> &retVal,
                          exlib::AsyncEvent *ac)
{
    if (!ac)
        return CALL_E_NOSYNC;

    retVal = new SslSocket();
    return retVal->accept(s, ac);
}

}
