/*
 * ZmqSocket.h
 *
 *  Created on: May 29, 2012
 *      Author: lion
 */

#include "ifs/ZmqSocket.h"
#include <zmq/include/zmq.h>

#ifndef ZMQSOCKET_H_
#define ZMQSOCKET_H_

namespace fibjs {

class ZmqSocket : public ZmqSocket_base {
    EVENT_SUPPORT();

public:
    ZmqSocket(int32_t type);
    virtual ~ZmqSocket();

public:
    // ZmqSocket_base
    virtual result_t bind(exlib::string addr);
    virtual result_t connect(exlib::string addr);
    virtual result_t recv(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t send(Buffer_base* data);
    virtual result_t close();
    virtual result_t get_type(int32_t& retVal);

private:
    void* m_sock;
    int32_t m_type;
};

} /* namespace fibjs */
#endif /* ZMQSOCKET_H_ */
