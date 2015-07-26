/*
 * Socket.h
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "ifs/Socket.h"
#include "Stream.h"
#include "inetAddr.h"

#ifndef SOCKET_H_
#define SOCKET_H_

namespace fibjs
{

#define KEEPALIVE_TIMEOUT   120

class Socket: public Socket_base
{
    FIBER_FREE();

public:
    Socket() :
        m_sock(INVALID_SOCKET),
        m_family(net_base::_AF_INET), m_type(net_base::_SOCK_STREAM),
        m_inRecv(0), m_inSend(0)
#ifdef _WIN32
        , m_bBind(FALSE)
#else
        , m_RecvOpt(NULL), m_SendOpt(NULL)
#endif
    {
    }

    Socket(SOCKET s, int32_t family, int32_t type) :
        m_sock(s), m_family(family), m_type(type),
        m_inRecv(0), m_inSend(0)
#ifdef _WIN32
        , m_bBind(FALSE)
#else
        , m_RecvOpt(NULL), m_SendOpt(NULL)
#endif
    {
    }

    virtual ~Socket();

public:
    // Stream_base
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                          AsyncEvent *ac);
    virtual result_t write(Buffer_base *data, AsyncEvent *ac);
    virtual result_t close(AsyncEvent *ac);
    virtual result_t copyTo(Stream_base *stm, int64_t bytes,
                            int64_t &retVal, AsyncEvent *ac);

public:
    // Socket_base
    virtual result_t get_family(int32_t &retVal);
    virtual result_t get_type(int32_t &retVal);
    virtual result_t get_remoteAddress(std::string &retVal);
    virtual result_t get_remotePort(int32_t &retVal);
    virtual result_t get_localAddress(std::string &retVal);
    virtual result_t get_localPort(int32_t &retVal);
    virtual result_t connect(const char *host, int32_t port,
                             AsyncEvent *ac);
    virtual result_t bind(const char *addr, int32_t port, bool allowIPv4);
    virtual result_t bind(int32_t port, bool allowIPv4);
    virtual result_t listen(int32_t backlog, AsyncEvent *ac);
    virtual result_t accept(obj_ptr<Socket_base> &retVal,
                            AsyncEvent *ac);
    virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                          AsyncEvent *ac);
    virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base> &retVal);
    virtual result_t send(Buffer_base *data, AsyncEvent *ac);
    virtual result_t sendto(Buffer_base *data, const char *host,
                            int32_t port);

public:
    result_t create(int32_t family, int32_t type);
    result_t recv(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                  AsyncEvent *ac, bool bRead);

private:
    SOCKET m_sock;
    int32_t m_family;
    int32_t m_type;

    exlib::atomic_t m_inRecv;
    exlib::atomic_t m_inSend;

#ifdef _WIN32
    BOOL m_bBind;
#else
    void *m_RecvOpt;
    void *m_SendOpt;

    void cancel_socket(AsyncEvent *ac);
#endif
};

}

#endif /* SOCKET_H_ */
