/*
 * Chain.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "Chain.h"
#include "JSHandler.h"
#include "ifs/mq.h"

namespace fibjs
{

result_t Chain_base::_new(obj_ptr<Chain_base>& retVal)
{
	retVal = new Chain();
	return 0;
}

result_t Chain_base::_new(v8::Handle<v8::Array> hdlrs,
		obj_ptr<Chain_base>& retVal)
{
	obj_ptr<Chain_base> chain = new Chain();
	result_t hr = chain->append(hdlrs);
	if (hr < 0)
		return hr;

	retVal = chain;

	return 0;
}

result_t Chain::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(Chain* pThis, object_base* v,
				exlib::AsyncEvent* ac) :
				asyncState(ac), m_pThis(pThis), m_v(v), m_pos(0)
		{
			set(invoke);
		}

	public:
		static int invoke(asyncState* pState, int n)
		{
			asyncInvoke* pThis = (asyncInvoke*) pState;

			if (pThis->m_pos == (int32_t) pThis->m_pThis->m_array.size())
				return pThis->done(CALL_RETURN_NULL);

			pThis->m_pos++;
			return mq_base::invoke(pThis->m_pThis->m_array[pThis->m_pos - 1],
					pThis->m_v, pThis);
		}

	private:
		obj_ptr<Chain> m_pThis;
		obj_ptr<object_base> m_v;
		int32_t m_pos;
	};

	if(m_array.size() == 0)
		return CALL_E_INVALID_CALL;

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncInvoke(this, v, ac))->post(0);
}

result_t Chain::append(Handler_base* hdlr)
{
	m_array.append(hdlr);
	return 0;
}

result_t Chain::append(v8::Handle<v8::Function> hdlr)
{
	obj_ptr<Handler_base> hdlr1 = new JSHandler(hdlr);
	return append(hdlr1);
}

result_t Chain::append(v8::Handle<v8::Array> hdlrs)
{
	int32_t len = hdlrs->Length();
	int32_t i;

	for (i = 0; i < len; i++)
	{
		v8::Handle<v8::Value> v = hdlrs->Get(i);

		if (v->IsFunction())
			append(v8::Handle<v8::Function>::Cast(v));
		else if (v->IsObject())
		{
			obj_ptr<Handler_base> hdlr = Handler_base::getInstance(v);

			if (hdlr == NULL)
				return CALL_E_BADVARTYPE;

			append(hdlr);
		}
		else
			return CALL_E_BADVARTYPE;
	}

	return 0;
}

} /* namespace fibjs */
