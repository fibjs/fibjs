/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _mq_base_H_
#define _mq_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Message_base;
class PacketMessage_base;
class PacketHandler_base;
class HttpHandler_base;
class Chain_base;
class Routing_base;
class Handler_base;
class AsyncWait_base;
class object_base;

class mq_base : public module_base
{
public:
	// mq_base
	static result_t httpHandler(v8::Local<v8::Value> hdlr, obj_ptr<HttpHandler_base>& retVal);
	static result_t chain(v8::Local<v8::Array> hdlrs, obj_ptr<Chain_base>& retVal);
	static result_t routing(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal);
	static result_t jsHandler(v8::Local<v8::Value> hdlr, obj_ptr<Handler_base>& retVal);
	static result_t await(obj_ptr<AsyncWait_base>& retVal);
	static result_t nullHandler(obj_ptr<Handler_base>& retVal);
	static result_t invoke(Handler_base* hdlr, object_base* v, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(mq_base);

public:
	static void s_httpHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_chain(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_routing(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_jsHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_await(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_nullHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATIC2(mq_base, invoke, Handler_base*, object_base*);
};

}

#include "Message.h"
#include "PacketMessage.h"
#include "PacketHandler.h"
#include "HttpHandler.h"
#include "Chain.h"
#include "Routing.h"
#include "Handler.h"
#include "AsyncWait.h"

namespace fibjs
{
	inline ClassInfo& mq_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"httpHandler", s_httpHandler, true},
			{"chain", s_chain, true},
			{"routing", s_routing, true},
			{"jsHandler", s_jsHandler, true},
			{"await", s_await, true},
			{"nullHandler", s_nullHandler, true},
			{"invoke", s_invoke, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Message", Message_base::class_info},
			{"PacketMessage", PacketMessage_base::class_info},
			{"PacketHandler", PacketHandler_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"mq", NULL, 
			7, s_method, 3, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void mq_base::s_httpHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<HttpHandler_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = httpHandler(v0, vr);

		METHOD_RETURN();
	}

	inline void mq_base::s_chain(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Chain_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = chain(v0, vr);

		METHOD_RETURN();
	}

	inline void mq_base::s_routing(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Routing_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = routing(v0, vr);

		METHOD_RETURN();
	}

	inline void mq_base::s_jsHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = jsHandler(v0, vr);

		METHOD_RETURN();
	}

	inline void mq_base::s_await(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<AsyncWait_base> vr;

		METHOD_ENTER(0, 0);

		hr = await(vr);

		METHOD_RETURN();
	}

	inline void mq_base::s_nullHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(0, 0);

		hr = nullHandler(vr);

		METHOD_RETURN();
	}

	inline void mq_base::s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Handler_base>, 0);
		ARG(obj_ptr<object_base>, 1);

		hr = ac_invoke(v0, v1);

		METHOD_VOID();
	}

}

#endif

