/*
 * Socket_ev.cpp
 *
 *  Created on: Apr 26, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "Buffer.h"

namespace fibjs
{

static AsyncQueue s_acSocket;

result_t Socket::sync_recv(int32_t bytes, obj_ptr<Buffer_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	std::string buf;
	int sz = bytes > 0 ? bytes : 4096;

	buf.resize(sz);
	char* p = &buf[0];

	do
	{
		int n = (int) ::recv(m_sock, p, sz, MSG_NOSIGNAL);
		if (n == SOCKET_ERROR)
			return SocketError();

		if (n == 0)
			break;

		sz -= n;
		p += n;
	} while (bytes > sz);

	buf.resize((bytes > 0 ? bytes : 4096) - sz);
	retVal = new Buffer(buf);

	return 0;
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal)
{
	return ac_sync_recv(s_acPool, bytes, retVal);
}

result_t Socket::send(const char* p, int sz)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	while (sz)
	{
		int n = (int) ::send(m_sock, p, sz, MSG_NOSIGNAL);
		if (n == SOCKET_ERROR)
			return SocketError();

		sz -= n;
		p += n;
	}

	return 0;
}

result_t Socket::sync_send(obj_ptr<Buffer_base> data)
{
	std::string strBuf;
	data->toString(strBuf);

	return send(strBuf.c_str(), (int) strBuf.length());
}

result_t Socket::send(obj_ptr<Buffer_base> data)
{
	return ac_sync_send(s_acPool, data);
}

}
