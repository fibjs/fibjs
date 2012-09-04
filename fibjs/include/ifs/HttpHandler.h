/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpHandler_base_H_
#define _HttpHandler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;

class HttpHandler_base : public Handler_base
{
public:
	// HttpHandler_base
	virtual result_t get_crossDomain(bool& retVal) = 0;
	virtual result_t set_crossDomain(bool newVal) = 0;

	DECLARE_CLASSINFO(HttpHandler_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Handler_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(crossDomain, bool);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_crossDomain(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& HttpHandler_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"crossDomain", s_get_crossDomain, s_set_crossDomain}
		};

		static ClassData s_cd = 
		{ 
			"HttpHandler", NULL, 
			0, NULL, 0, NULL, 1, s_property, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpHandler_base::s_get_crossDomain(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_crossDomain(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_crossDomain(v0);

		PROPERTY_SET_LEAVE();
	}

}

#endif

