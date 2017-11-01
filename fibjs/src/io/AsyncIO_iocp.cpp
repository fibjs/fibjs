/*
 * AsyncIO_iocp.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "object.h"
#include "utils.h"
#include "Socket.h"
#include "ifs/net.h"
#include "ifs/console.h"
#include "Buffer.h"
#include <fcntl.h>
#include <mswsock.h>
#include <mstcpip.h>
#include <exlib/include/thread.h>
#include "options.h"

namespace fibjs {

exlib::string clean_string(exlib::string s)
{
    exlib::string s1(s);

    char* c_buf = s1.c_buffer();
    int32_t len = (int32_t)s1.length();

    for (int32_t i = 0; i < len; i++)
        if ((c_buf[i] < 32 && c_buf[i] != 0xd && c_buf[i] != 0xa) || c_buf[i] > 127)
            c_buf[i] = '.';

    return s1;
}

void setOption(SOCKET s)
{
    int32_t keepAlive = 1;
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char*)&keepAlive,
        sizeof(keepAlive));

    tcp_keepalive Settings = { 1, KEEPALIVE_TIMEOUT * 1000, 20 * 1000 };
    DWORD dwBytes = 0L;

    WSAIoctl(s, SIO_KEEPALIVE_VALS, &Settings, sizeof(Settings), NULL, 0,
        &dwBytes, NULL, NULL);

    int32_t noDelay = 1;

    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char*)&noDelay,
        sizeof(noDelay));
}

HANDLE s_hIocp;

class asyncProc : public OVERLAPPED,
                  public exlib::Task_base {
public:
    asyncProc(SOCKET s, AsyncEvent* ac, exlib::Locker& locker)
        : m_s(s)
        , m_ac(ac)
        , m_locker(locker)
        , m_next(NULL)
    {
        memset((OVERLAPPED*)this, 0, sizeof(OVERLAPPED));
    }

    virtual ~asyncProc()
    {
    }

public:
    virtual void resume()
    {
        PostQueuedCompletionStatus(s_hIocp, -1, -1, (LPOVERLAPPED)this);
    }

public:
    void post()
    {
        if (m_locker.lock(this))
            PostQueuedCompletionStatus(s_hIocp, -1, -1, (LPOVERLAPPED)this);
    }

    void proc()
    {
        result_t hr = process();
        if (hr != CALL_E_PENDDING)
            asyncProc::ready(0, hr);
    }

    virtual result_t process()
    {
        return 0;
    }

    virtual void ready(DWORD dwBytes, int32_t nError)
    {
        m_locker.unlock(this);
        m_ac->apost(nError);
        delete this;
    }

public:
    SOCKET m_s;
    AsyncEvent* m_ac;
    exlib::Locker& m_locker;
    asyncProc* m_next;
};

class _acIO : public exlib::OSThread {
public:
    _acIO()
    {
        WSADATA wsaData;
        int32_t iResult;

        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            exit(-1);
        }

        s_hIocp = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
    }

    virtual void Run()
    {
        int32_t bRet;
        ULONG_PTR v;
        DWORD dwBytes, dwError;
        LPOVERLAPPED pOverlap;

        Runtime rt(NULL);

        while (true) {
            v = 0;
            pOverlap = NULL;

            bRet = GetQueuedCompletionStatus(s_hIocp, &dwBytes, &v, &pOverlap,
                INFINITE);

            if (!bRet)
                dwError = ::GetLastError();
            else
                dwError = 0;

            if (dwBytes == -1 && v == -1 && bRet)
                ((asyncProc*)pOverlap)->proc();
            else if (bRet || (dwError != WAIT_TIMEOUT))
                ((asyncProc*)pOverlap)->ready(dwBytes, -(int32_t)dwError);
        }
    }
};

void init_aio()
{
    static _acIO s_acIO;
    s_acIO.start();
}

result_t net_base::backend(exlib::string& retVal)
{
    retVal = "IOCP";
    return 0;
}

result_t AsyncIO::connect(exlib::string host, int32_t port, AsyncEvent* ac, Timer_base* timer)
{
    class asyncConnect : public asyncProc {
    public:
        asyncConnect(SOCKET s, inetAddr& ai, AsyncEvent* ac, exlib::Locker& locker, Timer_base* timer)
            : asyncProc(s, ac, locker)
            , m_ai(ai)
            , m_timer(timer)
        {
        }

        virtual result_t process()
        {
            static LPFN_CONNECTEX ConnectEx;
            int32_t nError;

            if (!ConnectEx) {
                GUID guidConnectEx = WSAID_CONNECTEX;
                DWORD dwBytes;

                if (SOCKET_ERROR
                    == WSAIoctl(m_s, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &guidConnectEx, sizeof(guidConnectEx),
                           &ConnectEx, sizeof(ConnectEx), &dwBytes, NULL,
                           NULL)) {
                    if (m_timer) {
                        m_timer->clear();
                        m_timer.Release();
                    }
                    return CHECK_ERROR(SocketError());
                }
            }

            if (ConnectEx(m_s, (sockaddr*)&m_ai, (int32_t)m_ai.size(), NULL, 0,
                    NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = WSAGetLastError();
            if (nError == WSA_IO_PENDING)
                return CHECK_ERROR(CALL_E_PENDDING);

            if (m_timer) {
                m_timer->clear();
                m_timer.Release();
            }
            return CHECK_ERROR(-nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (m_timer) {
                m_timer->clear();
                m_timer.Release();
            }

            if (!nError) {
                setsockopt(m_s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
                setOption(m_s);
            }
            asyncProc::ready(dwBytes, nError);
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
    if (addr_info.addr(host.c_str()) < 0) {
        exlib::string strAddr;
        result_t hr = net_base::cc_resolve(host, m_family, strAddr);
        if (hr < 0) {
            if (timer)
                timer->clear();
            return hr;
        }

        if (addr_info.addr(strAddr.c_str()) < 0) {
            if (timer)
                timer->clear();
            return CHECK_ERROR(CALL_E_INVALIDARG);
        }
    }

    (new asyncConnect(m_fd, addr_info, ac, m_lockRecv, timer))->post();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac)
{
    class asyncAccept : public asyncProc {
    public:
        asyncAccept(SOCKET s, SOCKET sListen, obj_ptr<Socket_base>& retVal,
            AsyncEvent* ac, exlib::Locker& locker)
            : asyncProc(s, ac, locker)
            , m_sListen(sListen)
            , m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            static LPFN_ACCEPTEX AcceptEx;
            int32_t nError;

            if (!AcceptEx) {
                GUID guidAcceptEx = WSAID_ACCEPTEX;
                DWORD dwBytes;

                if (SOCKET_ERROR
                    == WSAIoctl(m_s, SIO_GET_EXTENSION_FUNCTION_POINTER,
                           &guidAcceptEx, sizeof(guidAcceptEx), &AcceptEx,
                           sizeof(AcceptEx), &dwBytes, NULL, NULL))
                    return CHECK_ERROR(SocketError());
            }

            if (AcceptEx(m_sListen, m_s, &m_Buf, 0, sizeof(inetAddr) + 16,
                    sizeof(inetAddr) + 16, NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = GetLastError();
            return CHECK_ERROR((nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (!nError) {
                setsockopt(m_s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                    (char*)&m_sListen, sizeof(m_sListen));
                setOption(m_s);
            }
            asyncProc::ready(dwBytes, nError);
        }

    public:
        SOCKET m_sListen;
        obj_ptr<Socket_base>& m_retVal;
        char m_Buf[(sizeof(inetAddr) + 16) * 2];
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Socket> s = new Socket();
    result_t hr = s->create(m_family, m_type);
    if (hr < 0)
        return hr;

    retVal = s;

    asyncAccept* pa = new asyncAccept(s->m_aio.m_fd, m_fd, retVal, ac, m_lockRecv);
    s.Release();

    pa->post();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac, bool bRead, Timer_base* timer)
{
    class asyncRecv : public asyncProc {
    public:
        asyncRecv(SOCKET s, int32_t bytes, obj_ptr<Buffer_base>& retVal,
            AsyncEvent* ac, bool bRead, exlib::Locker& locker, Timer_base* timer)
            : asyncProc(s, ac, locker)
            , m_retVal(retVal)
            , m_pos(0)
            , m_bRead(bRead)
            , m_timer(timer)
        {
            m_buf.resize(bytes > 0 ? bytes : SOCKET_BUFF_SIZE);
        }

        virtual result_t process()
        {
            int32_t nError;

            if (ReadFile((HANDLE)m_s, &m_buf[m_pos],
                    (DWORD)m_buf.length() - m_pos, NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = GetLastError();

            if (nError == ERROR_BROKEN_PIPE) {
                if (m_timer) {
                    m_timer->clear();
                    m_timer.Release();
                }

                return CALL_RETURN_NULL;
            }

            if (nError == ERROR_IO_PENDING)
                return CHECK_ERROR(CALL_E_PENDDING);

            if (m_timer) {
                m_timer->clear();
                m_timer.Release();
            }
            return CHECK_ERROR(-nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (m_timer) {
                m_timer->clear();
                m_timer.Release();
            }

            if (nError == -ERROR_BROKEN_PIPE) {
                nError = 0;
                dwBytes = 0;
            }

            if (dwBytes == 0)
                m_bRead = false;

            if (!nError) {
                m_pos += dwBytes;

                if (m_bRead && m_pos < (int32_t)m_buf.length()) {
                    proc();
                    return;
                }

                if (m_pos) {
                    m_buf.resize(m_pos);
                    m_retVal = new Buffer(m_buf);

                    if (g_tracetcp)
                        outLog(console_base::_NOTICE, clean_string(m_buf));
                } else
                    nError = CALL_RETURN_NULL;
            }

            asyncProc::ready(dwBytes, nError);
        }

    public:
        obj_ptr<Buffer_base>& m_retVal;
        int32_t m_pos;
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

    (new asyncRecv(m_fd, bytes, retVal, ac, bRead, m_lockRecv, timer))->post();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::write(Buffer_base* data, AsyncEvent* ac)
{
    class asyncSend : public asyncProc {
    public:
        asyncSend(SOCKET s, Buffer_base* data, AsyncEvent* ac, exlib::Locker& locker)
            : asyncProc(s, ac, locker)
        {
            data->toString(m_buf);
            m_p = m_buf.c_str();
            m_sz = (int32_t)m_buf.length();

            if (g_tracetcp)
                outLog(console_base::_WARN, clean_string(m_buf));
        }

        virtual result_t process()
        {
            int32_t nError;

            if (WriteFile((HANDLE)m_s, (LPCSTR)m_p, m_sz, NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = GetLastError();
            return CHECK_ERROR((nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (!nError) {
                m_p += dwBytes;
                m_sz -= dwBytes;

                if (m_sz > 0) {
                    proc();
                    return;
                }
            }

            asyncProc::ready(dwBytes, nError);
        }

    public:
        exlib::string m_buf;
        const char* m_p;
        int32_t m_sz;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    (new asyncSend(m_fd, data, ac, m_lockSend))->post();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::recvfrom(int32_t bytes, obj_ptr<NObject>& retVal,
    AsyncEvent* ac)
{
    class asyncRecvFrom : public asyncProc {
    public:
        asyncRecvFrom(SOCKET s, int32_t bytes, obj_ptr<NObject>& retVal,
            AsyncEvent* ac, exlib::Locker& locker)
            : asyncProc(s, ac, locker)
            , m_retVal(retVal)
        {
            m_buf.resize(bytes > 0 ? bytes : SOCKET_BUFF_SIZE);
        }

        virtual result_t process()
        {
            int32_t nError;

            m_DataBuf.len = (DWORD)m_buf.length();
            m_DataBuf.buf = &m_buf[0];
            m_dwFlags = 0;
            sz = sizeof(addr_info);

            if (WSARecvFrom(m_s, &m_DataBuf, 1, NULL, &m_dwFlags,
                    (sockaddr*)&addr_info, &sz, this, NULL)
                != SOCKET_ERROR)
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = GetLastError();

            if (nError == ERROR_BROKEN_PIPE)
                return CALL_RETURN_NULL;

            if (nError == ERROR_IO_PENDING)
                return CHECK_ERROR(CALL_E_PENDDING);

            return CHECK_ERROR(-nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (nError == -ERROR_BROKEN_PIPE) {
                nError = 0;
                dwBytes = 0;
            }

            if (!nError) {
                m_buf.resize(dwBytes);
                m_retVal = new DatagramPacket(m_buf, addr_info);
            }

            asyncProc::ready(dwBytes, nError);
        }

    public:
        obj_ptr<NObject>& m_retVal;
        exlib::string m_buf;
        inetAddr addr_info;
        socklen_t sz;
        WSABUF m_DataBuf;
        DWORD m_dwFlags;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    (new asyncRecvFrom(m_fd, bytes, retVal, ac, m_lockRecv))->post();
    return CHECK_ERROR(CALL_E_PENDDING);
}
}

#endif
