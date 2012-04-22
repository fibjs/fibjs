/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "ifs/net.h"

namespace fibjs
{

#ifdef _WIN32
static class _initWinSock
{
public:
	_initWinSock()
	{
		WORD wVersionRequested;
		WSADATA wsaData;

		wVersionRequested = MAKEWORD(2, 2);
		WSAStartup(wVersionRequested, &wsaData);
	}
}s_initWinSock;

#endif

Socket::Socket()
{
	m_sock = SOCKET_ERROR;
}

Socket::~Socket()
{
	close();
}

result_t Socket::create(int32_t family, int32_t type)
{
	close();

	if (family == net_base::_AF_INET)
		family = AF_INET;
	else if (family == net_base::_AF_INET6)
		family = AF_INET6;
	else
		return CALL_E_INVALIDARG;

	if (type == net_base::_SOCK_STREAM)
		type = SOCK_STREAM;
	else if (type == net_base::_SOCK_DGRAM)
		type = SOCK_DGRAM;
	else
		return CALL_E_INVALIDARG;

	m_sock = socket(family, type, 0);
	if(m_sock == SOCKET_ERROR)
		return LastError();

	return 0;
}

result_t Socket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal)
{
	return 0;
}

result_t Socket::write(obj_ptr<Buffer_base> data)
{
	return 0;
}

result_t Socket::flush()
{
	return 0;
}

result_t Socket::close()
{
	if (m_sock != SOCKET_ERROR)
		::closesocket(m_sock);

	m_sock = SOCKET_ERROR;

	return 0;
}

result_t Socket::get_family(int32_t& retVal)
{
	return 0;
}

result_t Socket::get_type(int32_t& retVal)
{
	return 0;
}

result_t Socket::connect(const char* host, int32_t port)
{
	return 0;
}

result_t Socket::bind(const char* addr, int32_t port)
{
	return 0;
}

result_t Socket::bind(int32_t port)
{
	return 0;
}

result_t Socket::listen()
{
	return 0;
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal)
{
	return 0;
}

result_t Socket::recv(int32_t size, obj_ptr<Buffer_base>& retVal)
{
	return 0;
}

result_t Socket::recvFrom(int32_t size, obj_ptr<Buffer_base>& retVal)
{
	return 0;
}

result_t Socket::send(obj_ptr<Buffer_base> data)
{
	return 0;
}

result_t Socket::sendto(obj_ptr<Buffer_base> data, const char* host,
		int32_t port)
{
	return 0;
}

}
