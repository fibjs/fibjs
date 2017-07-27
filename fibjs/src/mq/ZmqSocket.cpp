/*
 * ZmqSocket.cpp
 *
 *  Created on: May 29, 2012
 *      Author: lion
 */

#include "ZmqSocket.h"
#include "Buffer.h"

namespace fibjs {

DECLARE_MODULE(zmq);

static void* s_ctx;

result_t ZmqSocket_base::_new(int32_t type, obj_ptr<ZmqSocket_base>& retVal,
    v8::Local<v8::Object> This)
{
    if (!s_ctx)
        s_ctx = zmq_init(1);

    retVal = new ZmqSocket(type);

    return 0;
}

ZmqSocket::ZmqSocket(int32_t type)
{
    m_type = type;
    m_sock = zmq_socket(s_ctx, type);
}

ZmqSocket::~ZmqSocket()
{
    close();
}

inline result_t zmq_LastError()
{
    int rc = zmq_errno();

    if (rc > ZMQ_HAUSNUMERO)
        return CHECK_ERROR(Runtime::setError(zmq_strerror(rc)));
    return CHECK_ERROR(LastError());
}

result_t ZmqSocket::bind(exlib::string addr)
{
    if (!m_sock)
        return CALL_E_INVALID_CALL;

    int rc = zmq_bind(m_sock, addr.c_str());
    if (rc)
        return zmq_LastError();

    return 0;
}

result_t ZmqSocket::connect(exlib::string addr)
{
    if (!m_sock)
        return CALL_E_INVALID_CALL;

    int rc = zmq_connect(m_sock, addr.c_str());
    if (rc)
        return zmq_LastError();

    return 0;
}

result_t ZmqSocket::recv(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (!m_sock)
        return CALL_E_INVALID_CALL;

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_LONGSYNC);

    zmq_msg_t msg;
    int rc = zmq_msg_init(&msg);
    if (rc < 0)
        return zmq_LastError();

    int nbytes = zmq_recvmsg(m_sock, &msg, 0);
    if (nbytes < 0) {
        result_t hr = zmq_LastError();
        zmq_msg_close(&msg);
        return hr;
    }

    exlib::string strBuf;

    strBuf.resize(nbytes);
    memcpy(&strBuf[0], zmq_msg_data(&msg), nbytes);
    retVal = new Buffer(strBuf);

    rc = zmq_msg_close(&msg);
    if (rc < 0)
        return zmq_LastError();

    return 0;
}

result_t ZmqSocket::send(Buffer_base* data)
{
    if (!m_sock)
        return CALL_E_INVALID_CALL;

    exlib::string strBuf;
    data->toString(strBuf);

    int nbytes = zmq_send(m_sock, &strBuf[0], strBuf.length(), 0);
    if (nbytes < 0)
        return zmq_LastError();

    return 0;
}

result_t ZmqSocket::close()
{
    if (m_sock)
        zmq_close(m_sock);

    m_sock = NULL;

    return 0;
}

result_t ZmqSocket::get_type(int32_t& retVal)
{
    if (!m_sock)
        return CALL_E_INVALID_CALL;

    retVal = m_type;

    return 0;
}

} /* namespace fibjs */
