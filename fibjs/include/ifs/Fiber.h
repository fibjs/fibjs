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

#include "if.h"

namespace fibjs
{

class Fiber_base : public object_base
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
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_join(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> Fiber_base::s_join(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);
		METHOD_INSTANCE(Fiber_base);

		hr = pInst->join();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Fiber_base::s_get_func(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Fiber_base);

		v8::Handle<v8::Function> vr;
		hr = pInst->get_func(vr);

		METHOD_RETURN();
	}

}

#endif

