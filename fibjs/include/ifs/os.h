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

class os_base : public module_base
{
public:
	// os_base
	static result_t hostname(std::string& retVal);
	static result_t type(std::string& retVal);
	static result_t release(std::string& retVal);
	static result_t arch(std::string& retVal);
	static result_t CPUInfo(v8::Handle<v8::Array>& retVal);
	static result_t CPUs(int32_t& retVal);
	static result_t networkInfo(v8::Handle<v8::Array>& retVal);
	static result_t time(date_t& retVal);
	static result_t exists(const char* path, bool& retVal, exlib::AsyncEvent* ac);
	static result_t unlink(const char* path, exlib::AsyncEvent* ac);
	static result_t mkdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rmdir(const char* path, exlib::AsyncEvent* ac);
	static result_t rename(const char* from, const char* to, exlib::AsyncEvent* ac);
	static result_t stat(const char* path, obj_ptr<Stat_base>& retVal, exlib::AsyncEvent* ac);
	static result_t readdir(const char* path, obj_ptr<List_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(os_base);

public:
	static v8::Handle<v8::Value> s_hostname(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_type(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_release(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_arch(const v8::Arguments& args);
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

public:
	ASYNC_STATIC2(os_base, exists);
	ASYNC_STATIC1(os_base, unlink);
	ASYNC_STATIC1(os_base, mkdir);
	ASYNC_STATIC1(os_base, rmdir);
	ASYNC_STATIC2(os_base, rename);
	ASYNC_STATIC2(os_base, stat);
	ASYNC_STATIC2(os_base, readdir);
};

}

#include "Stat.h"
#include "List.h"

namespace fibjs
{
	inline ClassInfo& os_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"hostname", s_hostname, true},
			{"type", s_type, true},
			{"release", s_release, true},
			{"arch", s_arch, true},
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
			{"readdir", s_readdir, true}
		};

		static ClassData s_cd = 
		{ 
			"os", NULL, 
			15, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> os_base::s_hostname(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = hostname(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_type(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = type(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_release(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = release(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_arch(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_ENTER(0, 0);

		hr = arch(vr);

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
		v8::Handle<v8::Array> vr;

		METHOD_ENTER(0, 0);

		hr = networkInfo(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> os_base::s_time(const v8::Arguments& args)
	{
		date_t vr;

		METHOD_ENTER(0, 0);

		hr = time(vr);

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

}

#endif

