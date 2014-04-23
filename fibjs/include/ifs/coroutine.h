/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _coroutine_base_H_
#define _coroutine_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Lock_base;
class Semaphore_base;
class Condition_base;
class Event_base;
class Trigger_base;
class BlockQueue_base;
class Fiber_base;

class coroutine_base : public module_base
{
public:
	// coroutine_base
	static result_t start(v8::Local<v8::Function> func, const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<Fiber_base>& retVal);
	static result_t parallel(v8::Local<v8::Array> func, v8::Local<v8::Array>& retVal);
	static result_t parallel(v8::Local<v8::Array> data, v8::Local<v8::Function> func, v8::Local<v8::Array>& retVal);
	static result_t parallel(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Array>& retVal);
	static result_t current(obj_ptr<Fiber_base>& retVal);
	static result_t sleep(int32_t ms, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(coroutine_base);

public:
	static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_parallel(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_current(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sleep(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATIC1(coroutine_base, sleep, int32_t);
};

}

#include "Lock.h"
#include "Semaphore.h"
#include "Condition.h"
#include "Event.h"
#include "Trigger.h"
#include "BlockQueue.h"
#include "Fiber.h"

namespace fibjs
{
	inline ClassInfo& coroutine_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"start", s_start, true},
			{"parallel", s_parallel, true},
			{"current", s_current, true},
			{"sleep", s_sleep, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Lock", Lock_base::class_info},
			{"Semaphore", Semaphore_base::class_info},
			{"Condition", Condition_base::class_info},
			{"Event", Event_base::class_info},
			{"Trigger", Trigger_base::class_info},
			{"BlockQueue", BlockQueue_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"coroutine", NULL, 
			4, s_method, 6, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void coroutine_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Fiber_base> vr;

		METHOD_ENTER(-1, 1);

		ARG(v8::Local<v8::Function>, 0);

		hr = start(v0, args, vr);

		METHOD_RETURN();
	}

	inline void coroutine_base::s_parallel(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = parallel(v0, vr);

		METHOD_OVER(2, 2);

		ARG(v8::Local<v8::Array>, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = parallel(v0, v1, vr);

		METHOD_OVER(-1, 0);

		hr = parallel(args, vr);

		METHOD_RETURN();
	}

	inline void coroutine_base::s_current(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Fiber_base> vr;

		METHOD_ENTER(0, 0);

		hr = current(vr);

		METHOD_RETURN();
	}

	inline void coroutine_base::s_sleep(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 0);

		hr = ac_sleep(v0);

		METHOD_VOID();
	}

}

#endif

