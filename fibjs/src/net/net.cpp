/*
 * net.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/tls.h"
#include "ifs/os.h"
#include "inetAddr.h"
#include "Url.h"
#include "options.h"
#include "AsyncUV.h"
#include "Socket.h"

namespace fibjs {

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
        return tls_base::connect(url, timeout, retVal, ac);

    if (qstrcmp(url.c_str(), "tcp:", 4) && qstrcmp(url.c_str(), "unix:", 5) && qstrcmp(url.c_str(), "pipe:", 5))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (!qstrcmp(url.c_str(), "tcp:", 4)) {
        obj_ptr<Url> u = new Url();

        result_t hr = u->parse(url);
        if (hr < 0)
            return hr;

        exlib::string port = u->port();
        if (port.length() == 0)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        int32_t nPort = atoi(port.c_str());
        int32_t family = u->isIPv6() ? net_base::C_AF_INET6 : net_base::C_AF_INET;

        obj_ptr<Socket_base> socket;

        hr = Socket_base::_new(family, socket);
        if (hr < 0)
            return hr;

        retVal = socket;
        return socket->connect(nPort, u->hostname(), timeout, ac);
    } else {
        obj_ptr<Socket_base> socket;

        result_t hr = Socket_base::_new(net_base::C_AF_UNIX, socket);
        if (hr < 0)
            return hr;

        retVal = socket;
        return socket->connect(url.substr(5), timeout, ac);
    }
}

result_t net_base::connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    bool is_ipv6 = false;
    isIPv6(host, is_ipv6);
    int32_t family = is_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET;

    obj_ptr<Socket_base> socket;
    result_t hr = Socket_base::_new(family, socket);
    if (hr < 0)
        return hr;

    retVal = socket;
    return socket->connect(port, host, timeout, ac);
}

result_t net_base::connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<ConnectOptions> opts;
        Isolate* isolate = Isolate::current(options);
        result_t hr = ConnectOptions::load(isolate, options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    ConnectOptions* opt = (ConnectOptions*)ac->m_ctx[0].object();
    return connect(opt->port.value(), opt->host.value(), opt->timeout.value(), retVal, ac);
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
