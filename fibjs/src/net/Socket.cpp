/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "Socket.h"
#include "UVSocket.h"
#include "Buffer.h"
#include "Stat.h"
#include <string.h>
#include <fcntl.h>
#include "options.h"
#include "EventInfo.h"

namespace fibjs {

result_t Socket_base::_new(int32_t family, obj_ptr<Socket_base>& retVal,
    v8::Local<v8::Object> This)
{
    if (g_uv_socket || family == net_base::C_AF_UNIX)
        return UVSocket::create(family, retVal);
    else
        return Socket::create(family, retVal);
}

Socket::~Socket()
{
    if (m_aio.m_fd != INVALID_SOCKET)
        async([fd = m_aio.m_fd]() {
            ::closesocket(fd);
        });
}

#ifdef _WIN32
extern HANDLE s_hIocp;
#endif

result_t Socket::create(int32_t family, obj_ptr<Socket_base>& retVal)
{
    obj_ptr<Socket> sock = new Socket();

    result_t hr = sock->create(family);
    if (hr < 0)
        return hr;

    retVal = sock;

    return 0;
}

result_t Socket::create(int32_t family)
{
    if (m_aio.m_fd != INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_aio.m_family = family;

    if (family == net_base::C_AF_INET)
        family = AF_INET;
    else if (family == net_base::C_AF_INET6)
        family = AF_INET6;
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

#ifdef _WIN32
    m_aio.m_fd = WSASocketW(family, SOCK_STREAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(SocketError());

    CreateIoCompletionPort((HANDLE)m_aio.m_fd, s_hIocp, 0, 0);
#else
    m_aio.m_fd = socket(family, SOCK_STREAM, 0);
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(SocketError());

    fcntl(m_aio.m_fd, F_SETFL, fcntl(m_aio.m_fd, F_GETFL, 0) | O_NONBLOCK);
    fcntl(m_aio.m_fd, F_SETFD, FD_CLOEXEC);
#endif

#ifdef Darwin
    int32_t set_option = 1;
    setsockopt(m_aio.m_fd, SOL_SOCKET, SO_NOSIGPIPE, &set_option,
        sizeof(set_option));
#endif

    return 0;
}

result_t Socket::get_fd(int32_t& retVal)
{
    retVal = (int32_t)m_aio.m_fd;
    return 0;
}

result_t Socket::readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    return m_aio.read(bytes, retVal, ac, bytes > 0, m_timeout);
}

result_t Socket::write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    return m_aio.write(data, retVal, ac, m_timeout);
}

result_t Socket::flush(AsyncEvent* ac)
{
    return 0;
}

result_t Socket::close(AsyncEvent* ac)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return 0;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return m_aio.close(ac);
}

result_t Socket::get_family(int32_t& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_aio.m_family;

    return 0;
}

result_t Socket::get_remoteAddress(exlib::string& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    inetAddr addr_info;
    socklen_t sz = sizeof(addr_info);

    if (::getpeername(m_aio.m_fd, (sockaddr*)&addr_info, &sz) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    retVal = addr_info.str();

    return 0;
}

result_t Socket::get_remotePort(int32_t& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    inetAddr addr_info;
    socklen_t sz = sizeof(addr_info);

    if (::getpeername(m_aio.m_fd, (sockaddr*)&addr_info, &sz) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    retVal = addr_info.port();

    return 0;
}

result_t Socket::get_localAddress(exlib::string& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    inetAddr addr_info;
    socklen_t sz = sizeof(addr_info);

    if (::getsockname(m_aio.m_fd, (sockaddr*)&addr_info, &sz) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    retVal = addr_info.str();

    return 0;
}

result_t Socket::get_localPort(int32_t& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    inetAddr addr_info;
    socklen_t sz = sizeof(addr_info);

    if (::getsockname(m_aio.m_fd, (sockaddr*)&addr_info, &sz) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    retVal = addr_info.port();

    return 0;
}

result_t Socket::get_timeout(int32_t& retVal)
{
    retVal = m_timeout;
    return 0;
}
result_t Socket::set_timeout(int32_t newVal)
{
    m_timeout = newVal;
    return 0;
}

result_t Socket::abort()
{
    m_aio.abort();
    return 0;
}

result_t Socket::bind(exlib::string addr, int32_t port, bool allowIPv4)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    inetAddr addr_info;

    addr_info.init(m_aio.m_family);
    addr_info.setPort(port);
    if (addr_info.addr(addr) < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t on = 1;
#ifndef _WIN32
    setsockopt(m_aio.m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
#endif

    if (m_aio.m_family == net_base::C_AF_INET6) {
        if (allowIPv4)
            on = 0;

        setsockopt(m_aio.m_fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&on,
            sizeof(on));
    }

    if (::bind(m_aio.m_fd, (struct sockaddr*)&addr_info, addr_info.size())
        == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

#ifdef _WIN32
    m_bBind = TRUE;
#endif

    return 0;
}

result_t Socket::bind(int32_t port, bool allowIPv4)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return bind("", port, allowIPv4);
}

result_t Socket::listen(int32_t backlog)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (::listen(m_aio.m_fd, backlog) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    return 0;
}

class connectWrapper : public AsyncEvent {
public:
    connectWrapper(Socket* sock, AsyncEvent* ac)
        : m_sock(sock)
        , m_ac(ac)
    {
        setAsync();
    }

    connectWrapper(Isolate* isolate, Socket* sock)
        : AsyncEvent(isolate)
        , m_sock(sock)
        , m_ac(nullptr)
    {
        setAsync();
        m_isolate->Ref();
    }

    ~connectWrapper()
    {
        if (!m_ac)
            m_isolate->Unref();
    }

    virtual int32_t post(int32_t v)
    {
        if (m_ac) {
            m_sock->on_connected(v < 0 ? v : 0);
            m_ac->post(v);
        } else {
            m_sock->on_connected(v < 0 ? v : 0);
            if (v < 0)
                (new EventInfo(m_sock, "error", v))->emit();
            else
                (new EventInfo(m_sock, "connect"))->emit();
        }
        delete this;
        return 0;
    }

private:
    obj_ptr<Socket> m_sock;
    AsyncEvent* m_ac;
};

result_t Socket::connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
    {
        startConnectEvent();
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

#ifdef _WIN32
    if (!m_bBind) {
        result_t hr = bind(0, TRUE);
        if (hr < 0)
            return hr;
    }
#endif

    retVal = this;
    if (!m_connect_event)
        return m_aio.connect(host, port, new connectWrapper(this, ac), timeout);

    m_aio.connect(host, port, new connectWrapper(holder(), this), timeout);
    return 0;
}

result_t Socket::connect(exlib::string path, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, timeout, retVal, ac);
}

result_t Socket::connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        obj_ptr<ConnectOptions> opts;
        Isolate* isolate = Isolate::current(options);
        result_t hr = ConnectOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx.resize(1);
        ac->m_ctx[0] = opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    ConnectOptions* opt = (ConnectOptions*)ac->m_ctx[0].object();
    return connect(opt->port.value(), opt->host.value(), opt->timeout.value(), retVal, ac);
}

result_t Socket::connect(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, "localhost", 0, connectListener, retVal, ac);
}

result_t Socket::connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, host, 0, connectListener, retVal, ac);
}

result_t Socket::connect(int32_t port, exlib::string host, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        v8::Local<v8::Object> _retVal;
        once("connect", connectListener, _retVal);
    }

    return connect(port, host, timeout, retVal, ac);
}

result_t Socket::connect(exlib::string path, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, 0, connectListener, retVal, ac);
}

result_t Socket::connect(exlib::string path, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, timeout, connectListener, retVal, ac);
}

result_t Socket::connect(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        v8::Local<v8::Object> _retVal;
        once("connect", connectListener, _retVal);
    }

    return connect(options, retVal, ac);
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac)
{
    return m_aio.accept(retVal, ac);
}

result_t Socket::send(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    return m_aio.write(data, retVal, ac, m_timeout);
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    return m_aio.read(bytes, retVal, ac, false, m_timeout);
}

result_t Socket::unbind(obj_ptr<object_base>& retVal)
{
    return unbind_dispose(retVal);
}

extern void setKeepAlive(SOCKET sockfd, int32_t enable, int32_t initialDelay, int32_t keepInterval, int32_t keepCount);
extern void setNoDelay(SOCKET sockfd, int32_t enable);

result_t Socket::setKeepAlive(bool enable, int32_t initialDelay)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    fibjs::setKeepAlive(m_aio.m_fd, enable ? 1 : 0, initialDelay, 0, 0);
    return 0;
}

result_t Socket::setNoDelay(bool noDelay)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    fibjs::setNoDelay(m_aio.m_fd, noDelay ? 1 : 0);
    return 0;
}
}
