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
#include "EventObject.h"

namespace fibjs
{

class EventObject_base;

class Fiber_base : public EventObject_base
{
public:
	// Fiber_base
	virtual result_t join() = 0;
	virtual result_t get_func(v8::Handle<v8::Function>& retVal) = 0;
	virtual result_t get_caller(obj_ptr<Fiber_base>& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = EventObject_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(func, v8::Handle<v8::Function>);
		CLONE_CLASS(caller, Fiber_base);

		return 0;
	}

private:
	static v8::Handle<v8::Value> s_join(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_caller(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline ClassInfo& Fiber_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"join", s_join}
		};

		static ClassProperty s_property[] = 
		{
			{"func", s_get_func},
			{"caller", s_get_caller}
		};

		static ClassData s_cd = 
		{ 
			"Fiber", NULL, 
			1, s_method, 0, NULL, 2, s_property, NULL,
			&EventObject_base::class_info()
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

}

#endif

