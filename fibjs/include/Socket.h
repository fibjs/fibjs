/*
 * Socket.h
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "ifs/Socket.h"

#ifndef SOCKET_H_
#define SOCKET_H_

namespace fibjs
{

union _sockaddr
{
	struct sockaddr_in addr4;
	struct sockaddr_in6 addr6;
};

class Socket: public Socket_base
{
	EVENT_SUPPORT();

public:
	Socket() :
			m_sock(INVALID_SOCKET), m_family(_AF_INET), m_type(_SOCK_STREAM)
	{
	}

	Socket(SOCKET s, int32_t family, int32_t type) :
			m_sock(s), m_family(family), m_type(type)
	{
	}

	virtual ~Socket();

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
			AsyncCall* ac);
	virtual result_t write(obj_ptr<Buffer_base> data, AsyncCall* ac);
	virtual result_t flush(AsyncCall* ac);
	virtual result_t close(AsyncCall* ac);

public:
	// Socket_base
	virtual result_t get_family(int32_t& retVal);
	virtual result_t get_type(int32_t& retVal);
	virtual result_t get_remoteAddress(std::string& retVal);
	virtual result_t get_remotePort(int32_t& retVal);
	virtual result_t get_localAddress(std::string& retVal);
	virtual result_t get_localPort(int32_t& retVal);
	virtual result_t connect(const char* addr, int32_t port, AsyncCall* ac);
	virtual result_t bind(const char* addr, int32_t port, bool allowIPv4);
	virtual result_t bind(int32_t port, bool allowIPv4);
	virtual result_t listen(int32_t backlog);
	virtual result_t accept(obj_ptr<Socket_base>& retVal, AsyncCall* ac);
	virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t recv(v8::Handle<v8::Function> cb,
			obj_ptr<Buffer_base>& retVal);
	virtual result_t recv(int32_t bytes, v8::Handle<v8::Function> cb,
			obj_ptr<Buffer_base>& retVal);
	virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t send(obj_ptr<Buffer_base> data);
	virtual result_t send(obj_ptr<Buffer_base> data,
			v8::Handle<v8::Function> cb);
	virtual result_t sendto(obj_ptr<Buffer_base> data, const char* host,
			int32_t port);

public:
	result_t create(int32_t family, int32_t type);
	result_t send(const char* p, int sz);

private:
	result_t getAddrInfo(const char* addr, int32_t port, _sockaddr& addr_info);

private:
	SOCKET m_sock;
	int32_t m_family;
	int32_t m_type;
};

int a_connect(SOCKET s, sockaddr* ai, int sz);
SOCKET a_accept(SOCKET s, sockaddr* ai, socklen_t* sz);
int a_recv(SOCKET s, char *p, size_t sz, int f);
int a_send(SOCKET s, const char *p, size_t sz, int f);

}

#endif /* SOCKET_H_ */
