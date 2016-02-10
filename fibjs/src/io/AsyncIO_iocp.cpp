/*
 * AsyncIO_iocp.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifdef _WIN32

#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include <fcntl.h>
#include <mswsock.h>
#include <mstcpip.h>
#include <exlib/include/thread.h>

namespace fibjs
{

void setOption(SOCKET s)
{
    int32_t keepAlive = 1;
    setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (const char *) &keepAlive,
               sizeof(keepAlive));

    tcp_keepalive Settings =
    { 1, KEEPALIVE_TIMEOUT * 1000, 20 * 1000 };
    DWORD dwBytes = 0L;

    WSAIoctl(s, SIO_KEEPALIVE_VALS, &Settings, sizeof(Settings), NULL, 0,
             &dwBytes, NULL, NULL);

    int32_t noDelay = 1;

    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (const char *) &noDelay,
               sizeof(noDelay));
}

HANDLE s_hIocp;

class asyncProc: public OVERLAPPED
{
public:
    asyncProc(SOCKET s, AsyncEvent *ac, intptr_t &guard) :
        m_s(s), m_ac(ac), m_guard(guard), m_next(NULL)
    {
        memset((OVERLAPPED *) this, 0, sizeof(OVERLAPPED));
    }

    virtual ~asyncProc()
    {
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
        m_guard = 0;
        m_ac->apost(nError);
        delete this;
    }

public:
    SOCKET m_s;
    AsyncEvent *m_ac;
    intptr_t &m_guard;
    asyncProc *m_next;
};

static class _acIO: public exlib::OSThread
{
public:
    _acIO()
    {
        WSADATA wsaData;
        int32_t iResult;

        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0)
        {
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

        while (true)
        {
            v = 0;
            pOverlap = NULL;

            bRet = GetQueuedCompletionStatus(s_hIocp, &dwBytes, &v, &pOverlap,
                                             INFINITE);
            if (!bRet)
                dwError = ::GetLastError();
            else
                dwError = 0;

            if (bRet || (dwError != WAIT_TIMEOUT))
                ((asyncProc *) pOverlap)->ready(dwBytes, -(int32_t) dwError);
        }
    }

} s_acIO;

void init_aio()
{
    s_acIO.start();
}

result_t net_base::backend(std::string &retVal)
{
    retVal = "IOCP";
    return 0;
}

result_t AsyncIO::connect(const char *host, int32_t port, AsyncEvent *ac)
{
    class asyncConnect: public asyncProc
    {
    public:
        asyncConnect(SOCKET s, inetAddr &ai, AsyncEvent *ac, intptr_t &guard) :
            asyncProc(s, ac, guard), m_ai(ai)
        {
        }

        virtual result_t process()
        {
            static LPFN_CONNECTEX ConnectEx;
            int32_t nError;

            if (!ConnectEx)
            {
                GUID guidConnectEx = WSAID_CONNECTEX;
                DWORD dwBytes;

                if (SOCKET_ERROR
                        == WSAIoctl(m_s, SIO_GET_EXTENSION_FUNCTION_POINTER,
                                    &guidConnectEx, sizeof(guidConnectEx),
                                    &ConnectEx, sizeof(ConnectEx), &dwBytes, NULL,
                                    NULL))
                    return CHECK_ERROR(SocketError());
            }

            if (ConnectEx(m_s, (sockaddr *) &m_ai, (int32_t) m_ai.size(), NULL, 0,
                          NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = WSAGetLastError();
            return CHECK_ERROR((nError == WSA_IO_PENDING) ? CALL_E_PENDDING : -nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (!nError)
            {
                setsockopt(m_s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0);
                setOption (m_s);
            }
            asyncProc::ready(dwBytes, nError);
        }

    public:
        inetAddr m_ai;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    inetAddr addr_info;

    addr_info.init(m_family);
    addr_info.setPort(port);
    if (addr_info.addr(host) < 0)
    {
        std::string strAddr;
        result_t hr = net_base::cc_resolve(host, m_family, strAddr);
        if (hr < 0)
            return hr;

        if (addr_info.addr(strAddr.c_str()) < 0)
            return CHECK_ERROR(CALL_E_INVALIDARG);
    }

    if (exlib::CompareAndSwap(&m_inRecv, 0, 1))
        return CHECK_ERROR(CALL_E_REENTRANT);

    (new asyncConnect(m_fd, addr_info, ac, m_inRecv))->proc();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::accept(obj_ptr<Socket_base> &retVal, AsyncEvent *ac)
{
    class asyncAccept: public asyncProc
    {
    public:
        asyncAccept(SOCKET s, SOCKET sListen, obj_ptr<Socket_base> &retVal,
                    AsyncEvent *ac, intptr_t &guard) :
            asyncProc(s, ac, guard), m_sListen(sListen), m_retVal(retVal)
        {
        }

        virtual result_t process()
        {
            static LPFN_ACCEPTEX AcceptEx;
            int32_t nError;

            if (!AcceptEx)
            {
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
            if (!nError)
            {
                setsockopt(m_s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                           (char *) &m_sListen, sizeof(m_sListen));
                setOption (m_s);
            }
            asyncProc::ready(dwBytes, nError);
        }

    public:
        SOCKET m_sListen;
        obj_ptr<Socket_base> &m_retVal;
        char m_Buf[(sizeof(inetAddr) + 16) * 2];
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (exlib::CompareAndSwap(&m_inRecv, 0, 1))
        return CHECK_ERROR(CALL_E_REENTRANT);

    obj_ptr<Socket> s = new Socket();
    result_t hr = s->create(m_family, m_type);
    if (hr < 0)
    {
        m_inRecv = 0;
        return hr;
    }

    retVal = s;

    asyncAccept *pa = new asyncAccept(s->m_aio.m_fd, m_fd, retVal, ac, m_inRecv);
    s.Release();

    pa->proc();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                       AsyncEvent *ac, bool bRead)
{
    class asyncRecv: public asyncProc
    {
    public:
        asyncRecv(SOCKET s, int32_t bytes, obj_ptr<Buffer_base> &retVal,
                  AsyncEvent *ac, bool bRead, intptr_t &guard) :
            asyncProc(s, ac, guard), m_retVal(retVal), m_pos(0), m_bRead(bRead)
        {
            m_buf.resize(bytes > 0 ? bytes : SOCKET_BUFF_SIZE);
        }

        virtual result_t process()
        {
            int32_t nError;

            if (ReadFile((HANDLE) m_s, &m_buf[m_pos],
                         (DWORD) m_buf.length() - m_pos, NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = GetLastError();

            if (nError == ERROR_NETNAME_DELETED)
                return CALL_RETURN_NULL;

            return CHECK_ERROR((nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (nError == -ERROR_NETNAME_DELETED)
            {
                nError = 0;
                dwBytes = 0;
            }

            if (dwBytes == 0)
                m_bRead = false;

            if (!nError)
            {
                m_pos += dwBytes;

                if (m_bRead && m_pos < (int32_t) m_buf.length())
                {
                    proc();
                    return;
                }

                if (m_pos)
                {
                    m_buf.resize(m_pos);
                    m_retVal = new Buffer(m_buf);
                }
                else
                    nError = CALL_RETURN_NULL;
            }

            asyncProc::ready(dwBytes, nError);
        }

    public:
        obj_ptr<Buffer_base> &m_retVal;
        int32_t m_pos;
        bool m_bRead;
        std::string m_buf;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (exlib::CompareAndSwap(&m_inRecv, 0, 1))
        return CHECK_ERROR(CALL_E_REENTRANT);

    (new asyncRecv(m_fd, bytes, retVal, ac, bRead, m_inRecv))->proc();
    return CHECK_ERROR(CALL_E_PENDDING);
}

result_t AsyncIO::write(Buffer_base *data, AsyncEvent *ac)
{
    class asyncSend: public asyncProc
    {
    public:
        asyncSend(SOCKET s, Buffer_base *data, AsyncEvent *ac, intptr_t &guard) :
            asyncProc(s, ac, guard)
        {
            data->toString(m_buf);
            m_p = m_buf.c_str();
            m_sz = (int32_t) m_buf.length();
        }

        virtual result_t process()
        {
            int32_t nError;

            if (WriteFile((HANDLE) m_s, (LPCSTR) m_p, m_sz, NULL, this))
                return CHECK_ERROR(CALL_E_PENDDING);

            nError = GetLastError();
            return CHECK_ERROR((nError == ERROR_IO_PENDING) ? CALL_E_PENDDING : -nError);
        }

        virtual void ready(DWORD dwBytes, int32_t nError)
        {
            if (!nError)
            {
                m_p += dwBytes;
                m_sz -= dwBytes;

                if (m_sz > 0)
                {
                    proc();
                    return;
                }
            }

            asyncProc::ready(dwBytes, nError);
        }

    public:
        std::string m_buf;
        const char *m_p;
        int32_t m_sz;
    };

    if (m_fd == INVALID_SOCKET)
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (exlib::CompareAndSwap(&m_inSend, 0, 1))
        return CHECK_ERROR(CALL_E_REENTRANT);

    (new asyncSend(m_fd, data, ac, m_inSend))->proc();
    return CHECK_ERROR(CALL_E_PENDDING);
}

}

#endif
