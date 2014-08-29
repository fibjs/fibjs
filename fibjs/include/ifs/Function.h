/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Function_base_H_
#define _Function_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Fiber_base;

class Function_base : public module_base
{
public:
	// Function_base
	static result_t start(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<Fiber_base>& retVal);

	DECLARE_CLASSINFO(Function_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Fiber.h"

namespace fibjs
{
	inline ClassInfo& Function_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"start", s_start}
		};

		static ClassData s_cd = 
		{ 
			"Function", NULL, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Function_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Fiber_base> vr;

		METHOD_ENTER(-1, 0);

		hr = start(args, vr);

		METHOD_RETURN();
	}

}

#endif

