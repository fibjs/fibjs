/*
 * HttpHandler.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "HttpHandler.h"
#include "HttpRequest.h"
#include "BufferedStream.h"
#include "JSHandler.h"
#include "ifs/mq.h"

namespace fibjs
{

result_t HttpHandler::invoke(object_base* v,
		obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(HttpHandler* pThis, Stream_base* stm,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_stm(stm)
		{
			m_stmBuffered = new BufferedStream(stm);
			m_stmBuffered->set_EOL("\r\n");

			m_req = new HttpRequest();
			m_req->get_response(m_rep);

			set(read);
		}

		static int read(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			bool bKeepAlive = false;

			pThis->m_rep->get_keepAlive(bKeepAlive);
			if (!bKeepAlive)
				return pThis->done(CALL_RETURN_NULL);

			pThis->set(invoke);
			return pThis->m_req->readFrom(pThis->m_stmBuffered, pThis);
		}

		static int invoke(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->set(send);
			return mq_base::invoke(pThis->m_pThis->m_hdlr, pThis->m_req, pThis);
		}

		static int send(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->set(read);
			return pThis->m_rep->sendTo(pThis->m_stm, pThis);
		}

		virtual int error(int v)
		{
			if (is(send))
			{
				m_rep->set_status(500);
				return 0;
			}

			if (is(invoke))
			{
				m_rep->set_keepAlive(false);
				m_rep->set_status(400);
				set(send);
				return 0;
			}

			return v;
		}

	private:
		obj_ptr<HttpHandler> m_pThis;
		obj_ptr<Stream_base> m_stm;
		obj_ptr<BufferedStream_base> m_stmBuffered;
		obj_ptr<HttpRequest_base> m_req;
		obj_ptr<HttpResponse_base> m_rep;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	obj_ptr<Stream_base> stm = Stream_base::getInstance(v);

	if (stm == NULL)
		return CALL_E_BADVARTYPE;

	return (new asyncInvoke(this, stm, ac))->post(0);
}

}

