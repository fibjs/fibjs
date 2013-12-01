/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Condition_base_H_
#define _Condition_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Lock.h"

namespace fibjs
{

class Lock_base;

class Condition_base : public Lock_base
{
public:
	// Condition_base
	static result_t _new(obj_ptr<Condition_base>& retVal);
	static result_t _new(Lock_base* lock, obj_ptr<Condition_base>& retVal);
	virtual result_t wait() = 0;
	virtual result_t notify() = 0;
	virtual result_t notifyAll() = 0;

	DECLARE_CLASSINFO(Condition_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notify(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notifyAll(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& Condition_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"wait", s_wait},
			{"notify", s_notify},
			{"notifyAll", s_notifyAll}
		};

		static ClassData s_cd = 
		{ 
			"Condition", s__new, 
			3, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Lock_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Condition_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Condition_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		METHOD_OVER(1, 1);

		ARG(obj_ptr<Lock_base>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline void Condition_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Condition_base);
		METHOD_ENTER(0, 0);

		hr = pInst->wait();

		METHOD_VOID();
	}

	inline void Condition_base::s_notify(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Condition_base);
		METHOD_ENTER(0, 0);

		hr = pInst->notify();

		METHOD_VOID();
	}

	inline void Condition_base::s_notifyAll(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Condition_base);
		METHOD_ENTER(0, 0);

		hr = pInst->notifyAll();

		METHOD_VOID();
	}

}

#endif

