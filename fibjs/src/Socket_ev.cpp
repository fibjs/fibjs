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
	switch (ev_backend(s_loop))
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

SOCKET a_accept(SOCKET s, sockaddr* ai, socklen_t* sz)
{
	SOCKET c = ::accept(s, ai, sz);
	if (c == INVALID_SOCKET)
	{
		int e = errno;
		if (e == EAGAIN || e == EWOULDBLOCK)
		{
			waitEV ac(s, EV_READ);
			c = ::accept(s, ai, sz);
		}
	}

	return c;
}

int a_recv(SOCKET s, void *p, size_t sz, int f)
{
	int n = (int) ::recv(s, p, sz, f);
	if (n == INVALID_SOCKET)
	{
		int e = errno;
		if (e == EAGAIN || e == EWOULDBLOCK)
		{
			waitEV ac(s, EV_READ);
			n = (int) ::recv(s, p, sz, f);
		}
	}

	return n;
}

int a_send(SOCKET s, const void *p, size_t sz, int f)
{
	int n = (int) ::send(s, p, sz, f);
	if (n == INVALID_SOCKET)
	{
		int e = errno;
		if (e == EAGAIN || e == EWOULDBLOCK)
		{
			waitEV ac(s, EV_WRITE);
			n = (int) ::send(s, p, sz, f);
		}
	}

	return n;
}

}
