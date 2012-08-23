/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Message_base_H_
#define _Message_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;
class Stream_base;
class BufferedStream_base;

class Message_base : public Trigger_base
{
public:
	// Message_base
	virtual result_t get_value(std::string& retVal) = 0;
	virtual result_t set_value(const char* newVal) = 0;
	virtual result_t clear() = 0;
	virtual result_t send(obj_ptr<Stream_base>& stm, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncSend(obj_ptr<Stream_base>& stm) = 0;
	virtual result_t onsend(v8::Handle<v8::Function> func) = 0;
	virtual result_t read(obj_ptr<BufferedStream_base>& stm, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncRead(obj_ptr<BufferedStream_base>& stm) = 0;
	virtual result_t onread(v8::Handle<v8::Function> func) = 0;

	DECLARE_CLASSINFO(Message_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Trigger_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(value);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_value(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncSend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onsend(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_read(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncRead(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onread(const v8::Arguments& args);

public:
	ASYNC_MEMBER1(Message_base, send);
	ASYNC_CALLBACK1(Message_base, send);
	ASYNC_MEMBER1(Message_base, read);
	ASYNC_CALLBACK1(Message_base, read);
};

}

#include "Stream.h"
#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& Message_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"clear", s_clear},
			{"send", s_send},
			{"asyncSend", s_asyncSend},
			{"onsend", s_onsend},
			{"read", s_read},
			{"asyncRead", s_asyncRead},
			{"onread", s_onread}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"value", s_get_value, s_set_value}
		};

		static ClassData s_cd = 
		{ 
			"Message", NULL, 
			7, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&Trigger_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Message_base::s_get_value(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Message_base);

		hr = pInst->get_value(vr);

		METHOD_RETURN();
	}

	inline void Message_base::s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Message_base);

		PROPERTY_VAL_String();
		hr = pInst->set_value(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> Message_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->ac_send(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_asyncSend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->asyncSend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_onsend(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onsend(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_read(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<BufferedStream_base>, 0);

		hr = pInst->ac_read(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_asyncRead(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<BufferedStream_base>, 0);

		hr = pInst->asyncRead(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_onread(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onread(v0);

		METHOD_VOID();
	}

}

#endif

