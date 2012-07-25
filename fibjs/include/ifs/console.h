/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _console_base_H_
#define _console_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;

class console_base : public module_base
{
public:
	// console_base
	static result_t log(const char* fmt, const v8::Arguments& args);
	static result_t info(const char* fmt, const v8::Arguments& args);
	static result_t warn(const char* fmt, const v8::Arguments& args);
	static result_t error(const char* fmt, const v8::Arguments& args);
	static result_t dir(v8::Handle<v8::Object> obj);
	static result_t time(const char* label);
	static result_t timeEnd(const char* label);
	static result_t trace(const char* label);
	static result_t assert(bool value, const char* msg);

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

public:
	static v8::Handle<v8::Value> s_log(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_info(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_warn(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_error(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_dir(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_time(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_timeEnd(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_trace(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_assert(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& console_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"log", s_log, true},
			{"info", s_info, true},
			{"warn", s_warn, true},
			{"error", s_error, true},
			{"dir", s_dir, true},
			{"time", s_time, true},
			{"timeEnd", s_timeEnd, true},
			{"trace", s_trace, true},
			{"assert", s_assert, true}
		};

		static ClassData s_cd = 
		{ 
			"console", NULL, 
			9, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> console_base::s_log(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = log(v0, args);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_info(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = info(v0, args);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_warn(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = warn(v0, args);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_error(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_String(0);

		hr = error(v0, args);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_dir(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = dir(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_time(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "time");

		hr = time(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_timeEnd(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "time");

		hr = timeEnd(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_trace(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "trace");

		hr = trace(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> console_base::s_assert(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(bool, 0);
		OPT_ARG_String(1, "");

		hr = assert(v0, v1);

		METHOD_VOID();
	}

}

#endif

