/*
 * TCPServer.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "TCPServer.h"
#include "ifs/mq.h"
#include "JSHandler.h"

namespace fibjs
{

result_t TCPServer_base::_new(int32_t port, Handler_base* listener,
		obj_ptr<TCPServer_base>& retVal)
{
	return _new("", port, listener, retVal);
}

result_t TCPServer_base::_new(const char* addr, int32_t port,
		Handler_base* listener, obj_ptr<TCPServer_base>& retVal)
{
	obj_ptr<TCPServer> svr = new TCPServer();
	result_t hr = svr->create(addr, port, listener);
	if (hr < 0)
		return hr;

	retVal = svr;

	return 0;
}

result_t TCPServer_base::_new(int32_t port, v8::Handle<v8::Function> listener,
		obj_ptr<TCPServer_base>& retVal)
{
	return _new("", port, listener, retVal);
}

result_t TCPServer_base::_new(const char* addr, int32_t port,
		v8::Handle<v8::Function> listener, obj_ptr<TCPServer_base>& retVal)
{
	obj_ptr<Handler_base> hdlr = new JSHandler(listener);
	return _new("", port, hdlr, retVal);
}

result_t TCPServer::create(const char* addr, int32_t port,
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

result_t TCPServer::create(const char* addr, int32_t port,
		v8::Handle<v8::Function> listener)
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

	hr = m_socket->onaccept(listener);
	if (hr < 0)
		return hr;

	return 0;
}

result_t TCPServer::run(exlib::AsyncEvent* ac)
{
	class asyncAccept: public asyncState
	{
	public:
		asyncAccept(TCPServer* pThis) :
				asyncState(NULL), m_pThis(pThis)
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

			pThis->set(close);

			(new asyncAccept(pThis->m_pThis))->post(0);

			pThis->m_v = pThis->m_retVal;
			return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_v, pThis);
		}

		static int close(asyncState* pState, int n)
		{
			asyncAccept* pThis = (asyncAccept*) pState;

			pThis->done(0);
			return pThis->m_retVal->close(pThis);
		}

	private:
		TCPServer* m_pThis;
		obj_ptr<Socket_base> m_retVal;
		obj_ptr<object_base> m_v;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncAccept(this))->post(0);
}

result_t TCPServer::asyncRun()
{
	acb_run();
	return 0;
}

result_t TCPServer::get_socket(obj_ptr<Socket_base>& retVal)
{
	retVal = m_socket;
	return 0;
}

} /* namespace fibjs */
