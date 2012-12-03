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
class Chain_base;
class Routing_base;
class Handler_base;
class AsyncWait_base;
class object_base;

class mq_base : public module_base
{
public:
	// mq_base
	static result_t chain(v8::Handle<v8::Array> hdlrs, obj_ptr<Chain_base>& retVal);
	static result_t routing(v8::Handle<v8::Object> map, obj_ptr<Routing_base>& retVal);
	static result_t jsHandler(v8::Handle<v8::Value> hdlr, obj_ptr<Handler_base>& retVal);
	static result_t moduleHandler(const char* id, const char* method, obj_ptr<Handler_base>& retVal);
	static result_t await(obj_ptr<AsyncWait_base>& retVal);
	static result_t nullHandler(obj_ptr<Handler_base>& retVal);
	static result_t invoke(Handler_base* hdlr, object_base* v, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(mq_base);

public:
	static v8::Handle<v8::Value> s_chain(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_routing(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_jsHandler(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_moduleHandler(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_await(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_nullHandler(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_invoke(const v8::Arguments& args);

public:
	ASYNC_STATIC2(mq_base, invoke, Handler_base*, object_base*);
};

}

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
			{"chain", s_chain, true},
			{"routing", s_routing, true},
			{"jsHandler", s_jsHandler, true},
			{"moduleHandler", s_moduleHandler, true},
			{"await", s_await, true},
			{"nullHandler", s_nullHandler, true},
			{"invoke", s_invoke, true}
		};

		static ClassData s_cd = 
		{ 
			"mq", NULL, 
			7, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> mq_base::s_chain(const v8::Arguments& args)
	{
		obj_ptr<Chain_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Array>, 0);

		hr = chain(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> mq_base::s_routing(const v8::Arguments& args)
	{
		obj_ptr<Routing_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = routing(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> mq_base::s_jsHandler(const v8::Arguments& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = jsHandler(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> mq_base::s_moduleHandler(const v8::Arguments& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "");

		hr = moduleHandler(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> mq_base::s_await(const v8::Arguments& args)
	{
		obj_ptr<AsyncWait_base> vr;

		METHOD_ENTER(0, 0);

		hr = await(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> mq_base::s_nullHandler(const v8::Arguments& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_ENTER(0, 0);

		hr = nullHandler(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> mq_base::s_invoke(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Handler_base>, 0);
		ARG(obj_ptr<object_base>, 1);

		hr = ac_invoke(v0, v1);

		METHOD_VOID();
	}

}

#endif

