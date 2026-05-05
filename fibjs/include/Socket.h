/*
 * Socket.h
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Socket.h"
#include "inetAddr.h"
#include "AsyncIO.h"
#include "Timer.h"
#include "AsyncStream.h"
#include <boost/preprocessor.hpp>
#include <functional>

namespace fibjs {

result_t socket_isAlive(SOCKET fd, bool& retVal);

class ConnectOptions : public obj_base {
public:
    LOAD_OPTIONS(ConnectOptions, (host)(port)(timeout));

public:
    std::optional<exlib::string> host = "localhost";
    std::optional<int32_t> port = 0;
    std::optional<int32_t> timeout = 0;
};

class Socket : public AsyncStream<Socket_base> {
    FIBER_FREE();

public:
    Socket(bool connected = false)
        : m_aio(INVALID_SOCKET, net_base::C_AF_INET)
        , m_timeout(0)
#ifdef _WIN32
        , m_bBind(FALSE)
#endif
    {
        if (!connected)
            m_state = 3;
    }

    Socket(SOCKET s, int32_t family)
        : m_aio(s, family)
        , m_timeout(0)
#ifdef _WIN32
        , m_bBind(FALSE)
#endif
    {
    }

    virtual ~Socket();

public:
    // object_base
    virtual result_t unbind(obj_ptr<object_base>& retVal);

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal);
    virtual result_t readBuffer(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac);
    virtual result_t writeBuffer(Buffer_base* data, AsyncEvent* ac);
    virtual result_t flush(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);

public:
    // Socket_base
    virtual result_t get_family(int32_t& retVal);
    virtual result_t get_remoteAddress(exlib::string& retVal);
    virtual result_t get_remotePort(int32_t& retVal);
    virtual result_t get_localAddress(exlib::string& retVal);
    virtual result_t get_localPort(int32_t& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t connect(int32_t port, exlib::string host, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(exlib::string path, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(int32_t port, exlib::string host, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(exlib::string path, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(exlib::string path, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t connect(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    virtual result_t bind(exlib::string addr, int32_t port, bool allowIPv4);
    virtual result_t bind(int32_t port, bool allowIPv4);
    virtual result_t listen(int32_t backlog);
    virtual result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac);
    virtual result_t setKeepAlive(bool enable, int32_t initialDelay);
    virtual result_t setNoDelay(bool noDelay);
    virtual result_t isAlive(bool& retVal);
    virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t send(Buffer_base* data, int32_t& retVal, AsyncEvent* ac);
    virtual result_t abort();
    virtual result_t setTimeout(int32_t timeout, obj_ptr<Socket_base>& retVal);
    virtual result_t setTimeout(int32_t timeout, v8::Local<v8::Function> callback, obj_ptr<Socket_base>& retVal);

public:
    static result_t create(int32_t family, obj_ptr<Socket_base>& retVal);

private:
    result_t create(int32_t family);

private:
    AsyncIO m_aio;
    int32_t m_timeout;

#ifdef _WIN32
    BOOL m_bBind;
#endif

    friend class AsyncIO;
};
}
