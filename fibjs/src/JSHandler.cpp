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

	m_handler = v8::Persistent<v8::Value>::New(hdlr);
	return 0;
}

inline result_t msgMethod(object_base* v, std::string& method)
{
	obj_ptr<Message_base> msg = Message_base::getInstance(v);

	if (msg == NULL)
		return CALL_E_BADVARTYPE;

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
	if (!ac)
	{
		v8::Handle<v8::Object> o;
		v8::Handle<v8::Value> a;
		v8::Handle<v8::Value> hdlr = m_handler;
		result_t hr;

		v->ValueOf(o);
		a = o;

		if (hdlr.IsEmpty())
		{
			std::string id = m_id;

			if (isPathSlash(id[id.length() - 1]))
			{
				std::string method;
				hr = msgMethod(v, method);
				if (hr < 0)
					return hr;

				id.append(method);
			}

			hr = global_base::require(id.c_str(), hdlr);
			if (hr < 0)
				return hr;

			if (!m_method.empty())
			{
				if (!hdlr->IsObject())
					return CALL_E_INVALID_CALL;

				hdlr = v8::Handle<v8::Object>::Cast(hdlr)->Get(
						v8::String::New(m_method.c_str(),
								(int) m_method.length()));
				if (IsEmpty(hdlr))
					return CALL_E_INVALID_CALL;
			}
		}

		while (true)
		{
			if (hdlr->IsFunction())
				JSFiber::callFunction(v8::Handle<v8::Function>::Cast(hdlr), &a,
						1, hdlr);
			else if (hdlr->IsObject())
			{
				if (retVal = Handler_base::getInstance(hdlr))
					return 0;

				std::string method;
				hr = msgMethod(v, method);
				if (hr < 0)
					return hr;

				hdlr = v8::Handle<v8::Object>::Cast(hdlr)->Get(
						v8::String::New(method.c_str(), (int) method.length()));
				if (IsEmpty(hdlr))
					return CALL_E_INVALID_CALL;
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

	return CALL_E_NOASYNC;
}

} /* namespace fibjs */
