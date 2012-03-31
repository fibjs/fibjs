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

#include "if.h"

namespace fibjs
{

class console_base;
class os_base;
class fs_base;

class global_base : public object_base
{
public:
	// global_base
	static result_t yield();
	static result_t get_console(obj_ptr<console_base>& retVal);
	static result_t get_os(obj_ptr<os_base>& retVal);
	static result_t get_fs(obj_ptr<fs_base>& retVal);
	static result_t print(const char* fmt, const v8::Arguments& args);

public:
	static ClassInfo& info()
	{
		static ClassMethod s_method[] = 
		{
			{"yield", s_yield},
			{"print", s_print}
		};

		static ClassProperty s_property[] = 
		{
			{"console", s_get_console},
			{"os", s_get_os},
			{"fs", s_get_fs}
		};

		static ClassData s_cd = 
		{ 
			"global", NULL, 
			2, s_method, 3, s_property, NULL,
			&object_base::info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual v8::Handle<v8::Value> JSObject()
	{
		return wrap(info());
	}

private:
	static v8::Handle<v8::Value> s_yield(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_os(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_fs(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_print(const v8::Arguments& args);
};

}

#include "console.h"
#include "os.h"
#include "fs.h"

namespace fibjs
{
	inline v8::Handle<v8::Value> global_base::s_yield(const v8::Arguments& args)
	{
		METHOD_ENTER(0, 0);

		hr = yield();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> global_base::s_get_console(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		obj_ptr<console_base> vr;
		hr = get_console(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> global_base::s_get_os(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		obj_ptr<os_base> vr;
		hr = get_os(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> global_base::s_get_fs(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();

		obj_ptr<fs_base> vr;
		hr = get_fs(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> global_base::s_print(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = print(v0, args);

		METHOD_VOID();
	}

}

#endif

