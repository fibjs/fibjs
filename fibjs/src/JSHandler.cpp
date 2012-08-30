/*
 * JSHandler.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "JSHandler.h"
#include "Fiber.h"
#include "ifs/Message.h"

namespace fibjs
{

result_t JSHandler::setHandler(v8::Handle<v8::Value> hdlr)
{
	if (!hdlr->IsFunction() && !hdlr->IsObject())
		return CALL_E_BADVARTYPE;

	m_handler = v8::Persistent<v8::Value>::New(hdlr);
	return 0;
}

result_t JSHandler::js_invoke(object_base* v, obj_ptr<Handler_base>& retVal)
{
	JSFiber::scope s;
	v8::Handle<v8::Object> o;
	v8::Handle<v8::Value> a;
	v8::Handle<v8::Value> hdlr = m_handler;

	v->ValueOf(o);
	a = o;

	while (true)
	{
		if (hdlr->IsFunction())
			JSFiber::callFunction(v8::Handle<v8::Function>::Cast(hdlr), &a, 1,
					hdlr);
		else if (hdlr->IsObject())
		{
			if (retVal = Handler_base::getInstance(hdlr))
				return 0;

			obj_ptr<Message_base> msg = Message_base::getInstance(v);

			if (msg == NULL)
				return CALL_E_BADVARTYPE;

			std::string str;
			const char* p;

			msg->get_value(str);
			if (str.empty())
				return CALL_E_BADVARTYPE;

			o = v8::Handle<v8::Object>::Cast(hdlr);

			p = str.c_str();
			while (*p && *p != '.' && *p != '/' && *p != '\\')
				p++;

			if (p == str.c_str())
				return CALL_E_INVALID_CALL;

			hdlr = o->Get(v8::String::New(str.c_str(), (int)(p - str.c_str())));
			if (IsEmpty(hdlr))
				return CALL_E_INVALID_CALL;

			if (*p)
				p++;
			msg->set_value(p);
		}
		else
			return CALL_E_INVALID_CALL;

		if (hdlr.IsEmpty())
			return CALL_E_INTERNAL;

		if (IsEmpty(hdlr))
			return CALL_RETURN_NULL;
	}

	return 0;
}

result_t JSHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	class asyncInvoke: public asyncState
	{
	public:
		asyncInvoke(JSHandler* pThis, object_base* v,
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
			m_hr = m_pThis->js_invoke(m_v, m_retVal);
			s_acPool.put(this);
		}

		virtual void invoke()
		{
			post(m_hr);
		}

	private:
		obj_ptr<JSHandler> m_pThis;
		obj_ptr<object_base> m_v;
		obj_ptr<Handler_base>& m_retVal;
		result_t m_hr;
	};

	if (!ac)
		return CALL_E_NOSYNC;

	return (new asyncInvoke(this, v, retVal, ac))->post(0);
}

} /* namespace fibjs */
