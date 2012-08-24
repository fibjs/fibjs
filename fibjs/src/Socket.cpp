/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "Buffer.h"
#include "Stat.h"
#include <string.h>
#include <fcntl.h>

namespace fibjs
{

result_t Socket_base::_new(int32_t family, int32_t type,
		obj_ptr<Socket_base>& retVal)
{
	obj_ptr<Socket> sock = new Socket();

	result_t hr = sock->create(family, type);
	if (hr < 0)
		return hr;

	retVal = sock;
	return 0;
}

Socket::~Socket()
{
	asyncEvent ac;
	close(&ac);
}

#ifdef _WIN32
extern HANDLE s_hIocp;
#endif

result_t Socket::create(int32_t family, int32_t type)
{
	asyncEvent ac;
	close(&ac);

	m_family = family;
	m_type = type;

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

#ifdef _WIN32

	m_sock = WSASocket(family, type, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_sock == INVALID_SOCKET)
	return SocketError();

	CreateIoCompletionPort((HANDLE) m_sock, s_hIocp, 0, 0);

#else

	m_sock = socket(family, type, 0);
	if (m_sock == INVALID_SOCKET)
		return SocketError();

	fcntl(m_sock, F_SETFL, fcntl(m_sock, F_GETFL, 0) | O_NONBLOCK);

#endif

#ifdef MacOS

	int set_option = 1;
	setsockopt(m_sock, SOL_SOCKET, SO_NOSIGPIPE, &set_option,
			sizeof(set_option));

#endif

	return 0;
}

result_t Socket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	return recv(bytes, retVal, ac, bytes > 0);
}

result_t Socket::asyncRead(int32_t bytes)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_read(bytes);
	return 0;
}

result_t Socket::onread(v8::Handle<v8::Function> func)
{
	return on("read", func);
}

result_t Socket::write(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac)
{
	return send(data, ac);
}

result_t Socket::asyncWrite(obj_ptr<Buffer_base>& data)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_write(data);
	return 0;
}

result_t Socket::onwrite(v8::Handle<v8::Function> func)
{
	return on("write", func);
}

result_t Socket::copyTo(obj_ptr<Stream_base>& stm, int64_t bytes,
		int64_t& retVal, exlib::AsyncEvent* ac)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return copyStream(this, stm, bytes, retVal, ac);
}

result_t Socket::asyncCopyTo(obj_ptr<Stream_base>& stm, int64_t bytes)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_copyTo(stm, bytes);
	return 0;
}

result_t Socket::oncopyto(v8::Handle<v8::Function> func)
{
	return on("copyto", func);
}

result_t Socket::stat(obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	obj_ptr<Stat> st = new Stat();

	st->init();
	st->m_isSocket = true;

	return 0;
}

result_t Socket::asyncStat()
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_stat();
	return 0;
}

result_t Socket::onstat(v8::Handle<v8::Function> func)
{
	return on("stat", func);
}

result_t Socket::close(exlib::AsyncEvent* ac)
{
	if (m_sock == INVALID_SOCKET)
		return 0;

	if (!ac)
		return CALL_E_NOSYNC;

	if (m_sock != INVALID_SOCKET)
	{
		::shutdown(m_sock, SHUT_RDWR);
		::closesocket(m_sock);
	}

	m_sock = INVALID_SOCKET;

	return 0;
}

result_t Socket::asyncClose()
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_close();
	return 0;
}

result_t Socket::onclose(v8::Handle<v8::Function> func)
{
	return on("close", func);
}

result_t Socket::onerror(v8::Handle<v8::Function> func)
{
	return on("error", func);
}

result_t Socket::get_family(int32_t& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	retVal = m_family;

	return 0;
}

result_t Socket::get_type(int32_t& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	retVal = m_type;

	return 0;
}

result_t Socket::get_remoteAddress(std::string& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	inetAddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getpeername(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	retVal = addr_info.str();

	return 0;
}

result_t Socket::get_remotePort(int32_t& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	inetAddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getpeername(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	retVal = ntohs(addr_info.port());

	return 0;
}

result_t Socket::get_localAddress(std::string& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	inetAddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getsockname(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	retVal = addr_info.str();

	return 0;
}

result_t Socket::get_localPort(int32_t& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	inetAddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getsockname(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	retVal = ntohs(addr_info.port());

	return 0;
}

result_t Socket::bind(const char* addr, int32_t port, bool allowIPv4)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	inetAddr addr_info;

	addr_info.init(m_family);
	addr_info.setPort(port);
	if (addr_info.addr(addr) < 0)
		return CALL_E_INVALIDARG;

	int on = 1;
	setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on));

	if (m_family == net_base::_AF_INET6)
	{
		if (allowIPv4)
			on = 0;

		setsockopt(m_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on,
				sizeof(on));
	}

	if (::bind(m_sock, (struct sockaddr*) &addr_info,
			addr_info.size()) == SOCKET_ERROR)
		return SocketError();

#ifdef _WIN32
	m_bBind = TRUE;
#endif

	return 0;
}

result_t Socket::bind(int32_t port, bool allowIPv4)
{
	return bind(NULL, port, allowIPv4);
}

result_t Socket::listen(int32_t backlog)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (::listen(m_sock, backlog) == SOCKET_ERROR)
		return SocketError();

	return 0;
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	return recv(bytes, retVal, ac, false);
}

result_t Socket::asyncRecv(int32_t bytes)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_recv(bytes);
	return 0;
}

result_t Socket::onrecv(v8::Handle<v8::Function> func)
{
	return on("recv", func);
}

result_t Socket::asyncSend(obj_ptr<Buffer_base>& data)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	acb_send(data);
	return 0;
}

result_t Socket::onsend(v8::Handle<v8::Function> func)
{
	return on("send", func);
}

result_t Socket::recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::sendto(obj_ptr<Buffer_base>& data, const char* host,
		int32_t port)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::asyncConnect(const char* host, int32_t port)
{
	std::string strHost(host);

	acb_connect(strHost, port);
	return 0;
}

result_t Socket::onconnect(v8::Handle<v8::Function> func)
{
	return on("connect", func);
}

result_t Socket::asyncAccept()
{
	acb_accept();
	return 0;
}

result_t Socket::onaccept(v8::Handle<v8::Function> func)
{
	return on("accept", func);
}

}
