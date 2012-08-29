/*
 * mq.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "JSHandler.h"
#include "NullHandler.h"
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

			if (n == CALL_RETURN_NULL)
				return pThis->done(0);

			pThis->m_hdlr = pThis->m_next;
			pThis->m_next.Release();

			return pThis->m_hdlr->invoke(pThis->m_v, pThis->m_next, pThis);
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

result_t mq_base::jsHandler(v8::Handle<v8::Function> hdlr,
		obj_ptr<Handler_base>& retVal)
{
	retVal = new JSHandler(hdlr);
	return 0;
}

result_t mq_base::moduleHandler(const char* id, const char* func,
		obj_ptr<Handler_base>& retVal)
{
	return 0;
}

result_t mq_base::objectHandler(v8::Handle<v8::Object> obj,
		obj_ptr<Handler_base>& retVal)
{
	retVal = new JSHandler(obj);
	return 0;
}

result_t mq_base::nullHandler(obj_ptr<Handler_base>& retVal)
{
	retVal = new NullHandler();
	return 0;
}

}
