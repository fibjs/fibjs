/*
 * TCPServer.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: lion
 */

#include "TCPServer.h"

namespace fibjs
{

result_t TCPServer_base::_new(int32_t port, v8::Handle<v8::Function> listener,
		obj_ptr<TCPServer_base>& retVal)
{
	obj_ptr<TCPServer> svr = new TCPServer();
	result_t hr = svr->create("", port, listener);
	if (hr < 0)
		return hr;

	retVal = svr;

	return 0;
}

result_t TCPServer_base::_new(int32_t port, const char* addr,
		v8::Handle<v8::Function> listener, obj_ptr<TCPServer_base>& retVal)
{
	obj_ptr<TCPServer> svr = new TCPServer();
	result_t hr = svr->create(addr, port, listener);
	if (hr < 0)
		return hr;

	retVal = svr;

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

	hr = m_socket->listen(20);
	if (hr < 0)
		return hr;

	hr = m_socket->onaccept(listener);
	if (hr < 0)
		return hr;

	return 0;
}

result_t TCPServer::run(exlib::AsyncEvent* ac)
{
	class _t: public AsyncCallBack
	{
	public:
		_t(Socket_base* pThis) :
				AsyncCallBack(pThis, NULL, _stub)
		{
		}

		static void _stub(AsyncCall* ac)
		{
			_t* t = (_t*) ac;
			result_t hr = ((Socket_base*) (object_base*) t->m_pThis)->accept(
					t->retVal, t);
			if (hr != CALL_E_PENDDING)
				t->post(hr);
		}

		virtual void post(int v)
		{
			s_acPool.put(new _t((Socket_base*) (object_base*) m_pThis));
			AsyncCallBack::post(v);
		}

		virtual void callback()
		{
			_trigger("accept", retVal);
		}

	private:
		obj_ptr<Socket_base> retVal;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	s_acPool.put(new _t(m_socket));

	return CALL_E_PENDDING;
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
