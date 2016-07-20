/*
 * AsyncIO_ev.cpp
 *
 *  Created on: Feb 11, 2016
 *      Author: lion
 */

#ifndef _WIN32

#include "object.h"
#include "utils.h"
#include "AsyncIO.h"
#include "Socket.h"
#include "ifs/net.h"
#include "Buffer.h"
#include <ev/ev.h>
#include <fcntl.h>
#include <exlib/include/thread.h>

namespace fibjs
{

void setOption(intptr_t s)
{
	int32_t keepAlive = 1;
	setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (void *) &keepAlive,
	           sizeof(keepAlive));

#ifdef TCP_KEEPIDLE
	int32_t keepIdle = KEEPALIVE_TIMEOUT;
	setsockopt(s, SOL_TCP, TCP_KEEPIDLE, (void *) &keepIdle, sizeof(keepIdle));
#endif

#ifdef TCP_KEEPINTVL
	int32_t keepInterval = 20;
	int32_t keepCount = 10;

	setsockopt(s, SOL_TCP, TCP_KEEPINTVL, (void *) &keepInterval,
	           sizeof(keepInterval));
	setsockopt(s, SOL_TCP, TCP_KEEPCNT, (void *) &keepCount, sizeof(keepCount));
#endif

	int32_t noDelay = 1;

	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (void *) &noDelay, sizeof(noDelay));
}

static struct ev_loop *s_loop;

result_t net_base::backend(exlib::string &retVal)
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

class asyncEv;

static ev_async s_asEvent;
static exlib::LockedList<asyncEv> s_evWait;

class asyncEv: public ev_io,
	public exlib::linkitem
{
public:
	virtual ~asyncEv()
	{
	}

	void post()
	{
		s_evWait.putTail(this);
		ev_async_send(s_loop, &s_asEvent);
	}

	virtual void start()
	{
	}
};

class asyncProc: public asyncEv
{
public:
	asyncProc(intptr_t s, int32_t op, AsyncEvent *ac, intptr_t &guard, void *&opt) :
		m_s(s), m_op(op), m_ac(ac), m_guard(guard), m_opt(opt)
	{
	}

	virtual void start()
	{
		m_opt = this;
		ev_io *io = (ev_io *) this;
		ev_io_init(io, io_cb, m_s, m_op);
		ev_io_start(s_loop, this);
	}

	result_t call()
	{
		result_t hr = process();
		if (hr == CALL_E_PENDDING)
			post();
		else
		{
			m_guard = 0;
			delete this;
		}

		return hr;
	}

	virtual result_t process()
	{
		return 0;
	}

	virtual void proc()
	{
		ready(process());
	}

	void ready(int32_t v)
	{
		m_opt = NULL;
		m_guard = 0;
		m_ac->apost(v);
		delete this;
	}

	void onready()
	{
		ev_io_stop(s_loop, this);
		proc();
	}

public:
	intptr_t m_s;
	int32_t m_op;
	AsyncEvent *m_ac;
	intptr_t &m_guard;
	void *&m_opt;

private:
	static void io_cb(struct ev_loop *loop, struct ev_io *watcher, int32_t revents)
	{
		((asyncProc *) watcher)->onready();
	}
};

class _acIO: public exlib::OSThread
{
public:
	_acIO()
	{
		s_loop = EV_DEFAULT;
	}

	virtual void Run()
	{
		ev_async_init(&s_asEvent, as_cb);
		ev_async_start(s_loop, &s_asEvent);

		ev_timer tm;
		tm_cb(s_loop, &tm, 0);

		Runtime rt(NULL);

		ev_run(s_loop, 0);
	}

private:
	static void doAsync()
	{
		exlib::List<asyncEv> jobs;
		asyncEv *p1;

		s_evWait.getList(jobs);

		while ((p1 = jobs.getHead()) != 0)
			p1->start();
	}

	static void tm_cb(struct ev_loop *loop, struct ev_timer *watcher,
	                  int32_t revents)
	{
		ev_timer_init(watcher, tm_cb, 10, 0);
		ev_timer_start(s_loop, watcher);

		doAsync();
	}

	static void as_cb(struct ev_loop *loop, struct ev_async *watcher,
	                  int32_t revents)
	{
		doAsync();
	}
};

void init_aio()
{
	static _acIO s_acIO;
	s_acIO.start();
}

result_t AsyncIO::cancel(AsyncEvent *ac)
{
	class asyncCancel: public asyncEv
	{
	public:
		asyncCancel(void *&opt1, void *&opt2, AsyncEvent *ac) :
			m_ac(ac), m_opt1(opt1), m_opt2(opt2)
		{
		}

		virtual void start()
		{
			if (m_opt1)
				((asyncProc *) m_opt1)->onready();

			if (m_opt2)
				((asyncProc *) m_opt2)->onready();

			m_ac->apost(0);
			delete this;
		}

	public:
		AsyncEvent *m_ac;
		void *&m_opt1;
		void *&m_opt2;
	};

	if (m_inRecv || m_inSend)
	{
		(new asyncCancel(m_RecvOpt, m_SendOpt, ac))->post();
		return CHECK_ERROR(CALL_E_PENDDING);
	}

	return 0;
}

result_t AsyncIO::connect(exlib::string host, int32_t port, AsyncEvent *ac)
{
	class asyncConnect: public asyncProc
	{
	public:
		asyncConnect(intptr_t s, inetAddr &ai, AsyncEvent *ac, intptr_t &guard, void *&opt) :
			asyncProc(s, EV_WRITE, ac, guard, opt), m_ai(ai)
		{
		}

		virtual result_t process()
		{
			int32_t n = ::connect(m_s, (struct sockaddr *) &m_ai, m_ai.size());
			if (n == SOCKET_ERROR)
			{
				int32_t nError = errno;
				return CHECK_ERROR((nError == EINPROGRESS) ? CALL_E_PENDDING : -nError);
			}

			return 0;
		}

		virtual void proc()
		{
			inetAddr addr_info;
			socklen_t sz1 = sizeof(addr_info);

			if (::getpeername(m_s, (sockaddr *) &addr_info, &sz1) == SOCKET_ERROR)
				ready(-ECONNREFUSED);
			else
			{
				setOption(m_s);
				ready(0);
			}
		}

	public:
		inetAddr m_ai;
	};

	if (m_fd == INVALID_SOCKET)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	inetAddr addr_info;

	addr_info.init(m_family);
	addr_info.setPort(port);
	if (addr_info.addr(host.c_str()) < 0)
	{
		exlib::string strAddr;
		result_t hr = net_base::cc_resolve(host, m_family, strAddr);
		if (hr < 0)
			return hr;

		if (addr_info.addr(strAddr.c_str()) < 0)
			return CHECK_ERROR(CALL_E_INVALIDARG);
	}

	if (exlib::CompareAndSwap(&m_inRecv, 0, 1))
		return CHECK_ERROR(CALL_E_REENTRANT);

	return (new asyncConnect(m_fd, addr_info, ac, m_inRecv, m_RecvOpt))->call();
}

result_t AsyncIO::accept(obj_ptr<Socket_base> &retVal, AsyncEvent *ac)
{
	class asyncAccept: public asyncProc
	{
	public:
		asyncAccept(intptr_t s, obj_ptr<Socket_base> &retVal,
		            AsyncEvent *ac, intptr_t &guard, void *&opt) :
			asyncProc(s, EV_READ, ac, guard, opt), m_retVal(retVal)
		{
		}

		virtual result_t process()
		{
			inetAddr ai;
			socklen_t sz = sizeof(ai);
			intptr_t c = ::accept(m_s, (sockaddr *) &ai, &sz);
			if (c == INVALID_SOCKET)
			{
				int32_t nError = errno;
				return CHECK_ERROR((nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError);
			}

			fcntl(c, F_SETFL, fcntl(c, F_GETFL, 0) | O_NONBLOCK);
			fcntl(c, F_SETFD, FD_CLOEXEC);

#ifdef Darwin
			int32_t set_option = 1;
			setsockopt(c, SOL_SOCKET, SO_NOSIGPIPE, &set_option,
			           sizeof(set_option));
#endif
			setOption(c);

			obj_ptr<Socket> sock = new Socket(c, ai.family(),
			                                  net_base::_SOCK_STREAM);

			m_retVal = sock;

			return 0;
		}

	public:
		obj_ptr<Socket_base> &m_retVal;
	};

	if (m_fd == INVALID_SOCKET)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	if (exlib::CompareAndSwap(&m_inRecv, 0, 1))
		return CHECK_ERROR(CALL_E_REENTRANT);

	return (new asyncAccept(m_fd, retVal, ac, m_inRecv, m_RecvOpt))->call();
}

result_t AsyncIO::read(int32_t bytes, obj_ptr<Buffer_base> &retVal,
                       AsyncEvent *ac, bool bRead)
{
	class asyncRecv: public asyncProc
	{
	public:
		asyncRecv(intptr_t s, int32_t bytes, obj_ptr<Buffer_base> &retVal,
		          AsyncEvent *ac, int32_t family, bool bRead, intptr_t &guard, void *&opt) :
			asyncProc(s, EV_READ, ac, guard, opt), m_retVal(retVal), m_pos(0),
			m_bytes(bytes > 0 ? bytes : SOCKET_BUFF_SIZE), m_family(family), m_bRead(bRead)
		{
		}

		virtual result_t process()
		{
			if (m_buf.empty())
				m_buf.resize(m_bytes);

			do
			{
				int32_t n;

				if (m_family)
					n = (int32_t) ::recv(m_s, &m_buf[m_pos], m_buf.length() - m_pos,
					                     MSG_NOSIGNAL);
				else
					n = (int32_t) ::read(m_s, &m_buf[m_pos], m_buf.length() - m_pos);
				if (n == SOCKET_ERROR)
				{
					int32_t nError = errno;
					if (nError == ECONNRESET)
						n = 0;
					else
					{
						if (m_pos == 0)
							m_buf = exlib::string();

						return CHECK_ERROR((nError == EWOULDBLOCK) ?
						                   CALL_E_PENDDING : -nError);
					}
				}

				if (n == 0)
					m_bRead = false;

				m_pos += n;
				if (m_pos == 0)
					return CALL_RETURN_NULL;
			}
			while (m_bRead && m_pos < (int32_t) m_buf.length());

			m_buf.resize(m_pos);
			m_retVal = new Buffer(m_buf);

			return 0;
		}

		virtual void proc()
		{
			result_t hr = process();

			if (hr == CALL_E_PENDDING)
				post();
			else
				ready(hr);
		}

	public:
		obj_ptr<Buffer_base> &m_retVal;
		int32_t m_pos;
		int32_t m_bytes;
		int32_t m_family;
		bool m_bRead;
		exlib::string m_buf;
	};

	if (m_fd == INVALID_SOCKET)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	if (exlib::CompareAndSwap(&m_inRecv, 0, 1))
		return CHECK_ERROR(CALL_E_REENTRANT);

	return (new asyncRecv(m_fd, bytes, retVal, ac, m_family, bRead, m_inRecv, m_RecvOpt))->call();
}

result_t AsyncIO::write(Buffer_base *data, AsyncEvent *ac)
{
	class asyncSend: public asyncProc
	{
	public:
		asyncSend(intptr_t s, Buffer_base *data, AsyncEvent *ac, int32_t family, intptr_t &guard, void *&opt) :
			asyncProc(s, EV_WRITE, ac, guard, opt), m_family(family)
		{
			data->toString(m_buf);
			m_p = m_buf.c_str();
			m_sz = m_buf.length();
		}

		virtual result_t process()
		{
			while (m_sz)
			{
				int32_t n;

				if (m_family)
					n = (int32_t) ::send(m_s, m_p, m_sz, MSG_NOSIGNAL);
				else
					n = (int32_t) ::write(m_s, m_p, m_sz);
				if (n == SOCKET_ERROR)
				{
					int32_t nError = errno;
					return CHECK_ERROR((nError == EWOULDBLOCK) ? CALL_E_PENDDING : -nError);
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
				ready(hr);
		}

	public:
		exlib::string m_buf;
		const char *m_p;
		int32_t m_sz;
		int32_t m_family;
	};

	if (m_fd == INVALID_SOCKET)
		return CHECK_ERROR(CALL_E_INVALID_CALL);

	if (!ac)
		return CHECK_ERROR(CALL_E_NOSYNC);

	if (exlib::CompareAndSwap(&m_inSend, 0, 1))
		return CHECK_ERROR(CALL_E_REENTRANT);

	return (new asyncSend(m_fd, data, ac, m_family, m_inSend, m_SendOpt))->call();
}

}

#endif