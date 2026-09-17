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
    using TcpServer_base::stop; // 保留 JS API：stop(AsyncEvent* ac)

    // object_base：isolate 终止时的中断，只打断 accept 等待，不做 isolate_unref()。
    // 释放点：asyncAccept 状态机的 terminating 分支（isolate_unref()）。
    virtual result_t stop()
    {
        // 先置 false，使 accept 循环的 terminating 分支直接收尾，
        // 不再走 stop(NULL)（ac 为空）那条路径。
        m_running = false;

        // abort 底层监听 socket：pending accept 会立刻以错误返回，
        // 由 asyncAccept::error() 的 terminating 分支完成收尾与 unref。
        if (m_socket)
            m_socket->stop();

        return 0;
    }

public:
    // TcpServer_base
    virtual result_t start();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t listen(int32_t port, exlib::string addr, int32_t backlog, AsyncEvent* ac);
    virtual result_t address(obj_ptr<AddressType>& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
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

public:
    int32_t m_timeout = 0;
    object_base* m_eventDelegate = nullptr;

private:
    void clearEventDelegate()
    {
        m_eventDelegateRef.Release();
    }

private:
    bool m_running;
    obj_ptr<Socket_base> m_socket;
    obj_ptr<Handler_base> m_hdlr;
    obj_ptr<object_base> m_eventDelegateRef;
};

} /* namespace fibjs */
