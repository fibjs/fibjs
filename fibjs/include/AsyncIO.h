/*
 * AsyncIO.h
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#pragma once

#include "AsyncCall.h"
#include "Buffer.h"
#include "ifs/net.h"
#include "ifs/Socket.h"
#include "Timer.h"
#include "inetAddr.h"

namespace fibjs {

#define KEEPALIVE_TIMEOUT 120
#define SOCKET_BUFF_SIZE 2048

class AsyncIO {
public:
    AsyncIO(intptr_t s, int32_t family)
        : m_fd(s)
        , m_family(family)
#ifndef _WIN32
        , m_RecvOpt(NULL)
        , m_SendOpt(NULL)
#endif
    {
    }

public:
    result_t connect(exlib::string host, int32_t port, AsyncEvent* ac, Timer_base* timer);
    result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac);
    result_t write(Buffer_base* data, AsyncEvent* ac);
    result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac, bool bRead, Timer_base* timer);

#ifndef _WIN32
    result_t close(AsyncEvent* ac);
#else
    result_t close(AsyncEvent* ac)
    {
        if (m_fd != INVALID_SOCKET)
            ::closesocket(m_fd);

        m_fd = INVALID_SOCKET;

        return 0;
    }
#endif

    static void run(void (*proc)(void*));

public:
    intptr_t m_fd;
    int32_t m_family;

private:
    exlib::Locker m_lockRecv;
    exlib::Locker m_lockSend;

    obj_ptr<Timer_base> m_timer;

#ifndef _WIN32
    void* m_RecvOpt;
    void* m_SendOpt;
#endif
};
}
