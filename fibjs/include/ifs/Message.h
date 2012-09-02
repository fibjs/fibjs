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
class List_base;
class Stream_base;
class BufferedStream_base;

class Message_base : public Trigger_base
{
public:
	// Message_base
	virtual result_t get_value(std::string& retVal) = 0;
	virtual result_t set_value(const char* newVal) = 0;
	virtual result_t get_params(obj_ptr<List_base>& retVal) = 0;
	virtual result_t set_params(List_base* newVal) = 0;
	virtual result_t get_result(Variant& retVal) = 0;
	virtual result_t set_result(Variant newVal) = 0;
	virtual result_t clear() = 0;
	virtual result_t sendTo(Stream_base* stm, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncSendTo(Stream_base* stm) = 0;
	virtual result_t onsendto(v8::Handle<v8::Function> func) = 0;
	virtual result_t readFrom(BufferedStream_base* stm, exlib::AsyncEvent* ac) = 0;
	virtual result_t asyncReadFrom(BufferedStream_base* stm) = 0;
	virtual result_t onreadfrom(v8::Handle<v8::Function> func) = 0;

	DECLARE_CLASSINFO(Message_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Trigger_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE_String(value);
		CLONE_CLASS(params, List_base);
		CLONE(result, Variant);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_get_value(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_params(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_params(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_result(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_result(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sendTo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncSendTo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onsendto(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readFrom(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_asyncReadFrom(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onreadfrom(const v8::Arguments& args);

public:
	ASYNC_MEMBER1(Message_base, sendTo);
	ASYNC_CALLBACK1(Message_base, sendTo);
	ASYNC_MEMBER1(Message_base, readFrom);
	ASYNC_CALLBACK1(Message_base, readFrom);
};

}

#include "List.h"
#include "Stream.h"
#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& Message_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"clear", s_clear},
			{"sendTo", s_sendTo},
			{"asyncSendTo", s_asyncSendTo},
			{"onsendto", s_onsendto},
			{"readFrom", s_readFrom},
			{"asyncReadFrom", s_asyncReadFrom},
			{"onreadfrom", s_onreadfrom}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"value", s_get_value, s_set_value},
			{"params", s_get_params, s_set_params},
			{"result", s_get_result, s_set_result}
		};

		static ClassData s_cd = 
		{ 
			"Message", NULL, 
			7, s_method, 0, NULL, 3, s_property, NULL, NULL,
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

	inline v8::Handle<v8::Value> Message_base::s_get_params(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<List_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Message_base);

		hr = pInst->get_params(vr);

		METHOD_RETURN();
	}

	inline void Message_base::s_set_params(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Message_base);

		PROPERTY_VAL(obj_ptr<List_base>);
		hr = pInst->set_params(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> Message_base::s_get_result(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		Variant vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Message_base);

		hr = pInst->get_result(vr);

		METHOD_RETURN();
	}

	inline void Message_base::s_set_result(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Message_base);

		PROPERTY_VAL(Variant);
		hr = pInst->set_result(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> Message_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_sendTo(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->ac_sendTo(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_asyncSendTo(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Stream_base>, 0);

		hr = pInst->asyncSendTo(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_onsendto(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onsendto(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_readFrom(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<BufferedStream_base>, 0);

		hr = pInst->ac_readFrom(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_asyncReadFrom(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<BufferedStream_base>, 0);

		hr = pInst->asyncReadFrom(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Message_base::s_onreadfrom(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Message_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onreadfrom(v0);

		METHOD_VOID();
	}

}

#endif

