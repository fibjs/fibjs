/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _global_base_H_
#define _global_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Buffer_base;
class console_base;

class global_base : public module_base
{
public:
	// global_base
	static result_t get_console(obj_ptr<console_base>& retVal);
	static result_t run(const char* fname);
	static result_t define(const char* id, v8::Local<v8::Array> deps, v8::Local<v8::Value> factory);
	static result_t require(const char* id, v8::Local<v8::Value>& retVal);
	static result_t GC();

	DECLARE_CLASSINFO(global_base);

public:
	static void s_get_console(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_define(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_require(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_GC(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "console.h"

namespace fibjs
{
	inline ClassInfo& global_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"run", s_run, true},
			{"define", s_define, true},
			{"require", s_require, true},
			{"GC", s_GC, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Buffer", Buffer_base::class_info}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"console", s_get_console, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"global", NULL, 
			4, s_method, 1, s_object, 1, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void global_base::s_get_console(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<console_base> vr;

		PROPERTY_ENTER();

		hr = get_console(vr);

		METHOD_RETURN();
	}

	inline void global_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = run(v0);

		METHOD_VOID();
	}

	inline void global_base::s_define(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 3);

		ARG(arg_string, 0);
		ARG(v8::Local<v8::Array>, 1);
		ARG(v8::Local<v8::Value>, 2);

		hr = define(v0, v1, v2);

		METHOD_VOID();
	}

	inline void global_base::s_require(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = require(v0, vr);

		METHOD_RETURN();
	}

	inline void global_base::s_GC(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(0, 0);

		hr = GC();

		METHOD_VOID();
	}

}

#endif

