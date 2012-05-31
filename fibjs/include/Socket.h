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

	size_t size()
	{
		return addr6.sin6_family == PF_INET6 ?
					sizeof(addr6) :
					sizeof(addr4);
	}
};

class Socket: public Socket_base
{
	EVENT_SUPPORT();

public:
	Socket() :
		m_sock(INVALID_SOCKET), m_family(net_base::_AF_INET), m_type(net_base::_SOCK_STREAM)
#ifdef _WIN32
		, m_bBind(FALSE)
#endif
	{
	}

	Socket(SOCKET s, int32_t family, int32_t type) :
		m_sock(s), m_family(family), m_type(type)
#ifdef _WIN32
		, m_bBind(FALSE)
#endif
	{
	}

	virtual ~Socket();

public:
	// Stream_base
	virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncRead(int32_t bytes);
	virtual result_t onread(v8::Handle<v8::Function> func);
	virtual result_t write(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac);
	virtual result_t asyncWrite(obj_ptr<Buffer_base>& data);
	virtual result_t onwrite(v8::Handle<v8::Function> func);
	virtual result_t stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncStat();
	virtual result_t onstat(v8::Handle<v8::Function> func);
	virtual result_t size(double& retVal);
	virtual result_t eof(bool& retVal);
	virtual result_t flush(exlib::AsyncEvent* ac);
	virtual result_t asyncFlush();
	virtual result_t onflush(v8::Handle<v8::Function> func);
	virtual result_t close(exlib::AsyncEvent* ac);
	virtual result_t asyncClose();
	virtual result_t onclose(v8::Handle<v8::Function> func);
	virtual result_t onerror(v8::Handle<v8::Function> func);

public:
	// Socket_base
	virtual result_t get_family(int32_t& retVal);
	virtual result_t get_type(int32_t& retVal);
	virtual result_t get_remoteAddress(std::string& retVal);
	virtual result_t get_remotePort(int32_t& retVal);
	virtual result_t get_localAddress(std::string& retVal);
	virtual result_t get_localPort(int32_t& retVal);
	virtual result_t connect(const char* addr, int32_t port, exlib::AsyncEvent* ac);
	virtual result_t asyncConnect(const char* addr, int32_t port);
	virtual result_t onconnect(v8::Handle<v8::Function> func);
	virtual result_t bind(const char* addr, int32_t port, bool allowIPv4);
	virtual result_t bind(int32_t port, bool allowIPv4);
	virtual result_t listen(int32_t backlog);
	virtual result_t accept(obj_ptr<Socket_base>& retVal, exlib::AsyncEvent* ac);
	virtual result_t asyncAccept();
	virtual result_t onaccept(v8::Handle<v8::Function> func);
	virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
			exlib::AsyncEvent* ac);
	virtual result_t recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal);
	virtual result_t send(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac);
	virtual result_t sendto(obj_ptr<Buffer_base>& data, const char* host,
			int32_t port);

public:
	result_t create(int32_t family, int32_t type);

private:
	result_t getAddrInfo(const char* addr, int32_t port, _sockaddr& addr_info);

private:
	SOCKET m_sock;
	int32_t m_family;
	int32_t m_type;

#ifdef _WIN32
	BOOL m_bBind;
#endif
};

int a_connect(SOCKET s, sockaddr* ai, int sz);
SOCKET a_accept(SOCKET s, sockaddr* ai, socklen_t* sz);
int a_recv(SOCKET s, char *p, size_t sz, int f);
int a_send(SOCKET s, const char *p, size_t sz, int f);

}

#endif /* SOCKET_H_ */
