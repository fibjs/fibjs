/*
 * TcpServer.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "TcpServer.h"
#include "ifs/mq.h"
#include "JSHandler.h"

namespace fibjs
{

result_t TcpServer_base::_new(int32_t port, Handler_base* listener,
		obj_ptr<TcpServer_base>& retVal)
{
	return _new("", port, listener, retVal);
}

result_t TcpServer_base::_new(const char* addr, int32_t port,
		Handler_base* listener, obj_ptr<TcpServer_base>& retVal)
{
	obj_ptr < TcpServer > svr = new TcpServer();
	result_t hr = svr->create(addr, port, listener);
	if (hr < 0)
		return hr;

	retVal = svr;

	return 0;
}

result_t TcpServer_base::_new(int32_t port, v8::Handle<v8::Function> listener,
		obj_ptr<TcpServer_base>& retVal)
{
	return _new("", port, listener, retVal);
}

result_t TcpServer_base::_new(const char* addr, int32_t port,
		v8::Handle<v8::Function> listener, obj_ptr<TcpServer_base>& retVal)
{
	obj_ptr < Handler_base > hdlr1;
	result_t hr = JSHandler::New(listener, hdlr1);
	if (hr < 0)
		return hr;

	return _new(addr, port, hdlr1, retVal);
}

static const char* s_staticCounter[] =
{ "total", "connections" };
static const char* s_Counter[] =
{ "accept", "close" };

enum
{
	TCPS_TOTAL = 0, TCPS_CONNECTIONS, TCPS_ACCEPT, TCPS_CLOSE
};

TcpServer::TcpServer()
{
	m_stats = new Stats();
	m_stats->init(s_staticCounter, 2, s_Counter, 2);
	m_running = false;
}

result_t TcpServer::create(const char* addr, int32_t port,
		Handler_base* listener)
{
	result_t hr;

	hr = Socket_base::_new(net_base::_AF_INET, net_base::_SOCK_STREAM,
			m_socket);
	if (hr < 0)
		return hr;

	hr = m_socket->bind(addr, port, false);
	if (hr < 0)
		return hr;

	hr = m_socket->listen(120);
	if (hr < 0)
		return hr;

	m_hdlr = listener;

	return 0;
}

result_t TcpServer::run(exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(TcpServer* pThis, Socket_base* pSock) :
				asyncState(NULL), m_pThis(pThis), m_sock(pSock), m_obj(pSock)
		{
			set(invoke);
		}

	public:
		static int invoke(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->set(close);

			return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_obj, pThis);
		}

		static int close(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->done(0);
			pThis->m_pThis->m_stats->inc(TCPS_CLOSE);
			pThis->m_pThis->m_stats->dec(TCPS_CONNECTIONS);
			return pThis->m_sock->close(pThis);
		}

	private:
		TcpServer* m_pThis;
		obj_ptr<Socket_base> m_sock;
		obj_ptr<object_base> m_obj;
	};

	class asyncAccept: public asyncState
	{
	public:
		asyncAccept(TcpServer* pThis, exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis)
		{
			set(accept);
		}

	public:
		static int accept(asyncState* pState, int n)
		{
			asyncAccept* pThis = (asyncAccept*) pState;

			pThis->set(invoke);
			return pThis->m_pThis->m_socket->accept(pThis->m_retVal, pThis);
		}

		static int invoke(asyncState* pState, int n)
		{
			asyncAccept* pThis = (asyncAccept*) pState;

			pThis->m_pThis->m_stats->inc(TCPS_TOTAL);
			pThis->m_pThis->m_stats->inc(TCPS_ACCEPT);
			pThis->m_pThis->m_stats->inc(TCPS_CONNECTIONS);
			(new asyncInvoke(pThis->m_pThis, pThis->m_retVal))->apost(0);

			return pThis->m_pThis->m_socket->accept(pThis->m_retVal, pThis);
		}

	private:
		TcpServer* m_pThis;
		obj_ptr<Socket_base> m_retVal;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	if(m_running)
		return CALL_E_INVALID_CALL;
	m_running = true;

	return (new asyncAccept(this, ac))->post(0);
}

result_t TcpServer::asyncRun()
{
	class asyncCall: public AsyncCall
	{
	public:
		asyncCall() :
				AsyncCall(NULL)
		{
		}

		virtual int post(int v)
		{
			delete this;
			return 0;
		}
	};

	if(m_running)
		return CALL_E_INVALID_CALL;

	run(new asyncCall());
	return 0;
}

result_t TcpServer::get_socket(obj_ptr<Socket_base>& retVal)
{
	retVal = m_socket;
	return 0;
}

result_t TcpServer::get_stats(obj_ptr<Stats_base>& retVal)
{
	retVal = m_stats;
	return 0;
}

} /* namespace fibjs */
