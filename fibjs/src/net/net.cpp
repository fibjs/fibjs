/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "ifs/ssl.h"
#include "Socket.h"
#include "Smtp.h"
#include "Url.h"

namespace fibjs
{

DECLARE_MODULE(net);

result_t net_base::resolve(const char *name, int32_t family,
                           std::string &retVal, AsyncEvent *ac)
{
    if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_LONGSYNC);

    inetAddr addr_info;

    addr_info.init(family);

    addrinfo hints =
    { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };
    addrinfo *result = NULL;
    addrinfo *ptr = NULL;

    if (getaddrinfo(name, NULL, &hints, &result))
        return CHECK_ERROR(SocketError());

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
        if (ptr->ai_family == addr_info.addr4.sin_family)
        {
            memcpy(&addr_info, ptr->ai_addr, addr_info.size());
            break;
        }

    freeaddrinfo(result);

    if (ptr == NULL)
    {
#ifdef _WIN32
        return -WSAHOST_NOT_FOUND;
#else
        return -ETIME;
#endif
    }

    retVal = addr_info.str();

    return 0;
}

result_t net_base::ip(const char *name, std::string &retVal,
                      AsyncEvent *ac)
{
    return resolve(name, net_base::_AF_INET, retVal, ac);
}

result_t net_base::ipv6(const char *name, std::string &retVal,
                        AsyncEvent *ac)
{
    return resolve(name, net_base::_AF_INET6, retVal, ac);
}

result_t net_base::connect(const char *host, int32_t port, int32_t family,
                           obj_ptr<Stream_base> &retVal, AsyncEvent *ac)
{
    if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;
    obj_ptr<Socket_base> socket;

    hr = Socket_base::_new(family, net_base::_SOCK_STREAM, socket);
    if (hr < 0)
        return hr;

    retVal = socket;
    return socket->connect(host, port, ac);
}

result_t net_base::connect(const char *url, obj_ptr<Stream_base> &retVal,
                           AsyncEvent *ac)
{
    if (!qstrcmp(url, "ssl:", 4))
        return ssl_base::connect(url, retVal, ac);

    if (qstrcmp(url, "tcp:", 4))
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
    return connect(u->m_hostname.c_str(), nPort,
                   u->m_ipv6 ? net_base::_AF_INET6 : net_base::_AF_INET,
                   retVal, ac);
}

result_t net_base::openSmtp(const char *host, int32_t port, int32_t family,
                            obj_ptr<Smtp_base> &retVal, AsyncEvent *ac)
{
    if (family != net_base::_AF_INET && family != net_base::_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = Smtp_base::_new(retVal);
    if (hr < 0)
        return hr;

    return retVal->connect(host, port, family, ac);
}

}
