/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpResponse_base_H_
#define _HttpResponse_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HttpMessage.h"

namespace fibjs
{

class HttpMessage_base;

class HttpResponse_base : public HttpMessage_base
{
public:
	// HttpResponse_base
	virtual result_t get_status(int32_t& retVal) = 0;
	virtual result_t set_status(int32_t newVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = HttpMessage_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(status, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_status(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_status(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& HttpResponse_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"status", s_get_status, s_set_status}
		};

		static ClassData s_cd = 
		{ 
			"HttpResponse", NULL, 
			0, NULL, 0, NULL, 1, s_property, NULL, NULL,
			&HttpMessage_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpResponse_base::s_get_status(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpResponse_base);

		hr = pInst->get_status(vr);

		METHOD_RETURN();
	}

	inline void HttpResponse_base::s_set_status(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpResponse_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_status(v0);

		PROPERTY_SET_LEAVE();
	}

}

#endif

