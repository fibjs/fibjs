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
#include "AsyncUV.h"

#ifndef INET6_ADDRSTRLEN
#define INET6_ADDRSTRLEN 46
#endif

namespace fibjs {

DECLARE_MODULE(dns);

result_t dns_base::resolve(exlib::string name, obj_ptr<NArray>& retVal, AsyncEvent* ac)
{
    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(obj_ptr<NArray>& retVal, AsyncEvent* ac)
            : _retVal(retVal)
            , _ac(ac)
        {
        }

    public:
        obj_ptr<NArray>& _retVal;
        AsyncEvent* _ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data(retVal, ac);
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;

            if (status < 0) {
                uv_freeaddrinfo(res);
                delete resolver;

                resolver->_ac->post(status);
                return;
            }

            obj_ptr<NArray> arr = new NArray();
            for (struct addrinfo* ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                inetAddr addr_info;
                addr_info.init(ptr->ai_addr);
                arr->append(addr_info.str());
            }

            resolver->_retVal = arr;
            resolver->_ac->post(0);

            uv_freeaddrinfo(res);
            delete resolver;
        },
        name.c_str(), NULL, &hints);

    if (r < 0) {
        delete resolver;
        return CHECK_ERROR(r);
    }

    return CALL_E_PENDDING;
}

result_t dns_base::lookup(exlib::string name, exlib::string& retVal, AsyncEvent* ac)
{
    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(exlib::string& retVal, AsyncEvent* ac)
            : _retVal(retVal)
            , _ac(ac)
        {
        }

    public:
        exlib::string& _retVal;
        AsyncEvent* _ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data(retVal, ac);
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;

            if (status < 0) {
                uv_freeaddrinfo(res);
                delete resolver;

                resolver->_ac->post(status);
                return;
            }

            inetAddr addr_info;
            addr_info.init(res->ai_addr);

            resolver->_retVal = addr_info.str();
            resolver->_ac->post(0);

            uv_freeaddrinfo(res);
            delete resolver;
        },
        name.c_str(), NULL, &hints);

    if (r < 0) {
        delete resolver;
        return CHECK_ERROR(r);
    }

    return CALL_E_PENDDING;
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

    class resolve_data : public uv_getaddrinfo_t {
    public:
        resolve_data(int32_t family, exlib::string& retVal, AsyncEvent* ac)
            : _family(family)
            , _retVal(retVal)
            , _ac(ac)
        {
        }

    public:
        int32_t _family;
        exlib::string& _retVal;
        AsyncEvent* _ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    addrinfo hints = { 0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0, 0 };

    resolve_data* resolver = new resolve_data(family, retVal, ac);
    int r = uv_getaddrinfo(
        s_uv_loop, resolver,
        [](uv_getaddrinfo_t* _resolver, int status, struct addrinfo* res) {
            resolve_data* resolver = (resolve_data*)_resolver;

            if (status < 0) {
                uv_freeaddrinfo(res);
                delete resolver;

                resolver->_ac->post(status);
                return;
            }

            struct addrinfo* ptr = NULL;
            for (ptr = res; ptr != NULL; ptr = ptr->ai_next) {
                if (ptr->ai_family == resolver->_family) {
                    inetAddr addr_info;
                    addr_info.init(ptr->ai_addr);
                    resolver->_retVal = addr_info.str();
                    break;
                }
            }

            if (ptr == NULL) {
#ifdef _WIN32
                resolver->_ac->post(-WSAHOST_NOT_FOUND);
#else
                resolver->_ac->post(-ETIME);
#endif
            } else
                resolver->_ac->post(0);

            uv_freeaddrinfo(res);
            delete resolver;
        },
        name.c_str(), NULL, &hints);

    if (r < 0) {
        delete resolver;
        return CHECK_ERROR(r);
    }

    return CALL_E_PENDDING;
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

    if (qstrcmp(url.c_str(), "tcp:", 4) && qstrcmp(url.c_str(), "unix:", 5) && qstrcmp(url.c_str(), "pipe:", 5))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!qstrcmp(url.c_str(), "tcp:", 4)) {
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

        retVal = socket;
        return socket->connect(u->m_hostname, nPort, timeout, ac);
    } else {
        obj_ptr<Socket_base> socket;

        result_t hr = Socket_base::_new(net_base::C_AF_UNIX, socket);
        if (hr < 0)
            return hr;

        retVal = socket;
        return socket->connect(url.substr(5), 0, timeout, ac);
    }
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
