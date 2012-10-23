/*
 * JSHandler.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "JSHandler.h"
#include "Fiber.h"
#include "ifs/Message.h"
#include "ifs/global.h"
#include "ifs/mq.h"
#include "ifs/console.h"

namespace fibjs
{

result_t JSHandler::setHandler(v8::Handle<v8::Value> hdlr)
{
	if (!hdlr->IsFunction() && !hdlr->IsObject())
		return CALL_E_BADVARTYPE;

	m_handler = v8::Persistent < v8::Value > ::New(hdlr);
	return 0;
}

inline result_t msgMethod(Message_base* msg, std::string& method)
{
	std::string str;
	const char *p, *p1;

	msg->get_value(str);

	p = p1 = str.c_str();
	while (true)
	{
		while (*p && *p != '.' && *p != '/' && *p != '\\')
			p++;
		if (p != p1)
			break;
		if (!*p)
			return CALL_E_INVALID_CALL;
		p++;
		p1 = p;
	}

	msg->set_value(*p ? p + 1 : "");
	method.assign(p1, (int) (p - p1));

	return 0;
}

result_t JSHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (ac)
		return CALL_E_NOASYNC;

	v8::Handle < v8::Object > o;
	v->ValueOf(o);

	obj_ptr < Message_base > msg = Message_base::getInstance(v);
	v8::Handle < v8::Value > a = o;
	v8::Handle < v8::Value > hdlr = m_handler;
	result_t hr;
	bool bResult = false;

	if (hdlr.IsEmpty())
	{
		std::string id = m_id;

		if (isPathSlash(id[id.length() - 1]))
		{
			if (msg == NULL)
				return CALL_E_BADVARTYPE;

			std::string method;
			hr = msgMethod(msg, method);
			if (hr < 0)
				return hr;

			id.append(method);
		}

		hr = m_sbox->require(id.c_str(), hdlr);
		if (hr < 0)
			return hr;

		if (!m_method.empty())
		{
			if (!hdlr->IsObject())
				return CALL_E_INVALID_CALL;

			hdlr = v8::Handle < v8::Object
					> ::Cast(hdlr)->Get(
							v8::String::New(m_method.c_str(),
									(int) m_method.length()));
			if (IsEmpty (hdlr))
				return CALL_E_INVALID_CALL;
		}
	}

	while (true)
	{
		if (hdlr->IsFunction())
		{
			obj_ptr < List_base > params;
			int32_t len = 0, i;

			if (msg != NULL)
			{
				msg->get_params(params);
				params->get_length(len);
			}

			if (len > 0)
			{
				std::vector < v8::Handle<v8::Value> > argv;

				argv.resize(len + 1);
				argv[0] = a;

				for (i = 0; i < len; i++)
				{
					Variant v;
					params->_indexed_getter(i, v);
					argv[i + 1] = v;
				}

				JSFiber::call(v8::Handle < v8::Function > ::Cast(hdlr),
						argv.data(), len + 1, hdlr);
			}
			else
				JSFiber::call(v8::Handle < v8::Function > ::Cast(hdlr), &a, 1,
						hdlr);

			if (hdlr.IsEmpty())
				return CALL_E_INTERNAL;

			if (IsEmpty (hdlr))
				return CALL_RETURN_NULL;

			bResult = true;
		}
		else if (hdlr->IsObject())
		{
			if (retVal = Handler_base::getInstance(hdlr))
				return 0;

			if (msg == NULL)
				return CALL_E_BADVARTYPE;

			if (bResult)
			{
				msg->set_result(hdlr);
				return CALL_RETURN_NULL;
			}

			std::string method;
			hr = msgMethod(msg, method);
			if (hr < 0)
				return hr;

			hdlr = v8::Handle < v8::Object
					> ::Cast(hdlr)->Get(
							v8::String::New(method.c_str(),
									(int) method.length()));
			if (IsEmpty (hdlr))
				return CALL_E_INVALID_CALL;

			bResult = false;
		}
		else if (bResult && msg)
		{
			msg->set_result(hdlr);
			return CALL_RETURN_NULL;
		}
		else
			return CALL_E_INTERNAL;
	}

	return 0;
}

result_t JSHandler::js_invoke(Handler_base* hdlr, object_base* v,
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
			{
				JSFiber::scope s;
				s.m_hr = m_hr = js_invoke(m_pThis, m_v, m_retVal, NULL);
			}

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

} /* namespace fibjs */
