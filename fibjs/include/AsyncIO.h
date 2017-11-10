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

    class DatagramPacket : public NObject {
    public:
        DatagramPacket(exlib::string data, inetAddr& addr)
        {
            add("data", new Buffer(data));
            add("address", addr.str());
            add("port", addr.port());
        }
    };

public:
    result_t connect(exlib::string host, int32_t port, AsyncEvent* ac, Timer_base* timer);
    result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac);
    result_t write(Buffer_base* data, AsyncEvent* ac);
    result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
        AsyncEvent* ac, bool bRead, Timer_base* timer);
    result_t recvfrom(int32_t bytes, obj_ptr<NObject>& retVal, AsyncEvent* ac);

#ifndef _WIN32
    result_t close(intptr_t& s, AsyncEvent* ac);
#else
    result_t close(intptr_t& s, AsyncEvent* ac)
    {
        if (s != INVALID_SOCKET) {
            if (m_type == -1)
                ::CloseHandle((HANDLE)s);
            else
                ::closesocket(s);
        }

        s = INVALID_SOCKET;

        return 0;
    }
#endif

    static result_t waitpid(intptr_t pid, int32_t& retVal, AsyncEvent* ac);

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
