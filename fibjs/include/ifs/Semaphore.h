/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Semaphore_base_H_
#define _Semaphore_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Lock.h"

namespace fibjs
{

class Lock_base;

class Semaphore_base : public Lock_base
{
public:
	// Semaphore_base
	static result_t _new(int32_t value, obj_ptr<Semaphore_base>& retVal);
	virtual result_t wait() = 0;
	virtual result_t post() = 0;
	virtual result_t trywait(bool& retVal) = 0;

	DECLARE_CLASSINFO(Semaphore_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_trywait(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& Semaphore_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"wait", s_wait},
			{"post", s_post},
			{"trywait", s_trywait}
		};

		static ClassData s_cd = 
		{ 
			"Semaphore", s__new, 
			3, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Lock_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Semaphore_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Semaphore_base> vr;

		CONSTRUCT_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 1);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline void Semaphore_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Semaphore_base);
		METHOD_ENTER(0, 0);

		hr = pInst->wait();

		METHOD_VOID();
	}

	inline void Semaphore_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Semaphore_base);
		METHOD_ENTER(0, 0);

		hr = pInst->post();

		METHOD_VOID();
	}

	inline void Semaphore_base::s_trywait(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Semaphore_base);
		METHOD_ENTER(0, 0);

		hr = pInst->trywait(vr);

		METHOD_RETURN();
	}

}

#endif

