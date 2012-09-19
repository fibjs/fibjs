/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _process_base_H_
#define _process_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class BufferedStream_base;

class process_base : public module_base
{
public:
	// process_base
	static result_t get_stderr(obj_ptr<BufferedStream_base>& retVal);
	static result_t get_stdin(obj_ptr<BufferedStream_base>& retVal);
	static result_t get_stdout(obj_ptr<BufferedStream_base>& retVal);
	static result_t get_argv(v8::Handle<v8::Array>& retVal);
	static result_t get_execPath(std::string& retVal);
	static result_t exit(int32_t code);
	static result_t memoryUsage(v8::Handle<v8::Object>& retVal);
	static result_t system(const char* cmd, int32_t& retVal, exlib::AsyncEvent* ac);
	static result_t exec(const char* cmd, obj_ptr<BufferedStream_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(process_base);

public:
	static v8::Handle<v8::Value> s_get_stderr(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_stdin(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_stdout(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_argv(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_execPath(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_exit(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_memoryUsage(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_system(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_exec(const v8::Arguments& args);

public:
	ASYNC_STATICVALUE2(process_base, system, const char*, int32_t);
	ASYNC_STATICVALUE2(process_base, exec, const char*, obj_ptr<BufferedStream_base>);
};

}

#include "BufferedStream.h"

namespace fibjs
{
	inline ClassInfo& process_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"exit", s_exit, true},
			{"memoryUsage", s_memoryUsage, true},
			{"system", s_system, true},
			{"exec", s_exec, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"stderr", s_get_stderr, NULL, true},
			{"stdin", s_get_stdin, NULL, true},
			{"stdout", s_get_stdout, NULL, true},
			{"argv", s_get_argv, NULL, true},
			{"execPath", s_get_execPath, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"process", NULL, 
			4, s_method, 0, NULL, 5, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> process_base::s_get_stderr(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<BufferedStream_base> vr;

		PROPERTY_ENTER();

		hr = get_stderr(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_get_stdin(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<BufferedStream_base> vr;

		PROPERTY_ENTER();

		hr = get_stdin(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_get_stdout(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<BufferedStream_base> vr;

		PROPERTY_ENTER();

		hr = get_stdout(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_get_argv(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		v8::Handle<v8::Array> vr;

		PROPERTY_ENTER();

		hr = get_argv(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_get_execPath(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_execPath(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_exit(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = exit(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> process_base::s_memoryUsage(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = memoryUsage(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_system(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_system(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> process_base::s_exec(const v8::Arguments& args)
	{
		obj_ptr<BufferedStream_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_exec(v0, vr);

		METHOD_RETURN();
	}

}

#endif

