/*
 * AsyncIO.h
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#include "AsyncCall.h"
#include "Buffer.h"
#include "ifs/net.h"
#include "ifs/Socket.h"
#include "Timer.h"
#include "inetAddr.h"

#ifndef ASYNCIO_H_
#define ASYNCIO_H_

namespace fibjs {

#define KEEPALIVE_TIMEOUT 120
#define SOCKET_BUFF_SIZE 2048

class AsyncIO {
public:
    AsyncIO(intptr_t s, int32_t family, int32_t type)
        : m_fd(s)
        , m_family(family)
        , m_type(type)
#ifndef _WIN32
        , m_RecvOpt(NULL)
        , m_SendOpt(NULL)
#endif
    {
    }

    class DatagramPacket : public DatagramPacket_base {
    public:
        DatagramPacket(exlib::string data, inetAddr& addr)
        {
            m_data = new Buffer(data);
            m_addr = addr.str();
            m_port = addr.port();
        }

    public:
        // DatagramPacket_base
        virtual result_t get_data(obj_ptr<Buffer_base>& retVal)
        {
            retVal = m_data;
            return 0;
        }

        virtual result_t get_address(exlib::string& retVal)
        {
            retVal = m_addr;
            return 0;
        }

        virtual result_t get_port(int32_t& retVal)
        {
            retVal = m_port;
            return 0;
        }

    private:
        obj_ptr<Buffer_base> m_data;
        exlib::string m_addr;
        int32_t m_port;
    };

public:
    result_t connect(exlib::string host, int32_t port, AsyncEvent* ac, Timer_base* timer);
    result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac);
    result_t write(Buffer_base* data, AsyncEvent* ac);
    result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac, bool bRead, Timer_base* timer);
    result_t recvfrom(int32_t bytes, obj_ptr<DatagramPacket_base>& retVal, AsyncEvent* ac);

#ifndef _WIN32
    result_t cancel(AsyncEvent* ac);
#endif

public:
    intptr_t m_fd;
    int32_t m_family;
    int32_t m_type;

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

#endif /* ASYNCIO_H_ */
