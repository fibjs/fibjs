/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _os_base_H_
#define _os_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class os_base : public object_base
{
	DECLARE_CLASS(os_base);

public:
	// os_base
	static result_t get_hostname(std::string& retVal);
	static result_t get_type(std::string& retVal);
	static result_t get_version(std::string& retVal);
	static result_t get_arch(std::string& retVal);
	static result_t get_timezone(int32_t& retVal);
	static result_t uptime(double& retVal);
	static result_t loadavg(v8::Local<v8::Array>& retVal);
	static result_t totalmem(int64_t& retVal);
	static result_t freemem(int64_t& retVal);
	static result_t CPUInfo(v8::Local<v8::Array>& retVal);
	static result_t CPUs(int32_t& retVal);
	static result_t networkInfo(v8::Local<v8::Object>& retVal);
	static result_t time(const char* tmString, date_t& retVal);
	static result_t dateAdd(date_t d, int32_t num, const char* part, date_t& retVal);
	static result_t get_execPath(std::string& retVal);
	static result_t memoryUsage(v8::Local<v8::Object>& retVal);

public:
	static void s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_arch(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_timezone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_uptime(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_freemem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_CPUInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_CPUs(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_networkInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_time(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_execPath(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
	inline ClassInfo& os_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"uptime", s_uptime, true},
			{"loadavg", s_loadavg, true},
			{"totalmem", s_totalmem, true},
			{"freemem", s_freemem, true},
			{"CPUInfo", s_CPUInfo, true},
			{"CPUs", s_CPUs, true},
			{"networkInfo", s_networkInfo, true},
			{"time", s_time, true},
			{"dateAdd", s_dateAdd, true},
			{"memoryUsage", s_memoryUsage, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"hostname", s_get_hostname, block_set, true},
			{"type", s_get_type, block_set, true},
			{"version", s_get_version, block_set, true},
			{"arch", s_get_arch, block_set, true},
			{"timezone", s_get_timezone, block_set, true},
			{"execPath", s_get_execPath, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"os", NULL, 
			10, s_method, 0, NULL, 6, s_property, NULL, NULL,
			NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void os_base::s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_hostname(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_type(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_get_version(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_version(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_get_arch(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_arch(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_get_timezone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_timezone(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_get_execPath(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_execPath(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_uptime(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		double vr;

		METHOD_ENTER(0, 0);

		hr = uptime(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_loadavg(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = loadavg(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_totalmem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_ENTER(0, 0);

		hr = totalmem(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_freemem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int64_t vr;

		METHOD_ENTER(0, 0);

		hr = freemem(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_CPUInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = CPUInfo(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_CPUs(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_ENTER(0, 0);

		hr = CPUs(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_networkInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = networkInfo(vr);

		METHOD_RETURN();
	}

	inline void os_base::s_time(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		date_t vr;

		METHOD_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "");

		hr = time(v0, vr);

		METHOD_RETURN();
	}

	inline void os_base::s_dateAdd(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		date_t vr;

		METHOD_ENTER(3, 3);

		ARG(date_t, 0);
		ARG(int32_t, 1);
		ARG(arg_string, 2);

		hr = dateAdd(v0, v1, v2, vr);

		METHOD_RETURN();
	}

	inline void os_base::s_memoryUsage(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = memoryUsage(vr);

		METHOD_RETURN();
	}

}

#endif

