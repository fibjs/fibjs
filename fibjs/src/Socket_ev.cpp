/*
 * Socket_ev.cpp
 *
 *  Created on: Apr 26, 2012
 *      Author: lion
 */

#ifndef _WIN32

#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include "ev.h"
#include <fcntl.h>
#include <exlib/thread.h>

namespace fibjs
{

void setOption(SOCKET s)
{
    int keepAlive = 1;
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *) &keepAlive,
               sizeof(keepAlive));

#ifdef TCP_KEEPIDLE
    int keepIdle = KEEPALIVE_TIMEOUT;
    setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (void *) &keepIdle, sizeof(keepIdle));
#endif

#ifdef TCP_KEEPINTVL
    int keepInterval = KEEPALIVE_TIMEOUT;
    int keepCount = 3;

    setsockopt(s, SOL_TCP, TCP_KEEPINTVL, (void *) &keepInterval,
               sizeof(keepInterval));
    setsockopt(s, SOL_TCP, TCP_KEEPCNT, (void *) &keepCount, sizeof(keepCount));
#endif

    int noDelay = 1;

    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *) &noDelay, sizeof(noDelay));
}

static struct ev_loop *s_loop;

result_t net_base::backend(std::string &retVal)
{
    switch (ev_backend(s_loop))
    {
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

class asyncProc;

static ev_async s_asEvent;
static exlib::lockfree<asyncProc> s_evWait;

class asyncProc: public ev_io
{
public:
    asyncProc(SOCKET s, int op, exlib::AsyncEvent *ac) :
        m_s(s), m_op(op), m_ac(ac), m_next(NULL)
    {
    }

    virtual ~asyncProc()
    {
    }

    void post()
    {
        s_evWait.put(this);
        ev_async_send(s_loop, &s_asEvent);
    }

    inline void start()
    {
        ev_io *io = (ev_io *) this;
        ev_io_init(io, io_cb, m_s, m_op);
        ev_io_start(s_loop, this);
    }

    result_t call()
    {
        result_t hr = process();
        if (hr == CALL_E_PENDDING)
            post();
        else
            delete this;

        return hr;
    }

    virtual result_t process()
    {
        return 0;
    }

    virtual void proc()
    {
        m_ac->apost(process());
        delete this;
    }

public:
    SOCKET m_s;
    int m_op;
    exlib::AsyncEvent *m_ac;
    asyncProc *m_next;

private:
    static void io_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
    {
        ev_io_stop(s_loop, watcher);
        ((asyncProc *) watcher)->proc();
    }
};

static class _acSocket: public exlib::OSThread
{
public:
    _acSocket()
    {
        s_loop = EV_DEFAULT;
        start();
    }

    virtual void Run()
    {
        ev_async_init(&s_asEvent, as_cb);
        ev_async_start(s_loop, &s_asEvent);

        ev_timer tm;
        tm_cb(s_loop, &tm, 0);

        Runtime rt;
        DateCache dc;
        rt.m_pDateCache = &dc;

        Runtime::reg(&rt);

        ev_run(s_loop, 0);
    }

private:
    static void doAsync()
    {
        asyncProc *p = s_evWait.getList(), *p1;

        while (p)
        {
            p1 = (asyncProc *) p->m_next;
            p->m_next = NULL;
            p->start();

            p = p1;
        }
    }

    static void tm_cb(struct ev_loop *loop, struct ev_timer *watcher,
                      int revents)
    {
        ev_timer_init(watcher, tm_cb, 10, 0);
        ev_timer_start(s_loop, watcher);

        doAsync();
    }

    static void as_cb(struct ev_loop *loop, struct ev_async *watcher,
                      int revents)
    {
        doAsync();
    }
} s_acSock;

class asyncWait: public asyncProc
{
public:
    asyncWait(SOCKET s, int op) :
        asyncProc(s, op, NULL)
    {
        m_ac = &m_aEvent;
        post();
        m_aEvent.wait();
    }

public:
    asyncEvent m_aEvent;
};

result_t Socket::connect(const char *host, int32_t port, exlib::AsyncEvent *ac)
{
    class asyncConnect: public asyncProc
    {
    public:
        asyncConnect(SOCKET s, inetAddr &ai, exlib::AsyncEvent *ac) :
            asyncProc(s, EV_WRITE, ac), m_ai(ai)
        {
        }

        virtual result_t process()
        {
            int n = ::connect(m_s, (struct sockaddr *) &m_ai, m_ai.size());
            if (n == SOCKET_ERROR)
            {
                int nError = errno;
                return (nError == EINPROGRESS) ? CALL_E_PENDDING : -nError;
            }

            return 0;
        }

        virtual void proc()
        {
            inetAddr addr_info;
            socklen_t sz1 = sizeof(addr_info);

            if (::getpeername(m_s, (sockaddr *) &addr_info, &sz1) == SOCKET_ERROR)
                m_ac->apost(-ECONNREFUSED);
            else
            {
                setOption(m_s);
                m_ac->apost(0);
            }

            delete this;
        }

    public:
        inetAddr m_ai;
    };

    if (m_sock == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    inetAddr addr_info;

    addr_info.init(m_family);
    addr_info.setPort(port);
    if (addr_info.addr(host) < 0)
    {
        std::string strAddr;
        result_t hr = net_base::resolve(host, m_family, strAddr, ac);
        if (hr < 0)
            return hr;

        if (addr_info.addr(strAddr.c_str()) < 0)
            return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    return (new asyncConnect(m_sock, addr_info, ac))->call();
}

result_t Socket::accept(obj_ptr<Socket_base> &retVal, exlib::AsyncEvent *ac)
{
    class asyncAccept: public asyncProc
    {
    public:
        asyncAccept(SOCKET s, obj_ptr<Socket_base> &retVal,
                    exlib::AsyncEvent *ac) :
            asyncProc(s, EV_READ, ac), m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            inetAddr ai;
            socklen_t sz = sizeof(ai);
            SOCKET c = ::accept(m_s, (sockaddr *) &ai, &sz);
            if (c == INVALID_SOCKET)
            {
                int nError = errno;
                return (nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError;
            }

            fcntl(c, F_SETFL, fcntl(c, F_GETFL, 0) | O_NONBLOCK);
            fcntl(c, F_SETFD, FD_CLOEXEC);

#ifdef MacOS
            int set_option = 1;
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
        obj_ptr<Socket_base> &m_retVal;
    };

    if (m_sock == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncAccept(m_sock, retVal, ac))->call();
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                      exlib::AsyncEvent *ac, bool bRead)
{
    class asyncRecv: public asyncProc
    {
    public:
        asyncRecv(SOCKET s, int32_t bytes, obj_ptr<Buffer_base> &retVal,
                  exlib::AsyncEvent *ac, bool bRead) :
            asyncProc(s, EV_READ, ac), m_retVal(retVal), m_pos(0), m_bRead(
                bRead)
        {
            m_buf.resize(bytes > 0 ? bytes : STREAM_BUFF_SIZE);
        }

        virtual result_t process()
        {
            do
            {
                int n = (int) ::recv(m_s, &m_buf[m_pos], m_buf.length() - m_pos,
                                     MSG_NOSIGNAL);
                if (n == SOCKET_ERROR)
                {
                    int nError = errno;
                    if (nError == ECONNRESET)
                        n = 0;
                    else
                        return (nError == EWOULDBLOCK) ?
                               CALL_E_PENDDING : -nError;
                }

                if (n == 0)
                    m_bRead = false;

                m_pos += n;
                if (m_pos == 0)
                    return CALL_RETURN_NULL;
            }
            while (m_bRead && m_pos < (int) m_buf.length());

            m_buf.resize(m_pos);
            m_retVal = new Buffer(m_buf);

            return 0;
        }

        virtual void proc()
        {
            result_t hr = process();

            if (hr == CALL_E_PENDDING)
                post();
            else
            {
                m_ac->apost(hr);
                delete this;
            }
        }

    public:
        obj_ptr<Buffer_base> &m_retVal;
        int m_pos;
        bool m_bRead;
        std::string m_buf;
    };

    if (m_sock == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncRecv(m_sock, bytes, retVal, ac, bRead))->call();
}

result_t Socket::send(Buffer_base *data, exlib::AsyncEvent *ac)
{
    class asyncSend: public asyncProc
    {
    public:
        asyncSend(SOCKET s, Buffer_base *data, exlib::AsyncEvent *ac) :
            asyncProc(s, EV_WRITE, ac)
        {
            data->toString(m_buf);
            m_p = m_buf.c_str();
            m_sz = m_buf.length();
        }

        virtual result_t process()
        {
            while (m_sz)
            {
                int n = (int) ::send(m_s, m_p, m_sz, MSG_NOSIGNAL);
                if (n == SOCKET_ERROR)
                {
                    int nError = errno;
                    return (nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError;
                }

                m_sz -= n;
                m_p += n;
            }

            return 0;
        }

        virtual void proc()
        {
            result_t hr = process();

            if (hr == CALL_E_PENDDING)
                post();
            else
            {
                m_ac->apost(hr);
                delete this;
            }
        }

    public:
        std::string m_buf;
        const char *m_p;
        int m_sz;
    };

    if (m_sock == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncSend(m_sock, data, ac))->call();
}

}

#endif
