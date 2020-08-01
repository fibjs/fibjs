/*
 * AsyncIO_ev.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifndef _WIN32

#include "object.h"
#include "utils.h"
#include "AsyncIO.h"
#include "Socket.h"
#include "ifs/net.h"
#include "ifs/console.h"
#include "Buffer.h"
#include <ev/ev.h>
#include <fcntl.h>
#include <exlib/include/thread.h>
#include "options.h"
#include <sys/wait.h>

namespace fibjs {

void setOption(intptr_t& sockfd)
{
    int32_t keepAlive = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepAlive,
        sizeof(keepAlive));

#ifdef TCP_KEEPIDLE
    int32_t keepIdle = KEEPALIVE_TIMEOUT;
    setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
#endif

#ifdef TCP_KEEPINTVL
    int32_t keepInterval = 20;
    int32_t keepCount = 10;

    setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void*)&keepInterval,
        sizeof(keepInterval));
    setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void*)&keepCount, sizeof(keepCount));
#endif

    int32_t noDelay = 1;

    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void*)&noDelay, sizeof(noDelay));
}

static struct ev_loop* s_loop;

result_t net_base::backend(exlib::string& retVal)
{
    switch (ev_backend(s_loop)) {
    case EVBACKEND_SELECT:
        retVal = "Select";
        break;
    case EVBACKEND_POLL:
        retVal = "Poll";
        break;
    case EVBACKEND_EPOLL:
        retVal = "EPoll";
        break;
    case EVBACKEND_KQUEUE:
        retVal = "KQueue";
        break;
    case EVBACKEND_DEVPOLL:
        retVal = "DevPoll";
        break;
    case EVBACKEND_PORT:
        retVal = "Port";
        break;
    }

    return 0;
}

class evAsyncEvent;

static ev_async s_evAsyncWatcher;
static exlib::LockedList<evAsyncEvent> s_evWait;

class evAsyncEvent : public exlib::Task_base {
public:
    virtual ~evAsyncEvent()
    {
    }

    void post()
    {
        s_evWait.putTail(this);
        ev_async_send(s_loop, &s_evAsyncWatcher);
    }

    virtual void start()
    {
    }

public:
    virtual void resume()
    {
        post();
    }
};

class AsyncSockProc : public evAsyncEvent {
public:
    AsyncSockProc(intptr_t& sockfd, int32_t ev_op_t, AsyncEvent* ac, exlib::Locker& locker, void*& opt)
        : m_sockfd(sockfd)
        , m_ev_op_t(ev_op_t)
        , m_ac(ac)
        , m_locker(locker)
        , m_opt(opt)
    {
    }

    virtual void start()
    {
        if (m_sockfd == SOCKET_ERROR) {
            m_ac->apost(SOCKET_ERROR);
            delete this;
            return;
        }

        m_opt = this;

        ev_io_init(&m_io_watcher, io_cb, m_sockfd, m_ev_op_t);
        ev_io_start(s_loop, &m_io_watcher);
    }

public:
    result_t request()
    {
        if (m_locker.lock(this)) {
            result_t hr = process();
            if (hr != CALL_E_PENDDING) {
                m_locker.unlock(this);
                delete this;

                return hr;
            }

            post();
        }

        return CALL_E_PENDDING;
    }

    virtual result_t process()
    {
        return 0;
    }

    virtual void after_unwatch()
    {
        ready(process());
    }

    void ready(int32_t v)
    {
        m_opt = NULL;
        m_locker.unlock(this);
        m_ac->apost(v);
        delete this;
    }

    void on_watched()
    {
        ev_io_stop(s_loop, &m_io_watcher);
        after_unwatch();
    }

public:
    intptr_t& m_sockfd;
    int32_t m_ev_op_t;
    AsyncEvent* m_ac;
    exlib::Locker& m_locker;
    void*& m_opt;
    ev_io m_io_watcher;

private:
    static void io_cb(struct ev_loop* loop, struct ev_io* watcher, int32_t revents)
    {
        AsyncSockProc* p = NULL;
        ((AsyncSockProc*)((intptr_t)watcher - (intptr_t)&p->m_io_watcher))->on_watched();
    }
};

class _acIO : public exlib::OSThread {
public:
    _acIO()
    {
        m_lock.lock();
        s_loop = EV_DEFAULT;
    }

    virtual void Run()
    {
        Runtime rtForThread(NULL);

        ev_async_init(&s_evAsyncWatcher, as_cb);
        ev_async_start(s_loop, &s_evAsyncWatcher);

        m_lock.unlock();
        ev_run(s_loop, 0);
    }

private:
    static void as_cb(struct ev_loop* loop, struct ev_async* watcher,
        int32_t revents)
    {
        exlib::List<evAsyncEvent> jobs;
        evAsyncEvent* p1;

        s_evWait.getList(jobs);

        while ((p1 = jobs.getHead()) != 0)
            p1->start();
    }

public:
    exlib::spinlock m_lock;
};

void InitializeAsyncIOThread()
{
    static _acIO s_acIO;

    s_acIO.start();
    s_acIO.m_lock.lock();
}

result_t AsyncIO::close(AsyncEvent* ac)
{
    class asyncClose : public evAsyncEvent {
    public:
        asyncClose(intptr_t& sockfd, void*& recvProc, void*& sendProc, AsyncEvent* ac)
            : m_ac(ac)
            , m_sockfd(sockfd)
            , m_pRecvProc(recvProc)
            , m_pSendProc(sendProc)
        {
        }

        virtual void start()
        {
            if (m_sockfd != INVALID_SOCKET) {
                ::closesocket(m_sockfd);
                m_sockfd = INVALID_SOCKET;

                if (m_pRecvProc)
                    ((AsyncSockProc*)m_pRecvProc)->on_watched();

                if (m_pSendProc)
                    ((AsyncSockProc*)m_pSendProc)->on_watched();
            }

            m_ac->apost(0);
            delete this;
        }

    public:
        AsyncEvent* m_ac;
        intptr_t& m_sockfd;
        void*& m_pRecvProc;
        void*& m_pSendProc;
    };

    (new asyncClose(m_fd, m_RecvOpt, m_SendOpt, ac))->post();
    return CALL_E_PENDDING;
}

result_t AsyncIO::connect(exlib::string host, int32_t port, AsyncEvent* ac, Timer_base* timer)
{
    class asyncConnect : public AsyncSockProc {
    public:
        asyncConnect(intptr_t& sockfd, inetAddr& ai, AsyncEvent* ac, exlib::Locker& locker, void*& opt, Timer_base* timer)
            : AsyncSockProc(sockfd, EV_WRITE, ac, locker, opt)
            , m_ai(ai)
            , m_timer(timer)
        {
        }

        virtual result_t process()
        {
            int32_t n = ::connect(m_sockfd, (struct sockaddr*)&m_ai, m_ai.size());
            if (n == SOCKET_ERROR) {
                int32_t nError = errno;
                if (nError == EINPROGRESS)
                    return CHECK_ERROR(CALL_E_PENDDING);

                if (m_timer) {
                    m_timer->clear();
                    m_timer.Release();
                }
                return CHECK_ERROR(-nError);
            }

            if (m_timer) {
                m_timer->clear();
                m_timer.Release();
            }
            return 0;
        }

        virtual void after_unwatch()
        {
            inetAddr addr_info;
            socklen_t sz1 = sizeof(addr_info);

            if (m_timer) {
                m_timer->clear();
                m_timer.Release();
            }
            if (::getpeername(m_sockfd, (sockaddr*)&addr_info, &sz1) == SOCKET_ERROR)
                ready(-ECONNREFUSED);
            else {
                setOption(m_sockfd);
                ready(0);
            }
        }

    public:
        inetAddr m_ai;
        obj_ptr<Timer_base> m_timer;
    };

    if (m_fd == INVALID_SOCKET) {
        if (timer)
            timer->clear();
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    inetAddr addr_info;

    addr_info.init(m_family);
    addr_info.setPort(port);
    if (addr_info.addr(host) < 0) {
        exlib::string strAddr;
        result_t hr = net_base::cc_resolve(host, m_family, strAddr);
        if (hr < 0) {
            if (timer)
                timer->clear();
            return hr;
        }

        if (addr_info.addr(strAddr) < 0) {
            if (timer)
                timer->clear();
            return CHECK_ERROR(CALL_E_INVALIDARG);
        }
    }

    return (new asyncConnect(m_fd, addr_info, ac, m_lockRecv, m_RecvOpt, timer))->request();
}

result_t AsyncIO::accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac)
{
    class asyncAccept : public AsyncSockProc {
    public:
        asyncAccept(intptr_t& sockfd, obj_ptr<Socket_base>& retVal,
            AsyncEvent* ac, exlib::Locker& locker, void*& opt)
            : AsyncSockProc(sockfd, EV_READ, ac, locker, opt)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            inetAddr ai;
            socklen_t sz = sizeof(ai);
            intptr_t c = ::accept(m_sockfd, (sockaddr*)&ai, &sz);
            if (c == INVALID_SOCKET) {
                int32_t nError = errno;
                return CHECK_ERROR((nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError);
            }

            fcntl(c, F_SETFL, fcntl(c, F_GETFL, 0) | O_NONBLOCK);
            fcntl(c, F_SETFD, FD_CLOEXEC);

#ifdef Darwin
            int32_t set_option = 1;
            setsockopt(c, SOL_SOCKET, SO_NOSIGPIPE, &set_option,
                sizeof(set_option));
#endif
            setOption(c);

            obj_ptr<Socket> sock = new Socket(c, ai.family(),
                net_base::_SOCK_STREAM);

            m_retVal = sock;

            return 0;
        }

    public:
        obj_ptr<Socket_base>& m_retVal;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncAccept(m_fd, retVal, ac, m_lockRecv, m_RecvOpt))->request();
}

result_t AsyncIO::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac, bool bRead, Timer_base* timer)
{
    class asyncRecv : public AsyncSockProc {
    public:
        asyncRecv(intptr_t& sockfd, int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac,
            int32_t family, bool bRead, exlib::Locker& locker, void*& opt, Timer_base* timer)
            : AsyncSockProc(sockfd, EV_READ, ac, locker, opt)
            , m_retVal(retVal)
            , m_pos(0)
            , m_bytes(bytes > 0 ? bytes : SOCKET_BUFF_SIZE)
            , m_family(family)
            , m_bRead(bRead)
            , m_timer(timer)
        {
        }

        virtual result_t process()
        {
            if (m_buf.empty())
                m_buf.resize(m_bytes);

            do {
                int32_t n;

                if (m_family)
                    n = (int32_t)::recv(m_sockfd, &m_buf[m_pos], m_buf.length() - m_pos,
                        MSG_NOSIGNAL);
                else
                    n = (int32_t)::read(m_sockfd, &m_buf[m_pos], m_buf.length() - m_pos);
                if (n == SOCKET_ERROR) {
                    int32_t nError = errno;
                    if (nError == ECONNRESET)
                        n = 0;
                    else {
                        if (m_pos == 0)
                            m_buf.resize(0);

                        if (nError == EWOULDBLOCK)
                            return CHECK_ERROR(CALL_E_PENDDING);

                        if (m_timer) {
                            m_timer->clear();
                            m_timer.Release();
                        }
                        return CHECK_ERROR(-nError);
                    }
                }

                if (n == 0)
                    m_bRead = false;

                m_pos += n;
                if (m_pos == 0) {
                    if (m_timer) {
                        m_timer->clear();
                        m_timer.Release();
                    }
                    return CALL_RETURN_NULL;
                }
            } while (m_bRead && m_pos < (int32_t)m_buf.length());

            m_buf.resize(m_pos);
            m_retVal = new Buffer(m_buf);
            if (g_tcpdump)
                outLog(console_base::_NOTICE, clean_string(m_buf));

            return 0;
        }

        virtual void after_unwatch()
        {
            result_t hr = process();

            if (hr == CALL_E_PENDDING)
                post();
            else {
                if (m_timer) {
                    m_timer->clear();
                    m_timer.Release();
                }
                ready(hr);
            }
        }

    public:
        obj_ptr<Buffer_base>& m_retVal;
        int32_t m_pos;
        int32_t m_bytes;
        int32_t m_family;
        bool m_bRead;
        exlib::string m_buf;
        obj_ptr<Timer_base> m_timer;
    };

    if (m_fd == INVALID_SOCKET) {
        if (timer)
            timer->clear();
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRecv(m_fd, bytes, retVal, ac, m_family, bRead, m_lockRecv, m_RecvOpt, timer))->request();
}

result_t AsyncIO::write(Buffer_base* data, AsyncEvent* ac)
{
    class asyncSend : public AsyncSockProc {
    public:
        asyncSend(intptr_t& sockfd, Buffer_base* data, AsyncEvent* ac, int32_t family, exlib::Locker& locker, void*& opt)
            : AsyncSockProc(sockfd, EV_WRITE, ac, locker, opt)
            , m_family(family)
        {
            data->toString(m_buf);
            m_p = m_buf.c_str();
            m_sz = m_buf.length();

            if (g_tcpdump)
                outLog(console_base::_WARN, clean_string(m_buf));
        }

        virtual result_t process()
        {
            while (m_sz) {
                int32_t n;

                if (m_family)
                    n = (int32_t)::send(m_sockfd, m_p, m_sz, MSG_NOSIGNAL);
                else
                    n = (int32_t)::write(m_sockfd, m_p, m_sz);
                if (n == SOCKET_ERROR) {
                    int32_t nError = errno;
                    return CHECK_ERROR((nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError);
                }

                m_sz -= n;
                m_p += n;
            }

            return 0;
        }

        virtual void after_unwatch()
        {
            result_t hr = process();

            if (hr == CALL_E_PENDDING)
                post();
            else
                ready(hr);
        }

    public:
        exlib::string m_buf;
        const char* m_p;
        int32_t m_sz;
        int32_t m_family;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSend(m_fd, data, ac, m_family, m_lockSend, m_SendOpt))->request();
}

result_t AsyncIO::recvfrom(int32_t bytes, obj_ptr<NObject>& retVal,
    AsyncEvent* ac)
{
    class asyncRecvFrom : public AsyncSockProc {
    public:
        asyncRecvFrom(intptr_t& sockfd, int32_t bytes, obj_ptr<NObject>& retVal, AsyncEvent* ac,
            exlib::Locker& locker, void*& opt)
            : AsyncSockProc(sockfd, EV_READ, ac, locker, opt)
            , m_retVal(retVal)
            , m_bytes(bytes > 0 ? bytes : SOCKET_BUFF_SIZE)
        {
        }

        virtual result_t process()
        {
            if (m_buf.empty())
                m_buf.resize(m_bytes);

            int32_t n;
            inetAddr addr_info;
            socklen_t sz = sizeof(addr_info);

            n = (int32_t)::recvfrom(m_sockfd, &m_buf[0], m_buf.length(), MSG_NOSIGNAL,
                (sockaddr*)&addr_info, &sz);
            if (n == SOCKET_ERROR) {
                int32_t nError = errno;

                m_buf.resize(0);
                if (nError == EWOULDBLOCK)
                    return CHECK_ERROR(CALL_E_PENDDING);

                return CHECK_ERROR(-nError);
            }

            if (n == 0)
                return CALL_RETURN_NULL;

            m_buf.resize(n);
            m_retVal = new DatagramPacket(m_buf, addr_info);

            return 0;
        }

        virtual void after_unwatch()
        {
            result_t hr = process();

            if (hr == CALL_E_PENDDING)
                post();
            else
                ready(hr);
        }

    public:
        obj_ptr<NObject>& m_retVal;
        int32_t m_bytes;
        exlib::string m_buf;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRecvFrom(m_fd, bytes, retVal, ac, m_lockRecv, m_RecvOpt))->request();
}

void AsyncIO::run(void (*watchProc)(void*))
{
    class asyncRun : public evAsyncEvent {
    public:
        asyncRun(void (*watchProc)(void*))
            : m_proc(watchProc)
        {
        }

        virtual void start()
        {
            m_proc(s_loop);
            delete this;
        }

    public:
        void (*m_proc)(void*);
    };

    (new asyncRun(watchProc))->post();
}
}

#endif