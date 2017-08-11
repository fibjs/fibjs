/*
 * TcpServer.h
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "ifs/net.h"
#include "ifs/Handler.h"
#include "ifs/TcpServer.h"
#include "Socket.h"
#include "Stats.h"

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

namespace fibjs {

class TcpServer : public TcpServer_base {
    FIBER_FREE();

public:
    TcpServer();

public:
    // object_base
    virtual result_t dispose()
    {
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

public:
    // TcpServer_base
    virtual result_t run(AsyncEvent* ac);
    virtual result_t asyncRun();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);
    virtual result_t get_stats(obj_ptr<Stats_base>& retVal);

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
    naked_ptr<Handler_base> m_hdlr;
    obj_ptr<Stats> m_stats;
};

} /* namespace fibjs */
#endif /* TCPSERVER_H_ */
