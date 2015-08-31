/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "ifs/crypto.h"
#include "ifs/ssl.h"
#include "SslSocket.h"
#include "Socket.h"
#include "Url.h"
#include "X509Cert.h"
#include <mbedtls/mbedtls/error.h>

namespace fibjs
{

_ssl g_ssl;

result_t _ssl::setError(int32_t ret)
{
    char msg[128];

    mbedtls_strerror(ret, msg, sizeof(msg));
    return CHECK_ERROR(Runtime::setError(msg));
}

result_t ssl_base::connect(const char *url, obj_ptr<Stream_base> &retVal,
                           AsyncEvent *ac)
{
    class asyncConnect: public AsyncState
    {
    public:
        asyncConnect(const char *host, int32_t port, bool ipv6,
                     obj_ptr<Stream_base> &retVal, AsyncEvent *ac) :
            AsyncState(ac), m_host(host), m_port(port), m_ipv6(ipv6), m_retVal(retVal)
        {
            set(connect);
        }

        static int32_t connect(AsyncState *pState, int32_t n)
        {
            asyncConnect *pThis = (asyncConnect *) pState;

            pThis->m_sock = new Socket();
            pThis->m_sock->create(pThis->m_ipv6 ? net_base::_AF_INET6 : net_base::_AF_INET,
                                  net_base::_SOCK_STREAM);

            pThis->set(handshake);
            return pThis->m_sock->connect(pThis->m_host, pThis->m_port, pThis);
        }

        static int32_t handshake(AsyncState *pState, int32_t n)
        {
            asyncConnect *pThis = (asyncConnect *) pState;

            pThis->set(ok);

            pThis->m_ssl_sock = new SslSocket();

            if (g_ssl.m_crt && g_ssl.m_key)
            {
                result_t hr = pThis->m_ssl_sock->setCert(g_ssl.m_crt, g_ssl.m_key);
                if (hr < 0)
                    return hr;
            }

            return pThis->m_ssl_sock->connect(pThis->m_sock, pThis->m_host, pThis->m_temp, pThis);
        }

        static int32_t ok(AsyncState *pState, int32_t n)
        {
            asyncConnect *pThis = (asyncConnect *) pState;

            pThis->m_retVal = pThis->m_ssl_sock;
            return pThis->done();
        }

    private:
        const char *m_host;
        int32_t m_port;
        bool m_ipv6;
        obj_ptr<Stream_base> &m_retVal;
        obj_ptr<Socket> m_sock;
        obj_ptr<SslSocket> m_ssl_sock;
        int32_t m_temp;
    };

    if (qstrcmp(url, "ssl:", 4))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    if (u->m_port.length() == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t nPort = atoi(u->m_port.c_str());

    return (new asyncConnect(u->m_hostname.c_str(), nPort, u->m_ipv6,
                             retVal, ac))->post(0);
}

result_t ssl_base::setClientCert(X509Cert_base* crt, PKey_base* key)
{
    g_ssl.m_crt = crt;
    g_ssl.m_key = key;
    return 0;
}

result_t ssl_base::loadClientCertFile(const char* crtFile, const char* keyFile,
                                      const char* password)
{
    result_t hr;

    hr = crypto_base::loadCert(crtFile, g_ssl.m_crt);
    if (hr < 0)
        return hr;

    hr = crypto_base::loadPKey(keyFile, password, g_ssl.m_key);
    if (hr < 0)
    {
        g_ssl.m_crt.Release();
        return hr;
    }

    return 0;
}

result_t ssl_base::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    retVal = g_ssl.ca();
    return 0;
}

result_t ssl_base::get_verification(int32_t &retVal)
{
    retVal = g_ssl.m_authmode;
    return 0;
}

result_t ssl_base::set_verification(int32_t newVal)
{
    if (newVal < ssl_base::_VERIFY_NONE || newVal > ssl_base::_VERIFY_REQUIRED)
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
