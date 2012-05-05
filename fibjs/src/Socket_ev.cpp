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

class asyncProc;

static ev_async s_asEvent;
static exlib::lockfree<asyncProc> s_evWait;

class asyncProc: public ev_io
{
public:
	asyncProc(SOCKET s, int op) :
			m_s(s), m_op(op), m_next(NULL)

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
		ev_io_init(((ev_io*)this), io_cb, m_s, m_op);
		ev_io_start(s_loop, this);
	}

	result_t call()
	{
		result_t hr = process();
		if (hr == CALL_E_PENDDING)
			post();

		return hr;
	}

	virtual result_t process()
	{
		return 0;
	}

	virtual void proc()
	{
	}

public:
	SOCKET m_s;
	int m_op;
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
			asyncProc(s, op)
	{
		post();
		m_aEvent.wait();
	}

	virtual void proc()
	{
		m_aEvent.post();
	}

public:
	exlib::AsyncEvent m_aEvent;
};

inline bool wouldBlock()
{
#ifdef _WIN32
	return WSAGetLastError() == WSAEWOULDBLOCK;
#else
	return errno == EWOULDBLOCK;
#endif
}

int a_connect(SOCKET s, sockaddr* ai, int sz)
{
	int n = ::connect(s, ai, sz);
	if (n == SOCKET_ERROR && (errno == EINPROGRESS))
	{
		asyncWait ac(s, EV_WRITE);
		_sockaddr addr_info;
		socklen_t sz1 = sizeof(addr_info);

		return ::getpeername(s, (sockaddr*) &addr_info, &sz1);
	}

	return 0;
}

SOCKET a_accept(SOCKET s, sockaddr* ai, socklen_t* sz)
{
	SOCKET c = ::accept(s, ai, sz);
	if (c == INVALID_SOCKET && wouldBlock())
	{
		asyncWait ac(s, EV_READ);
		c = ::accept(s, ai, sz);
	}

	return c;
}

int a_recv(SOCKET s, char *p, size_t sz, int f)
{
	int n = (int) ::recv(s, p, sz, f);
	if (n == SOCKET_ERROR && wouldBlock())
	{
		asyncWait ac(s, EV_READ);
		n = (int) ::recv(s, p, sz, f);
	}

	return n;
}

int a_send(SOCKET s, const char *p, size_t sz, int f)
{
	int n = (int) ::send(s, p, sz, f);
	if (n == SOCKET_ERROR && wouldBlock())
	{
		asyncWait ac(s, EV_WRITE);
		n = (int) ::send(s, p, sz, f);
	}

	return n;
}

result_t Socket::connect(const char* addr, int32_t port, AsyncCall* ac)
{
	class asyncConnect: public asyncProc
	{
	public:
		asyncConnect(SOCKET s, _sockaddr& ai, AsyncCall* ac) :
				asyncProc(s, EV_WRITE), m_ai(ai), m_ac(ac)
		{
		}

		virtual result_t process()
		{
			int n = ::connect(m_s, (struct sockaddr*) &m_ai,
					m_ai.addr4.sin_family == PF_INET ?
							sizeof(m_ai.addr4) : sizeof(m_ai.addr6));
			if (n == SOCKET_ERROR)
				return errno == EINPROGRESS ? CALL_E_PENDDING : SocketError();

			return 0;
		}

		virtual void proc()
		{
			_sockaddr addr_info;
			socklen_t sz1 = sizeof(addr_info);

			if (::getpeername(m_s, (sockaddr*) &addr_info, &sz1) == SOCKET_ERROR)
				m_ac->hr = -ETIMEDOUT;
			else
				m_ac->hr = 0;

			m_ac->post();
			delete this;
		}

	public:
		_sockaddr m_ai;
		AsyncCall* m_ac;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	_sockaddr addr_info;
	result_t hr = getAddrInfo(addr, port, addr_info);
	if (hr < 0)
		return hr;

	return (new asyncConnect(m_sock, addr_info, ac))->call();
}

inline void setNonBlock(SOCKET s)
{
#ifdef _WIN32
	u_long mode = 1;
	ioctlsocket(s, FIONBIO, &mode);
#else
	fcntl(s, F_SETFL, fcntl(s, F_GETFL, 0) | O_NONBLOCK);

#ifdef MacOS
	int set_option = 1;
	setsockopt(s, SOL_SOCKET, SO_NOSIGPIPE, &set_option, sizeof(set_option));
#endif

#endif
}

result_t Socket::accept(obj_ptr<Socket_base>& retVal, AsyncCall* ac)
{
	class asyncAccept: public asyncProc
	{
	public:
		asyncAccept(SOCKET s, obj_ptr<Socket_base>& retVal, AsyncCall* ac) :
				asyncProc(s, EV_READ), m_retVal(retVal), m_ac(ac)
		{
		}

		virtual result_t process()
		{
			_sockaddr ai;
			socklen_t sz = sizeof(ai);
			SOCKET c = ::accept(m_s, (sockaddr*) &ai, &sz);
			if (c == INVALID_SOCKET)
				return wouldBlock() ? CALL_E_PENDDING : SocketError();

			setNonBlock(c);
			obj_ptr<Socket> sock = new Socket(c,
					ai.addr6.sin6_family == PF_INET6 ?
							Socket::_AF_INET6 : Socket::_AF_INET,
					Socket::_SOCK_STREAM);

			m_retVal = sock;

			return 0;
		}

		virtual void proc()
		{
			m_ac->hr = process();
			m_ac->post();
			delete this;
		}

	public:
		obj_ptr<Socket_base>& m_retVal;
		AsyncCall* m_ac;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return (new asyncAccept(m_sock, retVal, ac))->call();
}

result_t Socket::recv(int32_t bytes, obj_ptr<Buffer_base>& retVal,
		AsyncCall* ac)
{
	class asyncRecv: public asyncProc
	{
	public:
		asyncRecv(SOCKET s, int32_t bytes, obj_ptr<Buffer_base>& retVal,
				AsyncCall* ac) :
				asyncProc(s, EV_READ), m_retVal(retVal), m_ac(ac)
		{
			m_buf.resize(bytes > 0 ? bytes : 4096);
		}

		virtual result_t process()
		{
			int n = (int) ::recv(m_s, &m_buf[0], m_buf.length(), MSG_NOSIGNAL);
			if (n == SOCKET_ERROR)
				return wouldBlock() ? CALL_E_PENDDING : SocketError();

			m_buf.resize(n);
			m_retVal = new Buffer(m_buf);

			return 0;
		}

		virtual void proc()
		{
			m_ac->hr = process();
			m_ac->post();
			delete this;
		}

	public:
		obj_ptr<Buffer_base>& m_retVal;
		std::string m_buf;
		AsyncCall* m_ac;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return (new asyncRecv(m_sock, bytes, retVal, ac))->call();
}

result_t Socket::send(obj_ptr<Buffer_base> data, AsyncCall* ac)
{
	class asyncSend: public asyncProc
	{
	public:
		asyncSend(SOCKET s, obj_ptr<Buffer_base> data, AsyncCall* ac) :
				asyncProc(s, EV_WRITE), m_ac(ac)
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
					return wouldBlock() ? CALL_E_PENDDING : SocketError();

				m_sz -= n;
				m_p += n;
			}

			return 0;
		}

		virtual void proc()
		{
			result_t hr = process();

			if (hr == 0 && m_sz > 0)
				post();
			else
			{
				m_ac->hr = hr;
				m_ac->post();
				delete this;
			}
		}

	public:
		std::string m_buf;
		const char* m_p;
		int m_sz;
		AsyncCall* m_ac;
	};

	if (m_sock == INVALID_SOCKET)
		return CALL_E_INVALID_CALL;

	return (new asyncSend(m_sock, data, ac))->call();
}

}
