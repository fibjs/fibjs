/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "ifs/net.h"
#include <string.h>

namespace fibjs
{

#define NS_INADDRSZ 4
#define NS_IN6ADDRSZ 16
#define NS_INT16SZ 2

static int inet_pton4(const char * src, unsigned char * dst)
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
				return (0);
			*tp = p;
			if (!saw_digit)
			{
				if (++octets > 4)
					return (0);
				saw_digit = 1;
			}
		}
		else if (ch == '.' && saw_digit)
		{
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		}
		else
			return (0);
	}
	if (octets < 4)
		return (0);
	memcpy(dst, tmp, NS_INADDRSZ);
	return (1);
}

static int inet_pton6(const char * src, struct in6_addr* dst)
{
	static const char xdigits[] = "0123456789abcdef";
	unsigned char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const char *curtok;
	int ch, saw_xdigit;
	unsigned val;

	tp = (unsigned char*) memset(tmp, '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = tolower(*src++)) != '\0')
	{
		const char *pch;

		pch = strchr(xdigits, ch);
		if (pch != NULL)
		{
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':')
		{
			curtok = src;
			if (!saw_xdigit)
			{
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			}
			else if (*src == '\0')
			{
				return (0);
			}
			if (tp + NS_INT16SZ > endp)
				return (0);
			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp)
				&& inet_pton4(curtok, tp) > 0)
		{
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break; /* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit)
	{
		if (tp + NS_INT16SZ > endp)
			return (0);
		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}
	if (colonp != NULL)
	{
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		if (tp == endp)
			return (0);
		for (i = 1; i <= n; i++)
		{
			endp[-i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);

	memcpy(dst, tmp, NS_IN6ADDRSZ);
	return (1);
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

result_t Socket::connect(const char* host, int32_t port)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (m_family == net_base::_AF_INET)
	{
		struct sockaddr_in addr;

		memset(&addr, 0, sizeof(addr));

		addr.sin_family = PF_INET;

		if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
			return CALL_E_INVALIDARG;

		addr.sin_port = htons(port);

		if (::connect(m_sock, (struct sockaddr*) &addr,
				sizeof(addr)) == SOCKET_ERROR)
			return SocketError();
	}
	else
	{
		struct sockaddr_in6 addr;

		memset(&addr, 0, sizeof(addr));

		addr.sin6_family = PF_INET6;
		if (inet_pton6(host, &addr.sin6_addr) < 0)
			return CALL_E_INVALIDARG;

		addr.sin6_port = htons(port);

		if (::connect(m_sock, (struct sockaddr*) &addr,
				sizeof(addr)) == SOCKET_ERROR)
			return SocketError();
	}

	return 0;
}

result_t Socket::bind(const char* addr, int32_t port)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::bind(int32_t port)
{
	return bind("", port);
}

result_t Socket::listen()
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::recv(int32_t size, obj_ptr<Buffer_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::recvFrom(int32_t size, obj_ptr<Buffer_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::send(obj_ptr<Buffer_base> data)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;





	return 0;
}

result_t Socket::sendto(obj_ptr<Buffer_base> data, const char* host,
		int32_t port)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

}
