/*
 * UVSocket.h
 *
 *  Created on: Mar 29, 2021
 *      Author: lion
 */

#pragma once

#include "ifs/Socket.h"
#include "inetAddr.h"
#include "AsyncUV.h"
#include "UVStream.h"

namespace fibjs {

class UVSocket : public UVStream_tmpl<Socket_base> {
    FIBER_FREE();

public:
    UVSocket(int32_t family)
        : m_family(family)
    {
    }

public:
    // Stream_base
    virtual result_t close(AsyncEvent* ac);

public:
    // Socket_base
    virtual result_t get_family(int32_t& retVal);
    virtual result_t get_remoteAddress(exlib::string& retVal);
    virtual result_t get_remotePort(int32_t& retVal);
    virtual result_t get_localAddress(exlib::string& retVal);
    virtual result_t get_localPort(int32_t& retVal);
    virtual result_t connect(exlib::string host, int32_t port, int32_t timeout, AsyncEvent* ac);
    virtual result_t bind(exlib::string addr, int32_t port, bool allowIPv4);
    virtual result_t bind(int32_t port, bool allowIPv4);
    virtual result_t listen(int32_t backlog);
    virtual result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac);
    virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t send(Buffer_base* data, AsyncEvent* ac);

public:
    static result_t create(int32_t family, obj_ptr<Socket_base>& retVal);

private:
    static void on_listen(uv_stream_t* server, int status);
    void on_listen(int status);

private:
    int32_t m_family;
    exlib::spinlock m_lock;
    std::list<obj_ptr<UVSocket>> m_socks;
    std::list<std::pair<obj_ptr<Socket_base>&, AsyncEvent*>> m_accepts;
};
}
