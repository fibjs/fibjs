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
	static result_t _new(obj_ptr<Lock_base>& lock, obj_ptr<Condition_base>& retVal);
	virtual result_t wait() = 0;
	virtual result_t notify() = 0;
	virtual result_t notifyAll() = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_wait(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notify(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notifyAll(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& Condition_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"wait", s_wait},
			{"notify", s_notify},
			{"notifyAll", s_notifyAll}
		};

		static ClassData s_cd = 
		{ 
			"Condition", s__new, 
			3, s_method, 0, NULL, 0, NULL, NULL,
			&Lock_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> Condition_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Condition_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		METHOD_OVER(1, 1);

		ARG(obj_ptr<Lock_base>, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Condition_base::s_wait(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Condition_base);
		METHOD_ENTER(0, 0);

		hr = pInst->wait();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Condition_base::s_notify(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Condition_base);
		METHOD_ENTER(0, 0);

		hr = pInst->notify();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Condition_base::s_notifyAll(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Condition_base);
		METHOD_ENTER(0, 0);

		hr = pInst->notifyAll();

		METHOD_VOID();
	}

}

#endif

