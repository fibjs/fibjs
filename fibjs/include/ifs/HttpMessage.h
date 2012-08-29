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
class SeekableStream_base;

class HttpMessage_base : public Message_base
{
public:
	// HttpMessage_base
	virtual result_t get_protocol(std::string& retVal) = 0;
	virtual result_t set_protocol(const char* newVal) = 0;
	virtual result_t get_headers(obj_ptr<HttpCollection_base>& retVal) = 0;
	virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal) = 0;
	virtual result_t set_body(SeekableStream_base* newVal) = 0;
	virtual result_t get_contentType(std::string& retVal) = 0;
	virtual result_t set_contentType(const char* newVal) = 0;
	virtual result_t get_contentLength(int64_t& retVal) = 0;
	virtual result_t get_keepAlive(bool& retVal) = 0;
	virtual result_t set_keepAlive(bool newVal) = 0;

	DECLARE_CLASSINFO(HttpMessage_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Message_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(protocol);
		CLONE_CLASS(headers, HttpCollection_base);
		CLONE_CLASS(body, SeekableStream_base);
		CLONE_String(contentType);
		CLONE(contentLength, int64_t);
		CLONE(keepAlive, bool);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_protocol(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_headers(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_body(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_contentType(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_contentType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_contentLength(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_keepAlive(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_keepAlive(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
};

}

#include "HttpCollection.h"
#include "SeekableStream.h"

namespace fibjs
{
	inline ClassInfo& HttpMessage_base::class_info()
	{
		static ClassData::ClassProperty s_property[] = 
		{
			{"protocol", s_get_protocol, s_set_protocol},
			{"headers", s_get_headers},
			{"body", s_get_body, s_set_body},
			{"contentType", s_get_contentType, s_set_contentType},
			{"contentLength", s_get_contentLength},
			{"keepAlive", s_get_keepAlive, s_set_keepAlive}
		};

		static ClassData s_cd = 
		{ 
			"HttpMessage", NULL, 
			0, NULL, 0, NULL, 6, s_property, NULL, NULL,
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

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_body(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<SeekableStream_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_body(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL(obj_ptr<SeekableStream_base>);
		hr = pInst->set_body(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_contentType(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_contentType(vr);

		METHOD_RETURN();
	}

	inline void HttpMessage_base::s_set_contentType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		PROPERTY_VAL_String();
		hr = pInst->set_contentType(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> HttpMessage_base::s_get_contentLength(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpMessage_base);

		hr = pInst->get_contentLength(vr);

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

}

#endif

