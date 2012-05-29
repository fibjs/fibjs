/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _zmq_Socket_base_H_
#define _zmq_Socket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;
class Buffer_base;

class zmq_Socket_base : public Trigger_base
{
public:
	enum{
		_PAIR = 0,
		_PUB = 1,
		_SUB = 2,
		_REQ = 3,
		_REP = 4,
		_XREQ = 5,
		_XREP = 6,
		_PULL = 7,
		_PUSH = 8,
		_XPUB = 9,
		_XSUB = 10
	};

public:
	// zmq_Socket_base
	static result_t _new(int32_t type, obj_ptr<zmq_Socket_base>& retVal);
	virtual result_t bind(const char* addr) = 0;
	virtual result_t connect(const char* addr) = 0;
	virtual result_t recv(obj_ptr<Buffer_base>& retVal) = 0;
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

protected:
	static v8::Handle<v8::Value> s_get_PAIR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_REQ(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_REP(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_XREQ(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_XREP(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PULL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PUSH(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_XPUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_XSUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_bind(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_connect(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_recv(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_send(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_close(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& zmq_Socket_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"bind", s_bind},
			{"connect", s_connect},
			{"recv", s_recv},
			{"send", s_send},
			{"close", s_close}
		};

		static ClassProperty s_property[] = 
		{
			{"PAIR", s_get_PAIR},
			{"PUB", s_get_PUB},
			{"SUB", s_get_SUB},
			{"REQ", s_get_REQ},
			{"REP", s_get_REP},
			{"XREQ", s_get_XREQ},
			{"XREP", s_get_XREP},
			{"PULL", s_get_PULL},
			{"PUSH", s_get_PUSH},
			{"XPUB", s_get_XPUB},
			{"XSUB", s_get_XSUB},
			{"type", s_get_type}
		};

		static ClassData s_cd = 
		{ 
			"zmq_Socket", s__new, 
			5, s_method, 0, NULL, 12, s_property, NULL,
			&Trigger_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_PAIR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PAIR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_PUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_SUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_REQ(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _REQ;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_REP(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _REP;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_XREQ(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _XREQ;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_XREP(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _XREP;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_PULL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PULL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_PUSH(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PUSH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_XPUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _XPUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_XSUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _XSUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(zmq_Socket_base);

		hr = pInst->get_type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<zmq_Socket_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_bind(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmq_Socket_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->bind(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_connect(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmq_Socket_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->connect(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_recv(const v8::Arguments& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(zmq_Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->recv(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_send(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmq_Socket_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->send(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> zmq_Socket_base::s_close(const v8::Arguments& args)
	{
		METHOD_INSTANCE(zmq_Socket_base);
		METHOD_ENTER(0, 0);

		hr = pInst->close();

		METHOD_VOID();
	}

}

#endif

