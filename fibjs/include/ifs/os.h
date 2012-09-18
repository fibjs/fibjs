/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _os_base_H_
#define _os_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Stat_base;
class List_base;
class BufferedStream_base;

class os_base : public module_base
{
public:
	// os_base
	static result_t get_shell(std::string& retVal);
	static result_t get_hostname(std::string& retVal);
	static result_t get_type(std::string& retVal);
	static result_t get_version(std::string& retVal);
	static result_t get_arch(std::string& retVal);
	static result_t get_timezone(int32_t& retVal);
	static result_t uptime(double& retVal);
	static result_t loadavg(v8::Handle<v8::Array>& retVal);
	static result_t totalmem(int64_t& retVal);
	static result_t freemem(int64_t& retVal);
	static result_t CPUInfo(v8::Handle<v8::Array>& retVal);
	static result_t CPUs(int32_t& retVal);
	static result_t networkInfo(v8::Handle<v8::Object>& retVal);
	static result_t time(const char* tmString, date_t& retVal);
	static result_t exists(const char* path, bool& retVal, exlib::AsyncEvent* ac);
	static result_t unlink(const char* path, exlib::AsyncEvent* ac);
	static result_t mkdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rmdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rename(const char* from, const char* to, exlib::AsyncEvent* ac);
	static result_t stat(const char* path, obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readdir(const char* path, obj_ptr<List_base>& retVal, exlib::AsyncEvent* ac);
	static result_t exit(int32_t code);
	static result_t system(const char* cmd, int32_t& retVal, exlib::AsyncEvent* ac);
	static result_t exec(const char* cmd, obj_ptr<BufferedStream_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(os_base);

public:
	static v8::Handle<v8::Value> s_get_shell(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_hostname(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_version(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_arch(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_timezone(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_uptime(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_loadavg(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_totalmem(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_freemem(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUInfo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_CPUs(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_networkInfo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_time(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_exists(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_unlink(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_mkdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rmdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_rename(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_stat(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_readdir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_exit(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_system(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_exec(const v8::Arguments& args);

public:
	ASYNC_STATICVALUE2(os_base, exists, const char*, bool);
	ASYNC_STATIC1(os_base, unlink, const char*);
	ASYNC_STATIC1(os_base, mkdir, const char*);
	ASYNC_STATIC1(os_base, rmdir, const char*);
	ASYNC_STATIC2(os_base, rename, const char*, const char*);
	ASYNC_STATICVALUE2(os_base, stat, const char*, obj_ptr<Stat_base>);
	ASYNC_STATICVALUE2(os_base, readdir, const char*, obj_ptr<List_base>);
	ASYNC_STATICVALUE2(os_base, system, const char*, int32_t);
	ASYNC_STATICVALUE2(os_base, exec, const char*, obj_ptr<BufferedStream_base>);
};

}

#include "Stat.h"
#include "List.h"
#include "BufferedStream.h"

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
			{"exists", s_exists, true},
			{"unlink", s_unlink, true},
			{"mkdir", s_mkdir, true},
			{"rmdir", s_rmdir, true},
			{"rename", s_rename, true},
			{"stat", s_stat, true},
			{"readdir", s_readdir, true},
			{"exit", s_exit, true},
			{"system", s_system, true},
			{"exec", s_exec, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"shell", s_get_shell, NULL, true},
			{"hostname", s_get_hostname, NULL, true},
			{"type", s_get_type, NULL, true},
			{"version", s_get_version, NULL, true},
			{"arch", s_get_arch, NULL, true},
			{"timezone", s_get_timezone, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"os", NULL, 
			18, s_method, 0, NULL, 6, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> os_base::s_get_shell(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_shell(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_hostname(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_hostname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_type(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_version(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_version(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_arch(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		std::string vr;

		PROPERTY_ENTER();

		hr = get_arch(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_get_timezone(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_timezone(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_uptime(const v8::Arguments& args)
	{
		double vr;

		METHOD_ENTER(0, 0);

		hr = uptime(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_loadavg(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = loadavg(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_totalmem(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_ENTER(0, 0);

		hr = totalmem(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_freemem(const v8::Arguments& args)
	{
		int64_t vr;

		METHOD_ENTER(0, 0);

		hr = freemem(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUInfo(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = CPUInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_CPUs(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_ENTER(0, 0);

		hr = CPUs(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_networkInfo(const v8::Arguments& args)
	{
		v8::Handle<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = networkInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_time(const v8::Arguments& args)
	{
		date_t vr;

		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "");

		hr = time(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_exists(const v8::Arguments& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_exists(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_unlink(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_unlink(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> os_base::s_mkdir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_mkdir(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> os_base::s_rmdir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_rmdir(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> os_base::s_rename(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = ac_rename(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> os_base::s_stat(const v8::Arguments& args)
	{
		obj_ptr<Stat_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_stat(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_readdir(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_readdir(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_exit(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = exit(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> os_base::s_system(const v8::Arguments& args)
	{
		int32_t vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_system(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_exec(const v8::Arguments& args)
	{
		obj_ptr<BufferedStream_base> vr;

		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = ac_exec(v0, vr);

		METHOD_RETURN();
	}

}

#endif

