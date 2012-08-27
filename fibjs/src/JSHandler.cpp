/*
 * JSHandler.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "JSHandler.h"
#include "Fiber.h"

namespace fibjs
{

result_t JSHandler::callFunction(obj_ptr<object_base>& v,
		obj_ptr<Handler_base>& retVal)
{
	v8::Handle<v8::Object> o;
	v8::Handle<v8::Value> a, r;
	v8::Handle<v8::Function> func;

	v->ValueOf(o);
	a = o;
	func = m_func;

	while (true)
	{
		JSFiber::callFunction(func, &a, 1, r);

		if (r.IsEmpty())
			return CALL_E_INTERNAL;

		if (IsEmpty(r))
			return CALL_RETURN_NULL;

		if (!r->IsFunction())
		{
			retVal = Handler_base::getInstance(r);
			return retVal != NULL ? 0 : CALL_E_RETURN_TYPE;
		}

		func = v8::Handle<v8::Function>::Cast(r);
	}

	return 0;
}

result_t JSHandler::invoke(obj_ptr<object_base>& v,
		obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(JSHandler* pThis, obj_ptr<object_base>& v,
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
			m_hr = m_pThis->callFunction(m_v, m_retVal);
			s_acPool.put(this);
		}

		virtual void invoke()
		{
			post(m_hr);
		}

	private:
		JSHandler* m_pThis;
		obj_ptr<object_base>& m_v;
		obj_ptr<Handler_base>& m_retVal;
		result_t m_hr;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncInvoke(this, v, retVal, ac))->post(0);
}

} /* namespace fibjs */
