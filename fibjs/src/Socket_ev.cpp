/*
 * Socket_ev.cpp
 *
 *  Created on: Apr 26, 2012
 *      Author: lion
 */

#ifndef _WIN32

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

class asyncProc;

static ev_async s_asEvent;
static exlib::lockfree<asyncProc> s_evWait;

class asyncProc: public ev_io
{
public:
	asyncProc(SOCKET s, int op, exlib::AsyncEvent* ac) :
			m_s(s), m_op(op), m_ac(ac), m_next(NULL)
	{
	}

	virtual ~asyncProc()
	{
	}

	void post()
	{
		s_evWait.put(this);
		ev_async_send(s_loop, &s_asEvent);
	}

	inline void start()
	{
		ev_io* io = (ev_io*) this;
		ev_io_init(io, io_cb, m_s, m_op);
		ev_io_start(s_loop, this);
	}

	result_t call()
	{
		result_t hr = process();
		if (hr == CALL_E_PENDDING)
			post();
		else
			delete this;

		return hr;
	}

	virtual result_t process()
	{
		return 0;
	}

	virtual void proc()
	{
		m_ac->post(process());
		delete this;
	}

public:
	SOCKET m_s;
	int m_op;
	exlib::AsyncEvent* m_ac;
	asyncProc* m_next;

private:
	static void io_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
	{
		ev_io_stop(s_loop, watcher);
		((asyncProc*) watcher)->proc();
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

		ev_timer tm;
		tm_cb(s_loop, &tm, 0);

		ev_run(s_loop, 0);
	}

private:
	static void doAsync()
	{
		asyncProc *p = s_evWait.getList(), *p1;

		while (p)
		{
			p1 = (asyncProc*) p->m_next;
			p->m_next = NULL;
			p->start();

			p = p1;
		}
	}

	static void tm_cb(struct ev_loop *loop, struct ev_timer *watcher,
			int revents)
	{
		ev_timer_init(watcher, tm_cb, 1, 0);
		ev_timer_start(s_loop, watcher);

		doAsync();
	}

	static void as_cb(struct ev_loop *loop, struct ev_async *watcher,
			int revents)
	{
		doAsync();
	}
} s_acSock;

class asyncWait: public asyncProc
{
public:
	asyncWait(SOCKET s, int op) :
			asyncProc(s, op, NULL)
	{
		m_ac = &m_aEvent;
		post();
		m_aEvent.wait();
	}

public:
	exlib::AsyncEvent m_aEvent;
};

int a_connect(SOCKET s, sockaddr* ai, int sz)
{
	int n = ::connect(s, ai, sz);
	if (n == SOCKET_ERROR && (errno == EINPROGRESS))
	{
		asyncWait ac(s, EV_WRITE);
		inetAddr addr_info;
		socklen_t sz1 = sizeof(addr_info);

		return ::getpeername(s, (sockaddr*) &addr_info, &sz1);
	}

	return 0;
}

SOCKET a_accept(SOCKET s, sockaddr* ai, socklen_t* sz)
{
	SOCKET c = ::accept(s, ai, sz);
	if (c == INVALID_SOCKET && (errno == EWOULDBLOCK))
	{
		asyncWait ac(s, EV_READ);
		c = ::accept(s, ai, sz);
	}

	return c;
}

int a_recv(SOCKET s, char *p, size_t sz, int f)
{
	int n = (int) ::recv(s, p, sz, f);
	if (n == SOCKET_ERROR && (errno == EWOULDBLOCK))
	{
		asyncWait ac(s, EV_READ);
		n = (int) ::recv(s, p, sz, f);
	}

	return n;
}

int a_send(SOCKET s, const char *p, size_t sz, int f)
{
	int n = (int) ::send(s, p, sz, f);
	if (n == SOCKET_ERROR && (errno == EWOULDBLOCK))
	{
		asyncWait ac(s, EV_WRITE);
		n = (int) ::send(s, p, sz, f);
	}

	return n;
}

result_t Socket::connect(const char* addr, int32_t port, exlib::AsyncEvent* ac)
{
	class asyncConnect: public asyncProc
	{
	public:
		asyncConnect(SOCKET s, inetAddr& ai, exlib::AsyncEvent* ac) :
				asyncProc(s, EV_WRITE, ac), m_ai(ai)
		{
		}

		virtual result_t process()
		{
			int n = ::connect(m_s, (struct sockaddr*) &m_ai, m_ai.size());
			if (n == SOCKET_ERROR)
			{
				int nError = errno;
				return (nError == EINPROGRESS) ? CALL_E_PENDDING : -nError;
			}

			return 0;
		}

		virtual void proc()
		{
			inetAddr addr_info;
			socklen_t sz1 = sizeof(addr_info);

			if (::getpeername(m_s, (sockaddr*) &addr_info, &sz1) == SOCKET_ERROR)
				m_ac->post(-ETIMEDOUT);
			else
				m_ac->post(0);

			delete this;
		}

	public:
		inetAddr m_ai;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	inetAddr addr_info;

	addr_info.init(m_family);
	addr_info.setPort(port);
	if (addr_info.addr(addr) < 0)
	{
		std::string strAddr;
		result_t hr = net_base::resolve(addr, m_family, strAddr, ac);
		if (hr < 0)
			return hr;

		if (addr_info.addr(strAddr.c_str()) < 0)
			return CALL_E_INVALIDARG;
	}

	return (new asyncConnect(m_sock, addr_info, ac))->call();
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal, exlib::AsyncEvent* ac)
{
	class asyncAccept: public asyncProc
	{
	public:
		asyncAccept(SOCKET s, obj_ptr<Socket_base>& retVal,
				exlib::AsyncEvent* ac) :
				asyncProc(s, EV_READ, ac), m_retVal(retVal)
		{
		}

		virtual result_t process()
		{
			inetAddr ai;
			socklen_t sz = sizeof(ai);
			SOCKET c = ::accept(m_s, (sockaddr*) &ai, &sz);
			if (c == INVALID_SOCKET)
			{
				int nError = errno;
				return (nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError;
			}

			fcntl(c, F_SETFL, fcntl(c, F_GETFL, 0) | O_NONBLOCK);

#ifdef MacOS
			int set_option = 1;
			setsockopt(c, SOL_SOCKET, SO_NOSIGPIPE, &set_option,
					sizeof(set_option));
#endif

			obj_ptr<Socket> sock = new Socket(c, ai.family(),
					net_base::_SOCK_STREAM);

			m_retVal = sock;

			return 0;
		}

	public:
		obj_ptr<Socket_base>& m_retVal;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncAccept(m_sock, retVal, ac))->call();
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncRecv: public asyncProc
	{
	public:
		asyncRecv(SOCKET s, int32_t bytes, obj_ptr<Buffer_base>& retVal,
				exlib::AsyncEvent* ac) :
				asyncProc(s, EV_READ, ac), m_retVal(retVal)
		{
			m_buf.resize(bytes > 0 ? bytes : STREAM_BUFF_SIZE);
		}

		virtual result_t process()
		{
			int n = (int) ::recv(m_s, &m_buf[0], m_buf.length(), MSG_NOSIGNAL);
			if (n == SOCKET_ERROR)
			{
				int nError = errno;
				if (nError == ECONNRESET)
					return 0;
				return (nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError;
			}

			if (n > 0)
			{
				m_buf.resize(n);
				m_retVal = new Buffer(m_buf);
			}

			return 0;
		}

	public:
		obj_ptr<Buffer_base>& m_retVal;
		std::string m_buf;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncRecv(m_sock, bytes, retVal, ac))->call();
}

result_t Socket::send(obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac)
{
	class asyncSend: public asyncProc
	{
	public:
		asyncSend(SOCKET s, obj_ptr<Buffer_base>& data, exlib::AsyncEvent* ac) :
				asyncProc(s, EV_WRITE, ac)
		{
			data->toString(m_buf);
			m_p = m_buf.c_str();
			m_sz = m_buf.length();
		}

		virtual result_t process()
		{
			while (m_sz)
			{
				int n = (int) ::send(m_s, m_p, m_sz, MSG_NOSIGNAL);
				if (n == SOCKET_ERROR)
				{
					int nError = errno;
					return (nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError;
				}

				m_sz -= n;
				m_p += n;
			}

			return 0;
		}

		virtual void proc()
		{
			result_t hr = process();

			if (hr == CALL_E_PENDDING)
				post();
			else
			{
				m_ac->post(hr);
				delete this;
			}
		}

	public:
		std::string m_buf;
		const char* m_p;
		int m_sz;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncSend(m_sock, data, ac))->call();
}

}

#endif
