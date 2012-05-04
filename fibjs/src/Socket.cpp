/*
 * Socket.cpp
 *
 *  Created on: Apr 22, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "Buffer.h"
#include <string.h>
#include  <fcntl.h>

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
	const char *curtok;
	int ch, saw_xdigit;
	unsigned val;

	tp = (unsigned char*) memset(tmp, '\0', NS_IN6ADDRSZ);
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':' && *++src != ':')
		return (0);

	curtok = src;
	saw_xdigit = 0;
	val = 0;

	while ((ch = *src++) != '\0')
	{
		if (ch == ':')
		{
			curtok = src;

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

		if (ch == '.')
		{
			if (((tp + NS_INADDRSZ) <= endp) && (inet_pton4(curtok, tp) == 0))
			{
				tp += NS_INADDRSZ;
				saw_xdigit = 0;
				break;
			}

			return -1;
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

#define MAX_IPv4_STR_LEN 16
#define MAX_IPv6_STR_LEN 64

static const char *hexchars = "0123456789abcdef";

static const char *inet_ntop4(const struct in_addr *addr, char *buf,
		socklen_t len)
{
	const unsigned char *ap = (const unsigned char *) &addr->s_addr;
	char tmp[MAX_IPv4_STR_LEN]; /* max length of ipv4 addr string */
	int fulllen;

	/*
	 * snprintf returns number of bytes printed (not including NULL) or
	 * number of bytes that would have been printed if more than would
	 * fit
	 */
	fulllen = sprintf(tmp, "%d.%d.%d.%d", ap[0], ap[1], ap[2], ap[3]);
	if (fulllen >= (int) len)
	{
		return NULL;
	}

	memcpy(buf, tmp, fulllen + 1);

	return buf;
}

static const char *inet_ntop6(const struct in6_addr *addr, char *dst,
		socklen_t size)
{
	char hexa[8][5], tmp[MAX_IPv6_STR_LEN];
	int zr[8];
	size_t len;
	int32_t i, j, k, skip;
	uint8_t x8, hx8;
	uint16_t x16;
	struct in_addr a4;

	if (addr == NULL)
		return NULL;

	memset(tmp, 0, sizeof(tmp));

	/*  check for mapped or compat addresses */
	i = IN6_IS_ADDR_V4MAPPED(addr);
	j = IN6_IS_ADDR_V4COMPAT(addr);
	if ((i != 0) || (j != 0))
	{
		char tmp2[16]; /* max length of ipv4 addr string */
		a4.s_addr = ((uint32_t*) &addr->s6_addr)[3];
		len = sprintf(tmp, "::%s%s", (i != 0) ? "ffff:" : "",
				inet_ntop4(&a4, tmp2, sizeof(tmp2)));
		if (len >= size)
			return NULL;
		memcpy(dst, tmp, len + 1);
		return dst;
	}

	k = 0;
	for (i = 0; i < 16; i += 2)
	{
		j = 0;
		skip = 1;

		memset(hexa[k], 0, 5);

		x8 = addr->s6_addr[i];

		hx8 = x8 >> 4;
		if (hx8 != 0)
		{
			skip = 0;
			hexa[k][j++] = hexchars[hx8];
		}

		hx8 = x8 & 0x0f;
		if ((skip == 0) || ((skip == 1) && (hx8 != 0)))
		{
			skip = 0;
			hexa[k][j++] = hexchars[hx8];
		}

		x8 = addr->s6_addr[i + 1];

		hx8 = x8 >> 4;
		if ((skip == 0) || ((skip == 1) && (hx8 != 0)))
		{
			hexa[k][j++] = hexchars[hx8];
		}

		hx8 = x8 & 0x0f;
		hexa[k][j++] = hexchars[hx8];

		k++;
	}

	/* find runs of zeros for :: convention */
	j = 0;
	for (i = 7; i >= 0; i--)
	{
		zr[i] = j;
		x16 = ((uint16_t*) &addr->s6_addr)[i];
		if (x16 == 0)
			j++;
		else
			j = 0;
		zr[i] = j;
	}

	/* find longest run of zeros */
	k = -1;
	j = 0;
	for (i = 0; i < 8; i++)
	{
		if (zr[i] > j)
		{
			k = i;
			j = zr[i];
		}
	}

	for (i = 0; i < 8; i++)
	{
		if (i != k)
			zr[i] = 0;
	}

	len = 0;
	for (i = 0; i < 8; i++)
	{
		if (zr[i] != 0)
		{
			/* check for leading zero */
			if (i == 0)
				tmp[len++] = ':';
			tmp[len++] = ':';
			i += (zr[i] - 1);
			continue;
		}
		for (j = 0; hexa[i][j] != '\0'; j++)
			tmp[len++] = hexa[i][j];
		if (i != 7)
			tmp[len++] = ':';
	}

	/* trailing NULL */
	len++;

	if (len > size)
		return NULL;

	memcpy(dst, tmp, len);
	return dst;
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
	close(NULL);
}

inline void setNonBlock(SOCKET s)
{
#ifdef _WIN32
	u_long mode = 1;
	ioctlsocket(s, FIONBIO, &mode);
#else
	fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);
#endif
}

result_t Socket::create(int32_t family, int32_t type)
{
	close(NULL);

	m_family = family;
	m_type = type;

	if (family == _AF_INET)
		family = AF_INET;
	else if (family == _AF_INET6)
		family = AF_INET6;
	else
		return CALL_E_INVALIDARG;

	if (type == _SOCK_STREAM)
		type = SOCK_STREAM;
	else if (type == _SOCK_DGRAM)
		type = SOCK_DGRAM;
	else
		return CALL_E_INVALIDARG;

	m_sock = socket(family, type, 0);
	if (m_sock == INVALID_SOCKET)
		return SocketError();

	setNonBlock(m_sock);

	return 0;
}

result_t Socket::read(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		AsyncCall* ac)
{
	return recv(bytes, retVal);
}

result_t Socket::write(obj_ptr<Buffer_base> data, AsyncCall* ac)
{
	return send(data);
}

result_t Socket::flush(AsyncCall* ac)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return 0;
}

result_t Socket::close(AsyncCall* ac)
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

result_t Socket::get_remoteAddress(std::string& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getpeername(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	char tmp[MAX_IPv6_STR_LEN];

	if (addr_info.addr6.sin6_family == PF_INET6)
		inet_ntop6(&addr_info.addr6.sin6_addr, tmp, MAX_IPv6_STR_LEN);
	else
		inet_ntop4(&addr_info.addr4.sin_addr, tmp, MAX_IPv6_STR_LEN);

	retVal = tmp;

	return 0;
}

result_t Socket::get_remotePort(int32_t& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getpeername(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	if (addr_info.addr6.sin6_family == PF_INET6)
		retVal = ntohs(addr_info.addr6.sin6_port);
	else
		retVal = ntohs(addr_info.addr4.sin_port);

	return 0;
}

result_t Socket::get_localAddress(std::string& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getsockname(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	char tmp[MAX_IPv6_STR_LEN];

	if (addr_info.addr6.sin6_family == PF_INET6)
		inet_ntop6(&addr_info.addr6.sin6_addr, tmp, MAX_IPv6_STR_LEN);
	else
		inet_ntop4(&addr_info.addr4.sin_addr, tmp, MAX_IPv6_STR_LEN);

	retVal = tmp;

	return 0;
}

result_t Socket::get_localPort(int32_t& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	socklen_t sz = sizeof(addr_info);

	if (::getsockname(m_sock, (sockaddr*) &addr_info, &sz) == SOCKET_ERROR)
		return SocketError();

	if (addr_info.addr6.sin6_family == PF_INET6)
		retVal = ntohs(addr_info.addr6.sin6_port);
	else
		retVal = ntohs(addr_info.addr4.sin_port);

	return 0;
}

result_t Socket::getAddrInfo(const char* addr, int32_t port,
		_sockaddr& addr_info)
{
	memset(&addr_info, 0, sizeof(addr_info));

	if (m_family == _AF_INET)
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

result_t Socket::bind(const char* addr, int32_t port, bool allowIPv4)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	result_t hr = getAddrInfo(addr, port, addr_info);
	if (hr < 0)
		return hr;

	int on = 1;
	setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &on, sizeof(on));

	if (m_family == _AF_INET6)
	{
		if (allowIPv4)
			on = 0;

		setsockopt(m_sock, IPPROTO_IPV6, IPV6_V6ONLY, (const char*) &on,
				sizeof(on));
	}

	if (::bind(m_sock, (struct sockaddr*) &addr_info,
			m_family == _AF_INET ?
					sizeof(addr_info.addr4) :
					sizeof(addr_info.addr6)) == SOCKET_ERROR)
		return SocketError();

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
		int n = (int) a_recv(m_sock, p, sz, MSG_NOSIGNAL);
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

result_t Socket::send(const char* p, int sz)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	while (sz)
	{
		int n = (int) a_send(m_sock, p, sz, MSG_NOSIGNAL);
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

result_t Socket::recvFrom(int32_t bytes, obj_ptr<Buffer_base>& retVal)
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

result_t Socket::recv(v8::Handle<v8::Function> cb, obj_ptr<Buffer_base>& retVal)
{
	return 0;
}

result_t Socket::recv(int32_t bytes, v8::Handle<v8::Function> cb,
		obj_ptr<Buffer_base>& retVal)
{
	return 0;
}

result_t Socket::send(obj_ptr<Buffer_base> data, v8::Handle<v8::Function> cb)
{
	return 0;
}

}
