/*
 * ssl.cpp
 *
 *  Created on: Apr 16, 2014
 *      Author: lion
 */

#include "ifs/ssl.h"
#include "SslSocket.h"
#include "Socket.h"
#include "Url.h"
#include "X509Cert.h"
#include <polarssl/polarssl/error.h>

namespace fibjs
{

_ssl g_ssl;

result_t _ssl::setError(int ret)
{
    char msg[128];

    polarssl_strerror(ret, msg, sizeof(msg));
    return Runtime::setError(msg);
}

result_t ssl_base::connect(const char *url, obj_ptr<Stream_base> &retVal,
                           exlib::AsyncEvent *ac)
{
    class asyncConnect: public asyncState
    {
    public:
        asyncConnect(const char *host, int32_t port, bool ipv6,
                     obj_ptr<Stream_base> &retVal, exlib::AsyncEvent *ac) :
            asyncState(ac), m_host(host), m_port(port), m_ipv6(ipv6), m_retVal(retVal)
        {
            set(connect);
        }

        static int connect(asyncState *pState, int n)
        {
            asyncConnect *pThis = (asyncConnect *) pState;

            pThis->m_sock = new Socket();
            pThis->m_sock->create(pThis->m_ipv6 ? net_base::_AF_INET6 : net_base::_AF_INET,
                                  net_base::_SOCK_STREAM);

            pThis->set(handshake);
            return pThis->m_sock->connect(pThis->m_host, pThis->m_port, pThis);
        }

        static int handshake(asyncState *pState, int n)
        {
            asyncConnect *pThis = (asyncConnect *) pState;

            pThis->set(ok);

            pThis->m_ssl_sock = new SslSocket();
            return pThis->m_ssl_sock->connect(pThis->m_sock, "", pThis->m_temp, pThis);
        }

        static int ok(asyncState *pState, int n)
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
        int m_temp;
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

}
