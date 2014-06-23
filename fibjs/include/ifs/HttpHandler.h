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
class Stats_base;

class HttpHandler_base : public Handler_base
{
public:
	// HttpHandler_base
	static result_t _new(v8::Local<v8::Value> hdlr, obj_ptr<HttpHandler_base>& retVal);
	virtual result_t get_crossDomain(bool& retVal) = 0;
	virtual result_t set_crossDomain(bool newVal) = 0;
	virtual result_t get_forceGZIP(bool& retVal) = 0;
	virtual result_t set_forceGZIP(bool newVal) = 0;
	virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
	virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
	virtual result_t get_maxUploadSize(int32_t& retVal) = 0;
	virtual result_t set_maxUploadSize(int32_t newVal) = 0;
	virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
	virtual result_t set_handler(Handler_base* newVal) = 0;
	virtual result_t get_stats(obj_ptr<Stats_base>& retVal) = 0;

	DECLARE_CLASSINFO(HttpHandler_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_crossDomain(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_forceGZIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_forceGZIP(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_maxUploadSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_handler(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_handler(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

#include "Stats.h"

namespace fibjs
{
	inline ClassInfo& HttpHandler_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"crossDomain", s_get_crossDomain, s_set_crossDomain},
			{"forceGZIP", s_get_forceGZIP, s_set_forceGZIP},
			{"maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount},
			{"maxUploadSize", s_get_maxUploadSize, s_set_maxUploadSize},
			{"handler", s_get_handler, s_set_handler},
			{"stats", s_get_stats, block_set}
		};

		static ClassData s_cd = 
		{ 
			"HttpHandler", s__new, 
			0, NULL, 0, NULL, 6, s_property, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void HttpHandler_base::s_get_crossDomain(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_crossDomain(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_crossDomain(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_crossDomain(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void HttpHandler_base::s_get_forceGZIP(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_forceGZIP(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_forceGZIP(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_forceGZIP(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void HttpHandler_base::s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_maxHeadersCount(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxHeadersCount(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void HttpHandler_base::s_get_maxUploadSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_maxUploadSize(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxUploadSize(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void HttpHandler_base::s_get_handler(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Handler_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_handler(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s_set_handler(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		PROPERTY_VAL(obj_ptr<Handler_base>);
		hr = pInst->set_handler(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void HttpHandler_base::s_get_stats(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Stats_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpHandler_base);

		hr = pInst->get_stats(vr);

		METHOD_RETURN();
	}

	inline void HttpHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpHandler_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

}

#endif

