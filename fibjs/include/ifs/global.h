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

namespace fibjs
{

class console_base;

class global_base : public object_base
{
public:
	// global_base
	static result_t get_console(obj_ptr<console_base>& retVal);
	static result_t print(const char* fmt, const v8::Arguments& args);
	static result_t run(const char* fname);
	static result_t sleep(int32_t ms);
	static result_t require(const char* mod, v8::Handle<v8::Value>& retVal);
	static result_t GC();

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"print", s_print},
			{"run", s_run},
			{"sleep", s_sleep},
			{"require", s_require},
			{"GC", s_GC}
		};

		static ClassProperty s_property[] = 
		{
			{"console", s_get_console}
		};

		static ClassData s_cd = 
		{ 
			"global", NULL, 
			5, s_method, 1, s_property, NULL,
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
	static v8::Handle<v8::Value> s_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_print(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_run(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_sleep(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_require(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_GC(const v8::Arguments& args);
};

}

#include "console.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> global_base::s_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		obj_ptr<console_base> vr;
		hr = get_console(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> global_base::s_print(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = print(v0, args);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> global_base::s_run(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = run(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> global_base::s_sleep(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 0);

		hr = sleep(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> global_base::s_require(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		v8::Handle<v8::Value> vr;
		hr = require(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> global_base::s_GC(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		hr = GC();

		METHOD_VOID();
	}

}

#endif

