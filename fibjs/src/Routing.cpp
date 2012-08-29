/*
 * Routing.cpp
 *
 *  Created on: Aug 26, 2012
 *      Author: lion
 */

#include "Routing.h"
#include "JSHandler.h"
#include "ifs/Message.h"

namespace fibjs
{

result_t Routing_base::_new(obj_ptr<Routing_base>& retVal)
{
	retVal = new Routing();
	return 0;
}

result_t Routing_base::_new(v8::Handle<v8::Object> map,
		obj_ptr<Routing_base>& retVal)
{
	obj_ptr<Routing_base> r = new Routing();

	result_t hr = r->append(map);
	if (hr < 0)
		return hr;

	retVal = r;
	return 0;
}

#define RE_SIZE	32
result_t Routing::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	int i;
	int rc = 0;
	obj_ptr<Message_base> msg = Message_base::getInstance(v);
	int ovector[RE_SIZE];

	if (msg == NULL)
		return CALL_E_BADVARTYPE;

	std::string value;

	msg->get_value(value);

	for (i = (int) m_array.size() - 1; i >= 0; i--)
	{
		obj_ptr<rule>& r = m_array[i];

		if ((rc = pcre_exec(r->m_re, r->m_extra, value.c_str(), (int)value.length(),
				0, 0, ovector, RE_SIZE)) > 0)
		{
			if (rc > 1)
				msg->set_value(
						value.substr(ovector[2], ovector[3] - ovector[2]).c_str());

			retVal = r->m_hdlr;
			return 0;
		}
	}

	return CALL_E_INVALID_CALL;
}

result_t Routing::append(const char* pattern, Handler_base* hdlr)
{
	int opt = PCRE_JAVASCRIPT_COMPAT | PCRE_EXTRA | PCRE_NEWLINE_ANYCRLF
			| PCRE_UCP | PCRE_CASELESS;
	const char *error;
	int erroffset;
	pcre *re;
	pcre_extra *extra;

	re = pcre_compile(pattern, opt, &error, &erroffset, NULL);
	if (re == NULL)
	{
		char buf[1024];

		sprintf(buf, "Compilation failed at offset %d: %s.", erroffset, error);
		return Runtime::setError(buf);
	}

	extra = pcre_study(re, PCRE_STUDY_JIT_COMPILE, &error);
	if (extra == NULL)
	{
		pcre_free(re);
		return Runtime::setError(error);
	}

	obj_ptr<rule> r = new rule(re, extra, hdlr);
	m_array.append(r);

	return 0;
}

result_t Routing::append(const char* pattern, v8::Handle<v8::Function> hdlr)
{
	obj_ptr<Handler_base> hdlr1 = new JSHandler(hdlr);
	return append(pattern, hdlr1);
}

result_t Routing::append(v8::Handle<v8::Object> map)
{
	v8::Handle<v8::Array> ks = map->GetPropertyNames();
	int len = ks->Length();
	int i;

	for (i = 0; i < len; i++)
	{
		v8::Handle<v8::Value> k = ks->Get(i);
		v8::Handle<v8::Value> v = map->Get(k);

		if (v->IsFunction())
			append(*v8::String::Utf8Value(k),
					v8::Handle<v8::Function>::Cast(v));
		else if (v->IsObject())
		{
			obj_ptr<Handler_base> hdlr = Handler_base::getInstance(v);

			if (hdlr == NULL)
				return CALL_E_BADVARTYPE;

			append(*v8::String::Utf8Value(k), hdlr);
		}
		else
			return CALL_E_BADVARTYPE;
	}

	return 0;
}

} /* namespace fibjs */
