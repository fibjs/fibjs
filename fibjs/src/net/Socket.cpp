/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "Socket.h"
#include "Buffer.h"
#include "Stat.h"
#include <string.h>
#include <fcntl.h>

namespace fibjs {

result_t Socket_base::_new(int32_t family, int32_t type,
    obj_ptr<Socket_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Socket> sock = new Socket();

    result_t hr = sock->create(family, type);
    if (hr < 0)
        return hr;

    retVal = sock;
    return 0;
}

Socket::~Socket()
{
    if (m_aio.m_fd != INVALID_SOCKET)
        asyncCall(::closesocket, m_aio.m_fd);
}

#ifdef _WIN32
extern HANDLE s_hIocp;
#endif

result_t Socket::create(int32_t family, int32_t type)
{
    if (m_aio.m_fd != INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_aio.m_family = family;
    m_aio.m_type = type;

    if (family == net_base::_AF_LOCAL)
        family = AF_LOCAL;
    else if (family == net_base::_AF_INET)
        family = AF_INET;
    else if (family == net_base::_AF_INET6)
        family = AF_INET6;
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (type == net_base::_SOCK_STREAM)
        type = SOCK_STREAM;
    else if (type == net_base::_SOCK_DGRAM)
        type = SOCK_DGRAM;
    else
        return CHECK_ERROR(CALL_E_INVALIDARG);

#ifdef _WIN32

    if (m_aio.m_family == net_base::_AF_LOCAL) {
        m_aio.m_type = SOCKET_TYPE_NAMED_PIPE;
        return 0;
    }

    m_aio.m_fd = WSASocketW(family, type, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(SocketError());

    CreateIoCompletionPort((HANDLE)m_aio.m_fd, s_hIocp, 0, 0);

#else

    m_aio.m_fd = socket(family, type, 0);
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(SocketError());

    fcntl(m_aio.m_fd, F_SETFL, fcntl(m_aio.m_fd, F_GETFL, 0) | O_NONBLOCK);
    fcntl(m_aio.m_fd, F_SETFD, FD_CLOEXEC);

#endif

    if (type == SOCK_DGRAM) {
        int broadcastEnable = 1;
        setsockopt(m_aio.m_fd, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcastEnable,
            sizeof(broadcastEnable));
    }

#ifdef Darwin

    int32_t set_option = 1;
    setsockopt(m_aio.m_fd, SOL_SOCKET, SO_NOSIGPIPE, &set_option,
        sizeof(set_option));

#endif

    return 0;
}

result_t Socket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    obj_ptr<Timer> timer;
    if (ac->isAsync() && m_timeout > 0) {
        timer = new IOTimer(m_timeout, this);
        timer->sleep();
    }

    return m_aio.read(bytes, retVal, ac, bytes > 0, timer);
}

result_t Socket::write(Buffer_base* data, AsyncEvent* ac)
{
    return m_aio.write(data, ac);
}

result_t Socket::flush(AsyncEvent* ac)
{
    return 0;
}

result_t Socket::copyTo(Stream_base* stm, int64_t bytes,
    int64_t& retVal, AsyncEvent* ac)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    return io_base::copyStream(this, stm, bytes, retVal, ac);
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

result_t Socket::get_type(int32_t& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    retVal = m_aio.m_type;

    return 0;
}

result_t Socket::get_remoteAddress(exlib::string& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    union s {
        inetAddr ia;
#ifndef _WIN32
        unixAddr ua;
#endif
    } addr_info;
    socklen_t sz;

    if (m_aio.m_family == net_base::_AF_LOCAL)
#ifdef _WIN32
    {
        retVal = m_aio.m_NamedPipePath;
        return 0;
    }
#else
        sz = sizeof(addr_info.ua);
#endif
    else
        sz = sizeof(addr_info.ia);

    if (::getpeername(m_aio.m_fd, (sockaddr*)&addr_info, &sz) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    if (m_aio.m_family == net_base::_AF_LOCAL)
#ifdef _WIN32
        return 0;
#else
        retVal = addr_info.ua.str();
#endif
    else
        retVal = addr_info.ia.str();

    return 0;
}

result_t Socket::get_remotePort(int32_t& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_aio.m_family == net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("socket family error, should not be AF_LOCAL"));

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

    union s {
        inetAddr ia;
#ifndef _WIN32
        unixAddr ua;
#endif
    } addr_info;
    socklen_t sz;

    if (m_aio.m_family == net_base::_AF_LOCAL)
#ifdef _WIN32
    {
        retVal = m_aio.m_NamedPipePath;
        return 0;
    }
#else
        sz = sizeof(addr_info.ua);
#endif
    else
        sz = sizeof(addr_info.ia);

    if (::getsockname(m_aio.m_fd, (sockaddr*)&addr_info, &sz) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    if (m_aio.m_family == net_base::_AF_LOCAL)
#ifdef _WIN32
        return 0;
#else
        retVal = addr_info.ua.str();
#endif
    else
        retVal = addr_info.ia.str();

    return 0;
}

result_t Socket::get_localPort(int32_t& retVal)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_aio.m_family == net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("socket family error, should not be AF_LOCAL"));

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

#ifdef _WIN32
result_t Socket::bind(exlib::string path, BOOL firstInstance, BOOL bClient, intptr_t& retFd)
{
    if (bClient) {
        HANDLE s;
        while (1) {
            s = open_named_pipe(path);
            if (s != INVALID_HANDLE_VALUE)
                break;
            if (GetLastError() != ERROR_PIPE_BUSY)
                return CHECK_ERROR(LastError());

            WaitNamedPipeW(UTF8_W(path), 3000);

            SwitchToThread();
        }
        m_aio.m_fd = (SOCKET)s;
        m_aio.m_type = -1;
    } else {
        m_aio.m_fd = (SOCKET)CreateNamedPipeW(UTF8_W(path),
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED | (firstInstance ? FILE_FLAG_FIRST_PIPE_INSTANCE : 0) | WRITE_DAC,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES, 65536, 65536, 0, NULL);
        if (m_aio.m_fd == (SOCKET)INVALID_HANDLE_VALUE)
            return CHECK_ERROR(LastError());
    }
    retFd = m_aio.m_fd;

    int32_t mode = PIPE_READMODE_BYTE | PIPE_WAIT;
    int32_t current_mode = 0;
    int32_t err;
    if (!SetNamedPipeHandleState((HANDLE)m_aio.m_fd, (LPDWORD)&mode, NULL, NULL)) {
        err = GetLastError();
        if (err == ERROR_ACCESS_DENIED) {
            if (!GetNamedPipeHandleState((HANDLE)m_aio.m_fd, (LPDWORD)&current_mode, NULL, NULL,
                    NULL, NULL, 0)) {
                return CHECK_ERROR(LastError());
            } else if (current_mode & PIPE_NOWAIT) {
                return CHECK_ERROR(-err);
            }
        } else if (err == ERROR_INVALID_PARAMETER) {
            return CHECK_ERROR(Runtime::setError("Not a named pipe"));
        }
    }
    CreateIoCompletionPort((HANDLE)m_aio.m_fd, s_hIocp, 0, 0);

    m_aio.m_NamedPipePath = path;
    m_bBind = TRUE;
    return 0;
}

HANDLE Socket::open_named_pipe(exlib::string path)
{
    HANDLE s;
    s = CreateFileW(UTF8_W(path),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL);
    if (s != INVALID_HANDLE_VALUE)
        return s;

    if (GetLastError() == ERROR_ACCESS_DENIED) {
        s = CreateFileW(UTF8_W(path),
            GENERIC_READ | FILE_WRITE_ATTRIBUTES,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL);

        if (s != INVALID_HANDLE_VALUE)
            return s;
    }

    if (GetLastError() == ERROR_ACCESS_DENIED) {
        s = CreateFileW(UTF8_W(path),
            GENERIC_WRITE | FILE_READ_ATTRIBUTES,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            NULL);

        if (s != INVALID_HANDLE_VALUE)
            return s;
    }

    return INVALID_HANDLE_VALUE;
}
#endif

result_t Socket::bind(exlib::string path)
{
#ifndef _WIN32
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_aio.m_family != net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("Socket family error, should be AF_LOCAL"));

    unixAddr addr_info;

    addr_info.init();
    if (addr_info.addr(path) < 0)
        return CHECK_ERROR(Runtime::setError("Path should be fullpath"));

    if (::bind(m_aio.m_fd, (struct sockaddr*)&addr_info, addr_info.size())
        == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    return 0;
#else
    return bind(path, TRUE, FALSE, m_aio.m_fd);
#endif
}

result_t Socket::bind(exlib::string addr, int32_t port, bool allowIPv4)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_aio.m_family == net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("socket family error, should not be AF_LOCAL"));

    inetAddr addr_info;

    addr_info.init(m_aio.m_family);
    addr_info.setPort(port);
    if (addr_info.addr(addr) < 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t on = 1;
#ifndef _WIN32
    setsockopt(m_aio.m_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
#endif

    if (m_aio.m_family == net_base::_AF_INET6) {
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

    if (m_aio.m_family == net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("socket family error, should not be AF_LOCAL"));

    return bind("", port, allowIPv4);
}

result_t Socket::listen(int32_t backlog)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

#ifdef _WIN32
    if (m_aio.m_family == net_base::_AF_LOCAL) {
        m_aio.m_bListen = TRUE;
        return 0;
    }
#endif

    if (::listen(m_aio.m_fd, backlog) == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    return 0;
}

result_t Socket::connect(exlib::string path, AsyncEvent* ac)
{
#ifdef _WIN32
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    result_t hr = bind(path, FALSE, TRUE, m_aio.m_fd);
    if (hr < 0)
        return hr;
    return 0;
#else
    obj_ptr<Timer> timer;
    if (ac->isAsync() && m_timeout > 0) {
        timer = new IOTimer(m_timeout, this);
        timer->sleep();
    }
    return m_aio.connect(path, ac, timer);
#endif
}

result_t Socket::connect(exlib::string host, int32_t port, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

#ifdef _WIN32
    if (!m_bBind) {
        result_t hr = bind(0, TRUE);
        if (hr < 0)
            return hr;
    }
#endif

    obj_ptr<Timer> timer;
    if (ac->isAsync() && m_timeout > 0) {
        timer = new IOTimer(m_timeout, this);
        timer->sleep();
    }

    return m_aio.connect(host, port, ac, timer);
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac)
{
    return m_aio.accept(retVal, ac);
}

result_t Socket::send(Buffer_base* data, AsyncEvent* ac)
{
    return m_aio.write(data, ac);
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    obj_ptr<Timer> timer;
    if (ac->isAsync() && m_timeout > 0) {
        timer = new IOTimer(m_timeout, this);
        timer->sleep();
    }

    return m_aio.read(bytes, retVal, ac, false, timer);
}

result_t Socket::recvfrom(int32_t bytes, obj_ptr<NObject>& retVal, AsyncEvent* ac)
{
    if (m_aio.m_family == net_base::_AF_LOCAL)
#ifdef _WIN32
        return CHECK_ERROR(Runtime::setError("Protocol error, named pipe not support udp protocol"));
#else
        return m_aio.recvfrom(bytes, retVal, ac);
#endif
    return m_aio.recvfrom(bytes, retVal, ac);
}

result_t Socket::sendto(Buffer_base* data, exlib::string path, AsyncEvent* ac)
{
#ifdef _WIN32
    return CHECK_ERROR(Runtime::setError("Protocol error, named pipe not support udp protocol"));
#else
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_aio.m_family != net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("Socket family error, should be AF_LOCAL"));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    unixAddr addr_info;

    addr_info.init();
    if (addr_info.addr(path) < 0)
        return CHECK_ERROR(Runtime::setError("Path should be fullpath"));

    exlib::string strData;
    data->toString(strData);

    if (::sendto(m_aio.m_fd, strData.c_str(), (int32_t)strData.length(), 0, (sockaddr*)&addr_info,
            addr_info.size())
        == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());
#endif

    return 0;
}

result_t Socket::sendto(Buffer_base* data, exlib::string host, int32_t port,
    AsyncEvent* ac)
{
    if (m_aio.m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (m_aio.m_family == net_base::_AF_LOCAL)
        return CHECK_ERROR(Runtime::setError("socket family error, should not be AF_LOCAL"));

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    inetAddr addr_info;

    addr_info.init(m_aio.m_family);
    addr_info.setPort(port);
    if (addr_info.addr(host.c_str()) < 0) {
        exlib::string strAddr;
        result_t hr = net_base::cc_resolve(host, m_aio.m_family, strAddr);
        if (hr < 0)
            return hr;

        if (addr_info.addr(strAddr.c_str()) < 0)
            return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    exlib::string strData;
    data->toString(strData);

    if (::sendto(m_aio.m_fd, strData.c_str(), (int32_t)strData.length(), 0, (sockaddr*)&addr_info,
            addr_info.size())
        == SOCKET_ERROR)
        return CHECK_ERROR(SocketError());

    return 0;
}

result_t Socket::unbind(obj_ptr<object_base>& retVal)
{
    return unbind_dispose(retVal);
}
}
