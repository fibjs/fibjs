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

#ifndef ASYNCIO_H_
#define ASYNCIO_H_

namespace fibjs
{

#define KEEPALIVE_TIMEOUT   120
#define SOCKET_BUFF_SIZE    2048

class AsyncIO
{
public:
    AsyncIO(intptr_t s, int32_t family, int32_t type) :
        m_fd(s), m_family(family), m_type(type),
        m_inRecv(0), m_inSend(0)
#ifndef _WIN32
        , m_RecvOpt(NULL), m_SendOpt(NULL)
#endif
    {}

public:
    result_t connect(exlib::string host, int32_t port, AsyncEvent *ac);
    result_t accept(obj_ptr<Socket_base> &retVal, AsyncEvent *ac);
    result_t write(Buffer_base *data, AsyncEvent *ac);
    result_t read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                  AsyncEvent *ac, bool bRead);
#ifndef _WIN32
    result_t cancel(AsyncEvent *ac);
#endif

public:
    intptr_t m_fd;
    int32_t m_family;
    int32_t m_type;

private:
    intptr_t m_inRecv;
    intptr_t m_inSend;

#ifndef _WIN32
    void *m_RecvOpt;
    void *m_SendOpt;
#endif
};

}

#endif /* ASYNCIO_H_ */
