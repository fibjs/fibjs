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
#include "Event.h"

namespace fibjs
{

class Event_base;

class Fiber_base : public Event_base
{
public:
	// Fiber_base
	virtual result_t join() = 0;
	virtual result_t get_func(v8::Handle<v8::Function>& retVal) = 0;

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"join", s_join}
		};

		static ClassProperty s_property[] = 
		{
			{"func", s_get_func}
		};

		static ClassData s_cd = 
		{ 
			"Fiber", NULL, 
			1, s_method, 1, s_property, NULL,
			&Event_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = Event_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(func, v8::Handle<v8::Function>);

		return 0;
	}

private:
	static v8::Handle<v8::Value> s_join(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}


namespace fibjs
{
	inline v8::Handle<v8::Value> Fiber_base::s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		v8::Handle<v8::Function> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Fiber_base);

		hr = pInst->get_func(vr);

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

