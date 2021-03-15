/*
 * TcpServer.h
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/net.h"
#include "ifs/Handler.h"
#include "ifs/TcpServer.h"
#include "Socket.h"

namespace fibjs {

class TcpServer : public TcpServer_base {
    FIBER_FREE();

public:
    TcpServer();

public:
    // TcpServer_base
    virtual result_t start();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

public:
    class Holder : public object_base {
    public:
        v8::Global<v8::Value> m_server;
    };

public:
    result_t create(exlib::string addr, int32_t port, Handler_base* listener);

private:
    bool m_running;
    obj_ptr<Socket_base> m_socket;
    obj_ptr<Handler_base> m_hdlr;
};

} /* namespace fibjs */
