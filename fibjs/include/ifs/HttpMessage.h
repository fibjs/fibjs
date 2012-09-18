/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpMessage_base_H_
#define _HttpMessage_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Message.h"

namespace fibjs
{

class Message_base;
class HttpCollection_base;

class HttpMessage_base : public Message_base
{
public:
	// HttpMessage_base
	virtual result_t get_protocol(std::string& retVal) = 0;
	virtual result_t set_protocol(const char* newVal) = 0;
	virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_keepAlive(bool& retVal) = 0;
	virtual result_t set_keepAlive(bool newVal) = 0;
	virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
	virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
	virtual result_t get_maxUploadSize(int32_t& retVal) = 0;
	virtual result_t set_maxUploadSize(int32_t newVal) = 0;
	virtual result_t hasHeader(const char* name, bool& retVal) = 0;
	virtual result_t firstHeader(const char* name, Variant& retVal) = 0;
	virtual result_t allHeader(const char* name, v8::Handle<v8::Array>& retVal) = 0;
	virtual result_t addHeader(v8::Handle<v8::Object> map) = 0;
	virtual result_t addHeader(const char* name, Variant value) = 0;
	virtual result_t setHeader(v8::Handle<v8::Object> map) = 0;
	virtual result_t setHeader(const char* name, Variant value) = 0;
	virtual result_t removeHeader(const char* name) = 0;

	DECLARE_CLASSINFO(HttpMessage_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Message_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(protocol);
		CLONE_CLASS(headers, HttpCollection_base);
		CLONE(keepAlive, bool);
		CLONE(maxHeadersCount, int32_t);
		CLONE(maxUploadSize, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_headers(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_keepAlive(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_maxUploadSize(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_hasHeader(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_firstHeader(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_allHeader(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_addHeader(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_setHeader(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_removeHeader(const v8::Arguments& args);
};

}

#include "HttpCollection.h"

namespace fibjs
{
	inline ClassInfo& HttpMessage_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"hasHeader", s_hasHeader},
			{"firstHeader", s_firstHeader},
			{"allHeader", s_allHeader},
			{"addHeader", s_addHeader},
			{"setHeader", s_setHeader},
			{"removeHeader", s_removeHeader}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"protocol", s_get_protocol, s_set_protocol},
			{"headers", s_get_headers},
			{"keepAlive", s_get_keepAlive, s_set_keepAlive},
			{"maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount},
			{"maxUploadSize", s_get_maxUploadSize, s_set_maxUploadSize}
		};

		static ClassData s_cd = 
		{ 
			"HttpMessage", NULL, 
			6, s_method, 0, NULL, 5, s_property, NULL, NULL,
			&Message_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_protocol(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL_String();
		hr = pInst->set_protocol(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_headers(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<HttpCollection_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_headers(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_keepAlive(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_keepAlive(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(bool);
		hr = pInst->set_keepAlive(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_maxHeadersCount(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_maxHeadersCount(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_maxHeadersCount(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxHeadersCount(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_maxUploadSize(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_maxUploadSize(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_maxUploadSize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_maxUploadSize(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_hasHeader(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->hasHeader(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_firstHeader(const v8::Arguments& args)
	{
		Variant vr;

		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->firstHeader(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_allHeader(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->allHeader(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_addHeader(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->addHeader(v0);

		METHOD_OVER(2, 2);

		ARG_String(0);
		ARG(Variant, 1);

		hr = pInst->addHeader(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_setHeader(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->setHeader(v0);

		METHOD_OVER(2, 2);

		ARG_String(0);
		ARG(Variant, 1);

		hr = pInst->setHeader(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_removeHeader(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpMessage_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->removeHeader(v0);

		METHOD_VOID();
	}

}

#endif

