/*
 * Socket_ev.cpp
 *
 *  Created on: Apr 26, 2012
 *      Author: lion
 */

#include "Socket.h"
#include "Buffer.h"
#include "ev.h"

namespace fibjs
{

static struct ev_loop *s_loop;

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
		ev_signal sig;
		ev_signal_init(&sig, sig_cb, SIGUSR1);
		ev_signal_start(s_loop, &sig);

		ev_run(s_loop, 0);
	}

private:
	static void sig_cb(struct ev_loop *loop, ev_signal *w, int revents)
	{
	}
} s_acSock;

class _accept: public exlib::AsyncEvent, ev_async, ev_io
{
public:
	_accept(SOCKET s, int op)
	{
		ev_io_init(((ev_io*)this), cb, s, op);
		ev_async_init(((ev_async*)this), as_cb);
		ev_async_start(s_loop, this);
		ev_async_send(s_loop, this);

		v8::Unlocker unlocker(isolate);
		wait();
	}

private:
	static void as_cb(struct ev_loop *loop, struct ev_async *watcher,
			int revents)
	{
		ev_async_stop(s_loop, (_accept*) watcher);
		ev_io_start(s_loop, (_accept*) watcher);
	}

	static void cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
	{
		ev_io_stop(s_loop, (_accept*) watcher);
		((_accept*) watcher)->post();
	}
};

result_t Socket::accept(obj_ptr<Socket_base>& retVal)
{
	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr ai;
	socklen_t sz = sizeof(_sockaddr);

	_accept ac(m_sock, EV_READ);
	SOCKET s = ::accept(m_sock, (sockaddr*) &ai, &sz);
	if (s == INVALID_SOCKET)
		return SocketError();

	obj_ptr<Socket> sock = new Socket();

	sock->m_sock = s;
	if (ai.addr6.sin6_family == PF_INET6)
		sock->m_family = _AF_INET6;

	retVal = sock;

	return 0;
}

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
