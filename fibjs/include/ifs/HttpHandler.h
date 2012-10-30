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
	virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
	virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
	virtual result_t get_maxUploadSize(int32_t& retVal) = 0;
	virtual result_t set_maxUploadSize(int32_t newVal) = 0;

	DECLARE_CLASSINFO(HttpHandler_base);

public:
	static v8::Handle<v8::Value> s_get_crossDomain(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_maxUploadSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& HttpHandler_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"crossDomain", s_get_crossDomain, s_set_crossDomain},
			{"maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount},
			{"maxUploadSize", s_get_maxUploadSize, s_set_maxUploadSize}
		};

		static ClassData s_cd = 
		{ 
			"HttpHandler", NULL, 
			0, NULL, 0, NULL, 3, s_property, NULL, NULL,
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

	inline v8::Handle<v8::Value> HttpHandler_base::s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_maxHeadersCount(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxHeadersCount(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpHandler_base::s_get_maxUploadSize(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_maxUploadSize(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxUploadSize(v0);

		PROPERTY_SET_LEAVE();
	}

}

#endif

