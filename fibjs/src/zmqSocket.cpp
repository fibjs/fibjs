/*
 * zmqSocket.cpp
 *
 *  Created on: May 29, 2012
 *      Author: lion
 */

#include "zmqSocket.h"
#include "Buffer.h"

namespace fibjs
{

static void *s_ctx;

result_t zmq_Socket_base::_new(int32_t type, obj_ptr<zmq_Socket_base>& retVal)
{
	if (!s_ctx)
		s_ctx = zmq_init(0);

	retVal = new zmqSocket(type);

	return 0;
}

zmqSocket::zmqSocket(int32_t type)
{
	m_type = type;

	m_sock = zmq_socket(s_ctx, type);
}

zmqSocket::~zmqSocket()
{
	// TODO Auto-generated destructor stub
}

result_t zmqSocket::bind(const char* addr)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	int rc = zmq_bind(m_sock, addr);
	if (rc)
		return LastError();

	return 0;
}

result_t zmqSocket::connect(const char* addr)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	int rc = zmq_connect(m_sock, addr);
	if (rc)
		return LastError();

	return 0;
}

result_t zmqSocket::recv(obj_ptr<Buffer_base>& retVal)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	std::string strBuf;

	strBuf.resize(4096);

	int nbytes = zmq_recv(m_sock, &strBuf[0], strBuf.length(), 0);
	if (nbytes == -1)
		return LastError();

	strBuf.resize(nbytes);
	retVal = new Buffer(strBuf);

	return 0;
}

result_t zmqSocket::send(obj_ptr<Buffer_base>& data)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	std::string strBuf;
	data->toString(strBuf);

	int rc = zmq_send(m_sock, &strBuf[0], strBuf.length(), 0);
	if (rc)
		return LastError();

	return 0;
}

result_t zmqSocket::close()
{
	if (m_sock)
		zmq_close(m_sock);

	m_sock = NULL;

	return 0;
}

result_t zmqSocket::get_type(int32_t& retVal)
{
	if (!m_sock)
		return CALL_E_INVALID_CALL;

	retVal = m_type;

	return 0;
}

} /* namespace fibjs */
