/*
 * UVSocket.cpp
 *
 *  Created on: Mar 29, 2021
 *      Author: lion
 */

#include "object.h"
#include "ifs/io.h"
#include "Socket.h"
#include "UVSocket.h"
#include "Buffer.h"
#include "EventInfo.h"

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

result_t UVSocket::abort()
{
    return UVStream_tmpl<Socket_base>::abort();
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
    obj_ptr<UVSocket> sock = new UVSocket(m_family, true); // accepted socket is already connected
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

result_t UVSocket::connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    class AsyncConnect : public uv_connect_t,
                         public UVTimeout {
    public:
        AsyncConnect(UVSocket* pThis, int32_t timeout, AsyncEvent* ac)
            : UVTimeout(pThis, timeout)
            , m_sock(pThis)
            , m_ac(ac)
        {
            // Already in uv loop thread, start timer immediately
            start_timer();
        }

        AsyncConnect(Isolate* isolate, UVSocket* pThis, int32_t timeout)
            : UVTimeout(pThis, timeout)
            , m_isolate(isolate)
            , m_sock(pThis)
            , m_ac(nullptr)
        {
            m_isolate->Ref();
            // Already in uv loop thread, start timer immediately
            start_timer();
        }

        ~AsyncConnect()
        {
            if (!m_ac)
                m_isolate->Unref();
        }

        static void callback(uv_connect_t* req, int status)
        {
            AsyncConnect* pThis = (AsyncConnect*)req;

            pThis->m_sock->on_connected(status < 0 ? status : 0);

            if (pThis->m_ac) {
                pThis->m_ac->apost(status);
            } else {
                if (status < 0)
                    (new EventInfo(pThis->m_sock, "error", status))->emit();
                else
                    (new EventInfo(pThis->m_sock, "connect"))->emit();
            }

            pThis->cancel_timer();
        }

    private:
        Isolate* m_isolate = nullptr;
        obj_ptr<UVSocket> m_sock;
        AsyncEvent* m_ac;
    };

    if (ac->isSync()) {
        startConnectEvent();
        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    retVal = this;
    if (m_family == net_base::C_AF_UNIX) {
        if (!m_connect_event)
            return uv_async([&] {
                uv_pipe_connect(new AsyncConnect(this, timeout, ac), &m_pipe, host.c_str(), AsyncConnect::callback);
                return 0;
            });

        uv_async([&] {
            uv_pipe_connect(new AsyncConnect(holder(), this, timeout), &m_pipe, host.c_str(), AsyncConnect::callback);
            return 0;
        });

        return 0;
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

        if (!m_connect_event)
            return uv_async([&] {
                return uv_tcp_connect(new AsyncConnect(this, timeout, ac), &m_tcp, (sockaddr*)&addr_info, AsyncConnect::callback);
            });

        uv_async([&] {
            return uv_tcp_connect(new AsyncConnect(holder(), this, timeout), &m_tcp, (sockaddr*)&addr_info, AsyncConnect::callback);
        });

        return 0;
    }
}

result_t UVSocket::connect(exlib::string path, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, timeout, retVal, ac);
}

result_t UVSocket::connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
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

result_t UVSocket::connect(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, "localhost", 0, connectListener, retVal, ac);
}

result_t UVSocket::connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, host, 0, connectListener, retVal, ac);
}

result_t UVSocket::connect(int32_t port, exlib::string host, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        v8::Local<v8::Object> _retVal;
        once("connect", connectListener, _retVal);
    }

    return connect(port, host, timeout, retVal, ac);
}

result_t UVSocket::connect(exlib::string path, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, 0, connectListener, retVal, ac);
}

result_t UVSocket::connect(exlib::string path, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(0, path, timeout, connectListener, retVal, ac);
}

result_t UVSocket::connect(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        v8::Local<v8::Object> _retVal;
        once("connect", connectListener, _retVal);
    }

    return connect(options, retVal, ac);
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

result_t UVSocket::send(Buffer_base* data, int32_t& retVal, AsyncEvent* ac)
{
    retVal = Buffer::Cast(data)->length();
    bool _retVal;
    return write(data, _retVal, ac);
}

result_t UVSocket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    uv_post(new AsyncRead(this, false, bytes, retVal, ac));
    return CALL_E_PENDDING;
}

result_t UVSocket::setKeepAlive(bool enable, int32_t initialDelay)
{
    if (m_family == net_base::C_AF_UNIX)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t hr = uv_tcp_keepalive(&m_tcp, enable ? 1 : 0, initialDelay);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}

result_t UVSocket::setNoDelay(bool noDelay)
{
    if (m_family == net_base::C_AF_UNIX)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    int32_t hr = uv_tcp_nodelay(&m_tcp, noDelay ? 1 : 0);
    if (hr < 0)
        return CHECK_ERROR(hr);

    return 0;
}
}
