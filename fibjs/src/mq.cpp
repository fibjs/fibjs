/*
 * mq.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "JSHandler.h"
#include "NullHandler.h"
#include "Chain.h"
#include "Routing.h"
#include "Fiber.h"
#include "ifs/mq.h"

namespace fibjs
{

result_t mq_base::invoke(Handler_base* hdlr, object_base* v,
		exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(Handler_base* hdlr, object_base* v, exlib::AsyncEvent* ac) :
				asyncState(ac), m_next(hdlr), m_v(v)
		{
			set(call);
		}

	public:
		static int call(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;
			result_t hr;

			if (n == CALL_RETURN_NULL)
				return pThis->done(0);

			pThis->m_hdlr = pThis->m_next;
			pThis->m_next.Release();

			hr = pThis->m_hdlr->invoke(pThis->m_v, pThis->m_next, pThis);
			if (hr == CALL_E_NOASYNC)
				return js_invoke(pThis->m_hdlr, pThis->m_v, pThis->m_next, pThis);

			return hr;
		}

	private:
		obj_ptr<Handler_base> m_hdlr;
		obj_ptr<Handler_base> m_next;
		obj_ptr<object_base> m_v;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncInvoke(hdlr, v, ac))->post(0);
}

result_t mq_base::js_invoke(Handler_base* hdlr, object_base* v,
		obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(Handler_base* pThis, object_base* v,
				obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_v(v), m_retVal(retVal)
		{
			set(call);
		}

	public:
		static int call(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			pThis->asyncEvent::post(0);
			return pThis->done(CALL_E_PENDDING);
		}

	public:
		virtual void js_callback()
		{
			JSFiber::scope s;

			m_hr = js_invoke(m_pThis, m_v, m_retVal, NULL);
			s_acPool.put(this);
		}

		virtual void invoke()
		{
			post(m_hr);
		}

	private:
		obj_ptr<Handler_base> m_pThis;
		obj_ptr<object_base> m_v;
		obj_ptr<Handler_base>& m_retVal;
		result_t m_hr;
	};

	if (!ac)
	{
		result_t hr;
		obj_ptr<Handler_base> hdlr1 = hdlr;
		obj_ptr<Handler_base> hdlr2;

		while (true)
		{
			hr = hdlr1->invoke(v, hdlr2, NULL);
			if (hr == CALL_E_NOSYNC)
			{
				retVal = hdlr1;
				return 0;
			}

			if (hr < 0 || hr == CALL_RETURN_NULL)
				return hr;

			hdlr1 = hdlr2;
		}

		return 0;
	}

	return (new asyncInvoke(hdlr, v, retVal, ac))->post(0);
}

result_t mq_base::chain(v8::Handle<v8::Array> hdlrs,
		obj_ptr<Chain_base>& retVal)
{
	obj_ptr<Chain_base> chain = new Chain();
	result_t hr = chain->append(hdlrs);
	if (hr < 0)
		return hr;

	retVal = chain;

	return 0;
}

result_t mq_base::routing(v8::Handle<v8::Object> map,
		obj_ptr<Routing_base>& retVal)
{
	obj_ptr<Routing_base> r = new Routing();

	result_t hr = r->append(map);
	if (hr < 0)
		return hr;

	retVal = r;
	return 0;
}

result_t mq_base::jsHandler(v8::Handle<v8::Value> hdlr,
		obj_ptr<Handler_base>& retVal)
{
	return JSHandler::New(hdlr, retVal);
}

result_t mq_base::moduleHandler(const char* id, const char* method,
		obj_ptr<Handler_base>& retVal)
{
	return JSHandler::New(id, method, retVal);
}

result_t mq_base::nullHandler(obj_ptr<Handler_base>& retVal)
{
	retVal = new NullHandler();
	return 0;
}

}
