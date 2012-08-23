/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Fiber_base_H_
#define _Fiber_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;

class Fiber_base : public Trigger_base
{
public:
	// Fiber_base
	virtual result_t join() = 0;
	virtual result_t get_func(v8::Handle<v8::Function>& retVal) = 0;
	virtual result_t get_caller(obj_ptr<Fiber_base>& retVal) = 0;
	virtual result_t onerror(v8::Handle<v8::Function> trigger) = 0;
	virtual result_t onexit(v8::Handle<v8::Function> trigger) = 0;

	DECLARE_CLASSINFO(Fiber_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Trigger_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(func, v8::Handle<v8::Function>);
		CLONE_CLASS(caller, Fiber_base);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_join(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_caller(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_onerror(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_onexit(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& Fiber_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"join", s_join},
			{"onerror", s_onerror},
			{"onexit", s_onexit}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"func", s_get_func},
			{"caller", s_get_caller}
		};

		static ClassData s_cd = 
		{ 
			"Fiber", NULL, 
			3, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&Trigger_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Fiber_base::s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		v8::Handle<v8::Function> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Fiber_base);

		hr = pInst->get_func(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Fiber_base::s_get_caller(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Fiber_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Fiber_base);

		hr = pInst->get_caller(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Fiber_base::s_join(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Fiber_base);
		METHOD_ENTER(0, 0);

		hr = pInst->join();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Fiber_base::s_onerror(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Fiber_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onerror(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Fiber_base::s_onexit(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Fiber_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = pInst->onexit(v0);

		METHOD_VOID();
	}

}

#endif

