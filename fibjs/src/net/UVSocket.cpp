/*
 * UVSocket.cpp
 *
 *  Created on: Mar 29, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "UVSocket.h"
#include "Buffer.h"

namespace fibjs {

result_t UVSocket::create(int32_t family, obj_ptr<Socket_base>& retVal)
{
    if (family != net_base::C_AF_INET
        && family != net_base::C_AF_INET6
        && family != net_base::C_AF_UNIX)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    obj_ptr<UVSocket> sock = new UVSocket(family);

    result_t hr = uv_call([&] {
        if (family == net_base::C_AF_UNIX)
            return uv_pipe_init(s_uv_loop, &sock->m_pipe, 0);
        else
            return uv_tcp_init(s_uv_loop, &sock->m_tcp);
    });
    if (hr < 0)
        return hr;

    retVal = sock;

    return 0;
}

result_t UVSocket::close(AsyncEvent* ac)
{
    if (ac && ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = UVStream_tmpl<Socket_base>::close(ac);

    m_lock.lock();

    m_socks.clear();
    while (m_accepts.size() > 0) {
        m_accepts.front().second->apost(CALL_E_INVALID_CALL);
        m_accepts.pop_front();
    }

    m_lock.unlock();

    return hr;
}

result_t UVSocket::get_family(int32_t& retVal)
{
    retVal = m_family;
    return 0;
}

result_t UVSocket::get_remoteAddress(exlib::string& retVal)
{
    inetAddr addr_info;
    int32_t sz = sizeof(addr_info);

    result_t hr = ::uv_tcp_getpeername(&m_tcp, (sockaddr*)&addr_info, &sz);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = addr_info.str();

    return 0;
}

result_t UVSocket::get_remotePort(int32_t& retVal)
{
    inetAddr addr_info;
    int32_t sz = sizeof(addr_info);

    result_t hr = ::uv_tcp_getpeername(&m_tcp, (sockaddr*)&addr_info, &sz);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = addr_info.port();

    return 0;
}

result_t UVSocket::get_localAddress(exlib::string& retVal)
{
    inetAddr addr_info;
    int32_t sz = sizeof(addr_info);

    result_t hr = ::uv_tcp_getsockname(&m_tcp, (sockaddr*)&addr_info, &sz);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = addr_info.str();

    return 0;
}

result_t UVSocket::get_localPort(int32_t& retVal)
{
    inetAddr addr_info;
    int32_t sz = sizeof(addr_info);

    result_t hr = ::uv_tcp_getsockname(&m_tcp, (sockaddr*)&addr_info, &sz);
    if (hr < 0)
        return CHECK_ERROR(hr);

    retVal = addr_info.port();

    return 0;
}

result_t UVSocket::bind(exlib::string addr, int32_t port, bool allowIPv4)
{
    if (m_family == net_base::C_AF_UNIX) {
#ifndef _WIN32
        ::unlink(addr.c_str());
#endif
        return uv_pipe_bind(&m_pipe, addr.c_str());
    } else {
        inetAddr addr_info;

        addr_info.init(m_family);
        addr_info.setPort(port);
        if (addr_info.addr(addr) < 0)
            return CHECK_ERROR(CALL_E_INVALIDARG);

        return uv_tcp_bind(&m_tcp, (struct sockaddr*)&addr_info,
            m_family == net_base::C_AF_INET ? 0 : (allowIPv4 ? 0 : UV_TCP_IPV6ONLY));
    }
}

result_t UVSocket::bind(int32_t port, bool allowIPv4)
{
    return bind("", port, allowIPv4);
}

void UVSocket::on_listen(int status)
{
    obj_ptr<UVSocket> sock = new UVSocket(m_family);
    int32_t ret;

    if (sock->m_family == net_base::C_AF_UNIX)
        uv_pipe_init(s_uv_loop, &sock->m_pipe, 0);
    else
        uv_tcp_init(s_uv_loop, &sock->m_tcp);

    ret = uv_accept(&m_stream, &sock->m_stream);
    if (ret < 0) {
        puts(uv_strerror(ret));
        return;
    }

    m_lock.lock();

    if (m_accepts.size() > 0) {
        std::pair<obj_ptr<Socket_base>&, AsyncEvent*>& _pair = m_accepts.front();

        _pair.first = sock;
        _pair.second->apost(0);

        m_accepts.pop_front();
    } else if (m_socks.size() < 256) {
        m_socks.push_back(sock);
    }

    m_lock.unlock();
}

void UVSocket::on_listen(uv_stream_t* server, int status)
{
    if (status >= 0)
        container_of(server, UVSocket, m_stream)->on_listen(status);
}

result_t UVSocket::listen(int32_t backlog)
{
    return uv_call([&] {
        return uv_listen(&m_stream, backlog, on_listen);
    });
}

result_t UVSocket::connect(exlib::string host, int32_t port, AsyncEvent* ac)
{
    class AsyncConnect : public uv_connect_t,
                         public UVTimeout {
    public:
        AsyncConnect(UVSocket* pThis, AsyncEvent* ac)
            : UVTimeout(pThis)
            , m_ac(ac)
        {
        }

        static void callback(uv_connect_t* req, int status)
        {
            AsyncConnect* pThis = (AsyncConnect*)req;

            pThis->m_ac->apost(status);
            pThis->cancel_timer();
        }

    public:
        AsyncEvent* m_ac;
    };

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_family == net_base::C_AF_UNIX) {
        return uv_async([&] {
            uv_pipe_connect(new AsyncConnect(this, ac), &m_pipe, host.c_str(), AsyncConnect::callback);
            return 0;
        });
    } else {
        inetAddr addr_info;

        addr_info.init(m_family);
        addr_info.setPort(port);
        if (addr_info.addr(host) < 0) {
            exlib::string strAddr;
            result_t hr = net_base::cc_resolve(host, m_family, strAddr);
            if (hr < 0)
                return hr;

            if (addr_info.addr(strAddr) < 0)
                return CHECK_ERROR(CALL_E_INVALIDARG);
        }

        return uv_async([&] {
            return uv_tcp_connect(new AsyncConnect(this, ac), &m_tcp, (sockaddr*)&addr_info, AsyncConnect::callback);
        });
    }
}

result_t UVSocket::accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = 0;

    m_lock.lock();

    if (uv_is_closing(&m_handle))
        hr = CHECK_ERROR(CALL_E_INVALID_CALL);
    else if (m_socks.size() > 0) {
        retVal = m_socks.front();
        m_socks.pop_front();
    } else {
        m_accepts.push_back(std::pair<obj_ptr<Socket_base>&, AsyncEvent*>(retVal, ac));
        hr = CALL_E_PENDDING;
    }

    m_lock.unlock();

    return hr;
}

result_t UVSocket::send(Buffer_base* data, AsyncEvent* ac)
{
    return write(data, ac);
}

result_t UVSocket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    uv_post(new AsyncRead(this, false, bytes, retVal, ac));
    return CALL_E_PENDDING;
}
}
