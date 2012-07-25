/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _zmqSocket_base_H_
#define _zmqSocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;
class zmq_base;
class Buffer_base;

class zmqSocket_base : public Trigger_base
{
public:
	// zmqSocket_base
	static result_t _new(int32_t type, obj_ptr<zmqSocket_base>& retVal);
	virtual result_t bind(const char* addr) = 0;
	virtual result_t connect(const char* addr) = 0;
	virtual result_t recv(obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t send(obj_ptr<Buffer_base>& data) = 0;
	virtual result_t close() = 0;
	virtual result_t get_type(int32_t& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Trigger_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(type, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_bind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_connect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);

public:
	ASYNC_MEMBER1(zmqSocket_base, recv);
	ASYNC_VALUEBACK0(zmqSocket_base, recv, obj_ptr<Buffer_base>);
};

}

#include "zmq.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& zmqSocket_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"bind", s_bind},
			{"connect", s_connect},
			{"recv", s_recv},
			{"send", s_send},
			{"close", s_close}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"type", s_get_type}
		};

		static ClassData s_cd = 
		{ 
			"zmqSocket", s__new, 
			5, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&Trigger_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(zmqSocket_base);

		hr = pInst->get_type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<zmqSocket_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(int32_t, 0, zmq_base::_PAIR);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s_bind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmqSocket_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->bind(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s_connect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmqSocket_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->connect(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s_recv(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(zmqSocket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_recv(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmqSocket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->send(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> zmqSocket_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmqSocket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->close();

		METHOD_VOID();
	}

}

#endif

