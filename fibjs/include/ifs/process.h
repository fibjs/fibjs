/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _process_base_H_
#define _process_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class BufferedStream_base;

class process_base : public object_base
{
	DECLARE_CLASS(process_base);

public:
	// process_base
	static result_t get_argv(v8::Local<v8::Array>& retVal);
	static result_t get_execPath(std::string& retVal);
	static result_t exit(int32_t code);
	static result_t cwd(std::string& retVal);
	static result_t chdir(const char* directory);
	static result_t memoryUsage(v8::Local<v8::Object>& retVal);
	static result_t system(const char* cmd, int32_t& retVal, AsyncEvent* ac);
	static result_t popen(const char* cmd, obj_ptr<BufferedStream_base>& retVal, AsyncEvent* ac);
	static result_t exec(const char* cmd);

public:
	static void s_get_argv(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_execPath(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_exit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_cwd(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_chdir(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_system(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_popen(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exec(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE2(process_base, system, const char*, int32_t);
	ASYNC_STATICVALUE2(process_base, popen, const char*, obj_ptr<BufferedStream_base>);
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
			{"cwd", s_cwd, true},
			{"chdir", s_chdir, true},
			{"memoryUsage", s_memoryUsage, true},
			{"system", s_system, true},
			{"popen", s_popen, true},
			{"exec", s_exec, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"argv", s_get_argv, block_set, true},
			{"execPath", s_get_execPath, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"process", NULL, 
			7, s_method, 0, NULL, 2, s_property, NULL, NULL,
			NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void process_base::s_get_argv(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		v8::Local<v8::Array> vr;

		PROPERTY_ENTER();

		hr = get_argv(vr);

		METHOD_RETURN();
	}

	inline void process_base::s_get_execPath(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_execPath(vr);

		METHOD_RETURN();
	}

	inline void process_base::s_exit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = exit(v0);

		METHOD_VOID();
	}

	inline void process_base::s_cwd(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = cwd(vr);

		METHOD_RETURN();
	}

	inline void process_base::s_chdir(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = chdir(v0);

		METHOD_VOID();
	}

	inline void process_base::s_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = memoryUsage(vr);

		METHOD_RETURN();
	}

	inline void process_base::s_system(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = ac_system(v0, vr);

		METHOD_RETURN();
	}

	inline void process_base::s_popen(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<BufferedStream_base> vr;

		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = ac_popen(v0, vr);

		METHOD_RETURN();
	}

	inline void process_base::s_exec(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = exec(v0);

		METHOD_VOID();
	}

}

#endif

