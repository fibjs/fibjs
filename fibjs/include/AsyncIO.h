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
#include <functional>

namespace fibjs {

#define KEEPALIVE_TIMEOUT 120
#define SOCKET_BUFF_SIZE 2048

// Lightweight timer for AsyncIO timeout handling
// Inherits from obj_base for reference counting, and Task_base for fiber sleep
class AsyncIOTimer : public obj_base, public exlib::Task_base {
public:
    AsyncIOTimer(int32_t timeout, void* data, std::function<void(AsyncIOTimer*)> callback)
        : m_timeout(timeout)
        , m_callback(callback)
        , m_cancelled(0)
        , m_data(data)
    {
    }

    void start()
    {
        if (m_timeout > 0)
            exlib::Fiber::sleep(m_timeout, this);
    }

    void cancel()
    {
        if (m_cancelled.CompareAndSwap(0, 1) == 0) {
            // Prevent this object from being destroyed before resume() is called.
            // The timer thread will call resume() when processing the cancel request.
            Ref();
            exlib::Fiber::cancel_sleep(this);
        }
    }

    bool is_cancelled() const
    {
        return m_cancelled != 0;
    }

    // exlib::Task_base
    virtual void resume()
    {
        if (m_cancelled == 0 && m_callback) {
            m_callback(this);
            m_callback = nullptr;
        } else if (m_cancelled != 0) {
            // Release the reference added in cancel()
            Unref();
        }
    }

    template <typename T>
    T* get_data() const { return static_cast<T*>(m_data); }

private:
    int32_t m_timeout;
    std::function<void(AsyncIOTimer*)> m_callback;
    exlib::atomic m_cancelled;
    void* m_data = nullptr;
};

class AsyncIO {
public:
    AsyncIO(intptr_t s, int32_t family)
        : m_fd(s)
        , m_family(family)
        , m_abort_version(0)
#ifndef _WIN32
        , m_RecvOpt(NULL)
        , m_SendOpt(NULL)
#endif
    {
    }

public:
    result_t connect(exlib::string host, int32_t port, AsyncEvent* ac, int32_t timeout = 0);
    result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac);
    result_t write(Buffer_base* data, AsyncEvent* ac, int32_t timeout = 0);
    result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac, bool bRead, int32_t timeout = 0);
    
    // Abort all pending operations
    void abort();
    intptr_t get_abort_version() const { return m_abort_version.value(); }

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
    exlib::atomic m_abort_version;  // Incremented on each abort() call

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
