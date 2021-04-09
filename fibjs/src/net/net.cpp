/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/dns.h"
#include "ifs/ssl.h"
#include "ifs/os.h"
#include "Socket.h"
#include "inetAddr.h"
#include "Smtp.h"
#include "Url.h"
#include "options.h"

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

namespace fibjs {

DECLARE_MODULE(dns);

result_t dns_base::resolve(exlib::string name, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };
    addrinfo* result = NULL;
    addrinfo* ptr = NULL;

    int res = getaddrinfo(name.c_str(), NULL, &hints, &result);
    if (res)
        return CHECK_ERROR(Runtime::setError(gai_strerror(res)));

    obj_ptr<NArray> arr = new NArray();
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        inetAddr addr_info;
        addr_info.init(ptr->ai_addr);
        arr->append(addr_info.str());
    }

    freeaddrinfo(result);

    retVal = arr;

    return 0;
}

result_t dns_base::lookup(exlib::string name, exlib::string& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };
    addrinfo* result = NULL;
    addrinfo* ptr = NULL;

    int res = getaddrinfo(name.c_str(), NULL, &hints, &result);
    if (res)
        return CHECK_ERROR(Runtime::setError(gai_strerror(res)));

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        inetAddr addr_info;
        addr_info.init(ptr->ai_addr);
        retVal = addr_info.str();
        break;
    }

    freeaddrinfo(result);

    return 0;
}

DECLARE_MODULE(net);

result_t net_base::get_use_uv_socket(bool& retVal)
{
    retVal = g_uv_socket;
    return 0;
}

result_t net_base::set_use_uv_socket(bool newVal)
{
    g_uv_socket = newVal;
    return 0;
}

result_t net_base::info(v8::Local<v8::Object>& retVal)
{
    return os_base::networkInterfaces(retVal);
}

result_t net_base::resolve(exlib::string name, int32_t family,
    exlib::string& retVal, AsyncEvent* ac)
{
    if (family != net_base::C_AF_INET && family != net_base::C_AF_INET6)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    inetAddr addr_info;

    addr_info.init(family);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };
    addrinfo* result = NULL;
    addrinfo* ptr = NULL;

    int res = getaddrinfo(name.c_str(), NULL, &hints, &result);
    if (res)
        return CHECK_ERROR(Runtime::setError(gai_strerror(res)));

    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
        if (ptr->ai_family == addr_info.addr4.sin_family) {
            memcpy(&addr_info, ptr->ai_addr, addr_info.size());
            break;
        }

    freeaddrinfo(result);

    if (ptr == NULL) {
#ifdef _WIN32
        return -WSAHOST_NOT_FOUND;
#else
        return -ETIME;
#endif
    }

    retVal = addr_info.str();

    return 0;
}

result_t net_base::ip(exlib::string name, exlib::string& retVal,
    AsyncEvent* ac)
{
    return resolve(name, net_base::C_AF_INET, retVal, ac);
}

result_t net_base::ipv6(exlib::string name, exlib::string& retVal,
    AsyncEvent* ac)
{
    return resolve(name, net_base::C_AF_INET6, retVal, ac);
}

result_t net_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal,
    AsyncEvent* ac)
{
    if (!qstrcmp(url.c_str(), "ssl:", 4))
        return ssl_base::connect(url, timeout, retVal, ac);

    if (qstrcmp(url.c_str(), "tcp:", 4))
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
    int32_t family = u->m_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;

    obj_ptr<Socket_base> socket;

    hr = Socket_base::_new(family, socket);
    if (hr < 0)
        return hr;

    socket->set_timeout(timeout);

    retVal = socket;
    return socket->connect(u->m_hostname, nPort, ac);
}

result_t net_base::openSmtp(exlib::string url, int32_t timeout,
    obj_ptr<Smtp_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr;

    hr = Smtp_base::_new(retVal);
    if (hr < 0)
        return hr;

    retVal->set_timeout(timeout);

    return retVal->connect(url, ac);
}

result_t net_base::isIP(exlib::string ip, int32_t& retVal)
{
    retVal = 0;
    bool is = false;

    isIPv4(ip, is);

    if (is) {
        retVal = 4;
        return 0;
    }

    isIPv6(ip, is);

    if (is)
        retVal = 6;

    return 0;
}

result_t net_base::isIPv4(exlib::string ip, bool& retVal)
{
    retVal = true;
    sockaddr_in dst;
    if (uv_ip4_addr(ip.c_str(), 0, &dst))
        retVal = false;

    return 0;
}

result_t net_base::isIPv6(exlib::string ip, bool& retVal)
{
    retVal = true;
    sockaddr_in6 dst;
    if (uv_ip6_addr(ip.c_str(), 0, &dst))
        retVal = false;

    return 0;
}
}
