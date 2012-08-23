/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpRequest_base_H_
#define _HttpRequest_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HttpMessage.h"

namespace fibjs
{

class HttpMessage_base;
class HttpCollection_base;

class HttpRequest_base : public HttpMessage_base
{
public:
	// HttpRequest_base
	static result_t _new(obj_ptr<HttpRequest_base>& retVal);
	virtual result_t get_method(std::string& retVal) = 0;
	virtual result_t set_method(const char* newVal) = 0;
	virtual result_t get_address(std::string& retVal) = 0;
	virtual result_t set_address(const char* newVal) = 0;
	virtual result_t get_cookies(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_form(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_query(obj_ptr<HttpCollection_base>& retVal) = 0;

	DECLARE_CLASSINFO(HttpRequest_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = HttpMessage_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(method);
		CLONE_String(address);
		CLONE_CLASS(cookies, HttpCollection_base);
		CLONE_CLASS(form, HttpCollection_base);
		CLONE_CLASS(query, HttpCollection_base);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_method(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_address(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_address(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_cookies(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_form(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_query(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

#include "HttpCollection.h"

namespace fibjs
{
	inline ClassInfo& HttpRequest_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"method", s_get_method, s_set_method},
			{"address", s_get_address, s_set_address},
			{"cookies", s_get_cookies},
			{"form", s_get_form},
			{"query", s_get_query}
		};

		static ClassData s_cd = 
		{ 
			"HttpRequest", s__new, 
			0, NULL, 0, NULL, 5, s_property, NULL, NULL,
			&HttpMessage_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_method(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_method(vr);

		METHOD_RETURN();
	}

	inline void HttpRequest_base::s_set_method(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		PROPERTY_VAL_String();
		hr = pInst->set_method(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_address(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_address(vr);

		METHOD_RETURN();
	}

	inline void HttpRequest_base::s_set_address(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		PROPERTY_VAL_String();
		hr = pInst->set_address(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_cookies(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_cookies(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_form(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_form(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s_get_query(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpRequest_base);

		hr = pInst->get_query(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpRequest_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<HttpRequest_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

}

#endif

