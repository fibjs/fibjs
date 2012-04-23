/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include <string.h>

namespace fibjs
{

#define NS_INADDRSZ 4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ 2

static int inet_pton4(const char * src, void* dst)
{
	int saw_digit, octets, ch;
	unsigned char tmp[NS_INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;

	while ((ch = *src++) != '\0')
	{
		if (ch >= '0' && ch <= '9')
		{
			unsigned p = *tp * 10 + (ch - '0');

			if (saw_digit && *tp == 0)
				return (0);

			if (p > 255)
				return -1;

			*tp = p;

			if (!saw_digit)
			{
				if (++octets > 4)
					return -1;
				saw_digit = 1;
			}
		}
		else if (ch == '.' && saw_digit)
		{
			if (octets == 4)
				return -1;

			*++tp = 0;
			saw_digit = 0;
		}
		else
			return -1;
	}

	if (octets < 4)
		return -1;

	memcpy(dst, tmp, NS_INADDRSZ);

	return 0;
}

static int inet_pton6(const char * src, void* dst)
{
	unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	int ch, saw_xdigit;
	unsigned val;

	tp = (unsigned char*) memset(tmp, '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':' && *++src != ':')
		return (0);

	saw_xdigit = 0;
	val = 0;

	while ((ch = *src++) != '\0')
	{
		if (ch == ':')
		{
			if (!saw_xdigit)
			{
				if (colonp)
					return -1;

				colonp = tp;
				continue;
			}
			else if (*src == '\0')
				return -1;

			if (tp + NS_INT16SZ > endp)
				return -1;

			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}

		if (ch >= '0' && ch <= '9')
			ch = ch - '0';
		else if (ch >= 'a' && ch <= 'f')
			ch = ch - 'a' + 10;
		else if (ch >= 'A' && ch <= 'f')
			ch = ch - 'A' + 10;
		else
			return -1;

		val <<= 4;
		val |= ch;

		if (val > 0xffff)
			return -1;
		saw_xdigit = 1;
	}

	if (saw_xdigit)
	{
		if (tp + NS_INT16SZ > endp)
			return -1;

		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}

	if (colonp != NULL)
	{
		const int n = (int) (tp - colonp);
		int i;

		if (tp == endp)
			return -1;

		for (i = 1; i <= n; i++)
		{
			endp[-i] = colonp[n - i];
			colonp[n - i] = 0;
		}

		tp = endp;
	}

	if (tp != endp)
		return -1;

	memcpy(dst, tmp, NS_IN6ADDRSZ);

	return 0;
}

#ifdef _WIN32
static class _initWinSock
{
public:
	_initWinSock()
	{
		WSADATA wsaData;
		int iResult;

		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			exit(-1);
		}
	}
}s_initWinSock;
#endif

Socket::Socket()
{
	m_family = net_base::_AF_INET;
	m_type = net_base::_SOCK_STREAM;

	m_sock = INVALID_SOCKET;
}

Socket::~Socket()
{
	close();
}

result_t Socket::create(int32_t family, int32_t type)
{
	close();

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

	m_sock = socket(family, type, 0);
	if (m_sock == INVALID_SOCKET)
		return SocketError();

	return 0;
}

result_t Socket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal)
{
	return recv(bytes, retVal);
}

result_t Socket::write(obj_ptr<Buffer_base> data)
{
	return send(data);
}

result_t Socket::flush()
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::close()
{
	if (m_sock != INVALID_SOCKET)
		::closesocket(m_sock);

	m_sock = INVALID_SOCKET;

	return 0;
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

result_t Socket::getAddrInfo(const char* addr, int32_t port,
		_sockaddr& addr_info)
{
	memset(&addr_info, 0, sizeof(addr_info));

	if (m_family == net_base::_AF_INET)
	{
		addr_info.addr4.sin_family = PF_INET;
		addr_info.addr4.sin_port = htons(port);

		if (addr == NULL)
			addr_info.addr4.sin_addr.s_addr = INADDR_ANY;
		else if (inet_pton4(addr, &addr_info.addr4.sin_addr.s_addr) < 0)
			return CALL_E_INVALIDARG;
	}
	else
	{
		addr_info.addr6.sin6_family = PF_INET6;
		addr_info.addr6.sin6_port = htons(port);

		if (addr == NULL)
			addr_info.addr6.sin6_addr = in6addr_any;
		else if (inet_pton6(addr, &addr_info.addr6.sin6_addr) < 0)
			return CALL_E_INVALIDARG;


	}

	return 0;
}

result_t Socket::connect(const char* addr, int32_t port)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	result_t hr = getAddrInfo(addr, port, addr_info);
	if (hr < 0)
		return hr;

	if (::connect(m_sock, (struct sockaddr*) &addr_info, sizeof(addr_info))
			== SOCKET_ERROR)
		return SocketError();

	return 0;
}

result_t Socket::bind(const char* addr, int32_t port, bool onlyIPv6)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	result_t hr = getAddrInfo(addr, port, addr_info);
	if (hr < 0)
		return hr;

	int on = 1;
	setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on));

	if (m_family == net_base::_AF_INET6)
	{
		if (!onlyIPv6)
			on = 0;
		setsockopt(m_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on, sizeof(on));
	}

	if (::bind(m_sock, (struct sockaddr*) &addr_info, sizeof(addr_info))
			== SOCKET_ERROR)
		return SocketError();

	return 0;
}

result_t Socket::bind(int32_t port, bool onlyIPv6)
{
	return bind(NULL, port, onlyIPv6);
}

result_t Socket::listen(int32_t backlog)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (::listen(m_sock, backlog) == SOCKET_ERROR)
		return SocketError();

	return 0;
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	socklen_t sz = sizeof(addr_info);

	memset(&addr_info, 0, sizeof(addr_info));

	SOCKET s = ::accept(m_sock, (sockaddr*) &addr_info, &sz);
	if (s == INVALID_SOCKET)
		return SocketError();

	obj_ptr<Socket> sock = new Socket();

	sock->m_sock = s;
	if (addr_info.addr6.sin6_family == PF_INET6)
		sock->m_family = net_base::_AF_INET6;

	retVal = sock;

	return 0;
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal)
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

result_t Socket::recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
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

result_t Socket::send(obj_ptr<Buffer_base> data)
{
	std::string strBuf;
	data->toString(strBuf);

	return send(strBuf.c_str(), (int) strBuf.length());
}

result_t Socket::sendto(obj_ptr<Buffer_base> data, const char* host,
		int32_t port)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

}
