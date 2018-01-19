/*
 * DgramSocket.h
 *
 *  Created on: Jan 1, 2018
 *      Author: lion
 */

#include "ifs/DgramSocket.h"
#include "AsyncIO.h"

#ifndef DGRAMSOCKET_H_
#define DGRAMSOCKET_H_

namespace fibjs {

class DgramSocket : public DgramSocket_base {
public:
    DgramSocket()
        : m_aio(INVALID_SOCKET, 0, net_base::_SOCK_STREAM)
        , m_closed(false)
        , m_bound(false)
    {
    }

    result_t create(int32_t type, bool reuseAddr);

public:
    EVENT_SUPPORT();

public:
    // DgramSocket_base
    virtual result_t bind(int32_t port, exlib::string addr, AsyncEvent* ac);
    virtual result_t bind(v8::Local<v8::Object> opts, AsyncEvent* ac);
    virtual result_t send(Buffer_base* msg, int32_t port, exlib::string address, int32_t& retVal, AsyncEvent* ac);
    virtual result_t send(Buffer_base* msg, int32_t offset, int32_t length, int32_t port, exlib::string address, int32_t& retVal, AsyncEvent* ac);
    virtual result_t address(obj_ptr<NObject>& retVal);
    virtual result_t close();
    virtual result_t close(v8::Local<v8::Function> callback);
    virtual result_t getRecvBufferSize(int32_t& retVal);
    virtual result_t getSendBufferSize(int32_t& retVal);
    virtual result_t setRecvBufferSize(int32_t size);
    virtual result_t setSendBufferSize(int32_t size);
    virtual result_t setBroadcast(bool flag);
    virtual result_t ref(obj_ptr<DgramSocket_base>& retVal);
    virtual result_t unref(obj_ptr<DgramSocket_base>& retVal);

public:
    AsyncIO m_aio;
    bool m_closed;
    bool m_bound;
};
}

#endif // DGRAMSOCKET_H_