/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Function_base_H_
#define _Function_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Fiber_base;

class Function_base : public object_base
{
public:
	// Function_base
	static result_t start(const v8::Arguments& args, obj_ptr<Fiber_base>& retVal);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"start", s_start}
		};

		static ClassData s_cd = 
		{ 
			"Function", NULL, 
			1, s_method, 0, NULL, NULL,
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
	static v8::Handle<v8::Value> s_start(const v8::Arguments& args);
};

}

#include "Fiber.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> Function_base::s_start(const v8::Arguments& args)
	{
		obj_ptr<Fiber_base> vr;

		METHOD_ENTER(-1, 0);

		hr = start(args, vr);

		METHOD_RETURN();
	}

}

#endif

