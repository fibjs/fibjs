/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/crypto.h"
#include "ifs/ssl.h"
#include "SslSocket.h"
#include "Socket.h"
#include "Url.h"
#include "X509Cert.h"
#include <mbedtls/mbedtls/error.h>

namespace fibjs {

DECLARE_MODULE(ssl);
DECLARE_MODULE_EX(tls, ssl);

_ssl g_ssl;

class X509CertProxy : public X509Cert_base {
public:
    X509CertProxy(X509Cert* ca)
        : m_ca(ca)
    {
    }

public:
    // X509Cert_base
    result_t load(Buffer_base* derCert)
    {
        return m_ca->load(derCert);
    }

    result_t load(exlib::string txtCert)
    {
        return m_ca->load(txtCert);
    }

    result_t loadFile(exlib::string filename)
    {
        return m_ca->loadFile(filename);
    }

    result_t loadRootCerts()
    {
        return m_ca->loadRootCerts();
    }

    result_t verify(X509Cert_base* cert, bool& retVal, AsyncEvent* ac)
    {
        return m_ca->verify(cert, retVal, ac);
    }

    result_t dump(bool pem, v8::Local<v8::Array>& retVal)
    {
        return m_ca->dump(pem, retVal);
    }

    result_t clear()
    {
        return m_ca->clear();
    }

    result_t get_version(int32_t& retVal)
    {
        return m_ca->get_version(retVal);
    }

    result_t get_serial(exlib::string& retVal)
    {
        return m_ca->get_serial(retVal);
    }

    result_t get_issuer(exlib::string& retVal)
    {
        return m_ca->get_issuer(retVal);
    }

    result_t get_subject(exlib::string& retVal)
    {
        return m_ca->get_subject(retVal);
    }

    result_t get_notBefore(date_t& retVal)
    {
        return m_ca->get_notBefore(retVal);
    }

    result_t get_notAfter(date_t& retVal)
    {
        return m_ca->get_notAfter(retVal);
    }

    result_t get_ca(bool& retVal)
    {
        return m_ca->get_ca(retVal);
    }

    result_t get_pathlen(int32_t& retVal)
    {
        return m_ca->get_pathlen(retVal);
    }

    result_t get_usage(exlib::string& retVal)
    {
        return m_ca->get_usage(retVal);
    }

    result_t get_type(exlib::string& retVal)
    {
        return m_ca->get_type(retVal);
    }

    result_t get_publicKey(obj_ptr<PKey_base>& retVal)
    {
        return m_ca->get_publicKey(retVal);
    }

    result_t get_next(obj_ptr<X509Cert_base>& retVal)
    {
        return m_ca->get_next(retVal);
    }

private:
    obj_ptr<X509Cert> m_ca;
};

result_t _ssl::setError(int32_t ret)
{
    char msg[128];

    mbedtls_strerror(ret, msg, sizeof(msg));
    return Runtime::setError(msg);
}

result_t ssl_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal,
    AsyncEvent* ac)
{
    return connect(url, g_ssl.m_crt, g_ssl.m_key, timeout, retVal, ac);
}

result_t ssl_base::connect(exlib::string url, X509Cert_base* crt, PKey_base* key, int32_t timeout,
    obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    class asyncConnect : public AsyncState {
    public:
        asyncConnect(const exlib::string host, int32_t port, bool ipv6, X509Cert_base* crt, PKey_base* key,
            int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_host(host)
            , m_port(port)
            , m_ipv6(ipv6)
            , m_crt(crt)
            , m_key(key)
            , m_timeout(timeout)
            , m_retVal(retVal)
        {
            next(connect);
        }

        ON_STATE(asyncConnect, connect)
        {
            Socket_base::_new(m_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET, m_sock);

            m_sock->set_timeout(m_timeout);
            return m_sock->connect(m_host, m_port, next(handshake));
        }

        ON_STATE(asyncConnect, handshake)
        {
            m_ssl_sock = new SslSocket();

            if (m_crt && m_key) {
                result_t hr = m_ssl_sock->setCert("", m_crt, m_key);
                if (hr < 0)
                    return hr;
            }

            return m_ssl_sock->connect(m_sock, m_host, m_temp, next(ok));
        }

        ON_STATE(asyncConnect, ok)
        {
            m_retVal = m_ssl_sock;
            return next();
        }

    private:
        const exlib::string m_host;
        int32_t m_port;
        bool m_ipv6;
        obj_ptr<X509Cert_base> m_crt;
        obj_ptr<PKey_base> m_key;
        int32_t m_timeout;
        obj_ptr<Stream_base>& m_retVal;
        obj_ptr<Socket_base> m_sock;
        obj_ptr<SslSocket> m_ssl_sock;
        int32_t m_temp;
    };

    if (qstrcmp(url.c_str(), "ssl:", 4))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    if (u->m_port.length() == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t nPort = atoi(u->m_port.c_str());

    return (new asyncConnect(u->m_hostname, nPort, u->m_ipv6, crt, key, timeout,
                retVal, ac))
        ->post(0);
}

result_t ssl_base::setClientCert(X509Cert_base* crt, PKey_base* key)
{
    g_ssl.m_crt = crt;
    g_ssl.m_key = key;
    return 0;
}

result_t ssl_base::loadClientCertFile(exlib::string crtFile, exlib::string keyFile,
    exlib::string password)
{
    result_t hr;

    hr = crypto_base::loadCert(crtFile, g_ssl.m_crt);
    if (hr < 0)
        return hr;

    hr = crypto_base::loadPKey(keyFile, password, g_ssl.m_key);
    if (hr < 0) {
        g_ssl.m_crt.Release();
        return hr;
    }

    return 0;
}

result_t ssl_base::loadRootCerts()
{
    obj_ptr<X509Cert_base> ca = new X509CertProxy(g_ssl.ca());
    return ca->loadRootCerts();
}

result_t ssl_base::get_ca(obj_ptr<X509Cert_base>& retVal)
{
    retVal = new X509CertProxy(g_ssl.ca());
    return 0;
}

result_t ssl_base::get_verification(int32_t& retVal)
{
    retVal = g_ssl.m_authmode;
    return 0;
}

result_t ssl_base::set_verification(int32_t newVal)
{
    if (newVal < ssl_base::C_VERIFY_NONE || newVal > ssl_base::C_VERIFY_REQUIRED)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    g_ssl.m_authmode = newVal;
    return 0;
}

result_t ssl_base::get_min_version(int32_t& retVal)
{
    retVal = g_ssl.m_min_version;
    return 0;
}

result_t ssl_base::set_min_version(int32_t newVal)
{
    g_ssl.m_min_version = newVal;
    return 0;
}

result_t ssl_base::get_max_version(int32_t& retVal)
{
    retVal = g_ssl.m_max_version;
    return 0;
}

result_t ssl_base::set_max_version(int32_t newVal)
{
    g_ssl.m_max_version = newVal;
    return 0;
}
}
