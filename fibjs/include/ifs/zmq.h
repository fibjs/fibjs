/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _zmq_base_H_
#define _zmq_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class zmq_Socket_base;

class zmq_base : public module_base
{
public:
	enum{
		_PAIR = 0,
		_PUB = 1,
		_SUB = 2,
		_REQ = 3,
		_REP = 4,
		_DEALER = 5,
		_ROUTER = 6,
		_PULL = 7,
		_PUSH = 8,
		_XPUB = 9,
		_XSUB = 10
	};

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

protected:
	static v8::Handle<v8::Value> s_get_PAIR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_SUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_REQ(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_REP(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_DEALER(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ROUTER(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PULL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_PUSH(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_XPUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_XSUB(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

#include "zmq_Socket.h"

namespace fibjs
{
	inline ClassInfo& zmq_base::class_info()
	{
		static ClassObject s_object[] = 
		{
			{"Socket", zmq_Socket_base::class_info}
		};

		static ClassProperty s_property[] = 
		{
			{"PAIR", s_get_PAIR},
			{"PUB", s_get_PUB},
			{"SUB", s_get_SUB},
			{"REQ", s_get_REQ},
			{"REP", s_get_REP},
			{"DEALER", s_get_DEALER},
			{"ROUTER", s_get_ROUTER},
			{"PULL", s_get_PULL},
			{"PUSH", s_get_PUSH},
			{"XPUB", s_get_XPUB},
			{"XSUB", s_get_XSUB}
		};

		static ClassData s_cd = 
		{ 
			"zmq", NULL, 
			0, NULL, 1, s_object, 11, s_property, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_PAIR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PAIR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_PUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_SUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _SUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_REQ(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _REQ;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_REP(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _REP;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_DEALER(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _DEALER;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_ROUTER(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _ROUTER;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_PULL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PULL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_PUSH(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _PUSH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_XPUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _XPUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> zmq_base::s_get_XSUB(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _XSUB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

}

#endif

