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
	static result_t print(const char* fmt, const v8::Arguments& args);
	static result_t run(const char* fname);
	static result_t define(const char* id, v8::Handle<v8::Array> deps, v8::Handle<v8::Value> factory);
	static result_t require(const char* id, v8::Handle<v8::Value>& retVal);
	static result_t GC();

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_print(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_run(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_define(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_require(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_GC(const v8::Arguments& args);
};

}

#include "Buffer.h"
#include "console.h"

namespace fibjs
{
	inline ClassInfo& global_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"print", s_print},
			{"run", s_run},
			{"define", s_define},
			{"require", s_require},
			{"GC", s_GC}
		};

		static ClassObject s_object[] = 
		{
			{"Buffer", Buffer_base::class_info}
		};

		static ClassProperty s_property[] = 
		{
			{"console", s_get_console}
		};

		static ClassData s_cd = 
		{ 
			"global", NULL, 
			5, s_method, 1, s_object, 1, s_property, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> global_base::s_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<console_base> vr;

		PROPERTY_ENTER();

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

	inline v8::Handle<v8::Value> global_base::s_define(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 3);

		ARG_String(0);
		ARG(v8::Handle<v8::Array>, 1);
		ARG(v8::Handle<v8::Value>, 2);

		hr = define(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> global_base::s_require(const v8::Arguments& args)
	{
		v8::Handle<v8::Value> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

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

