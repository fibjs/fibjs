/*
 * Socket_ev.cpp
 *
 *  Created on: Apr 26, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include "ev.h"
#include  <fcntl.h>

namespace fibjs
{

static struct ev_loop *s_loop;

result_t net_base::backend(std::string& retVal)
{
	switch(ev_backend(s_loop))
	{
	case EVBACKEND_SELECT:
		retVal = "Select";
		break;
	case EVBACKEND_POLL:
		retVal = "Poll";
		break;
	case EVBACKEND_EPOLL:
		retVal = "EPoll";
		break;
	case EVBACKEND_KQUEUE:
		retVal = "KQueue";
		break;
	case EVBACKEND_DEVPOLL:
		retVal = "DevPoll";
		break;
	case EVBACKEND_PORT:
		retVal = "Port";
		break;
	}

	return 0;
}

class waitEV;

static ev_async s_asEvent;
static exlib::lockfree<waitEV> s_evWait;

class waitEV: public exlib::AsyncEvent, ev_io
{
public:
	waitEV(SOCKET s, int op)
	{
		ev_io_init(((ev_io*)this), io_cb, s, op);

		s_evWait.put(this);
		ev_async_send(s_loop, &s_asEvent);

		v8::Unlocker unlocker(isolate);
		wait();
	}

	inline void start()
	{
		ev_io_start(s_loop, this);
	}

private:
	static void io_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
	{
		ev_io_stop(s_loop, watcher);
		((waitEV*) watcher)->post();
	}
};

static class _acSocket: public exlib::Thread
{
public:
	_acSocket()
	{
		s_loop = EV_DEFAULT;
		start();
	}

	virtual void Run()
	{
		ev_async_init(&s_asEvent, as_cb);
		ev_async_start(s_loop, &s_asEvent);

		ev_run(s_loop, 0);
	}

private:
	static void as_cb(struct ev_loop *loop, struct ev_async *watcher,
			int revents)
	{
		waitEV *p = s_evWait.getList(), *p1;

		while (p)
		{
			p1 = (waitEV*) p->m_next;
			p->m_next = NULL;
			p->start();

			p = p1;
		}
	}
} s_acSock;

result_t Socket::accept(obj_ptr<Socket_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr ai;
	socklen_t sz = sizeof(_sockaddr);

	SOCKET s = ::accept(m_sock, (sockaddr*) &ai, &sz);
	if (s == INVALID_SOCKET)
	{
		int e = errno;
		if( e == EAGAIN || e == EWOULDBLOCK)
		{
			waitEV ac(m_sock, EV_READ);
			s = ::accept(m_sock, (sockaddr*) &ai, &sz);
		}
	}
	if (s == INVALID_SOCKET)
		return SocketError();

	obj_ptr<Socket> sock = new Socket();

	sock->m_sock = s;
	if (ai.addr6.sin6_family == PF_INET6)
		sock->m_family = _AF_INET6;

	fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);

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
		if (n == INVALID_SOCKET)
		{
			int e = errno;
			if( e == EAGAIN || e == EWOULDBLOCK)
			{
				waitEV ac(m_sock, EV_READ);
				n = (int) ::recv(m_sock, p, sz, MSG_NOSIGNAL);
			}
		}

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
		int n = (int) ::send(m_sock, p, sz, MSG_NOSIGNAL);
		if (n == INVALID_SOCKET)
		{
			int e = errno;
			if( e == EAGAIN || e == EWOULDBLOCK)
			{
				waitEV ac(m_sock, EV_WRITE);
				n = (int) ::send(m_sock, p, sz, MSG_NOSIGNAL);
			}
		}
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

}
