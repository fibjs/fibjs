/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _console_H_
#define _console_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "if.h"

namespace fibjs
{

class console_base : public object_base
{
public:
	static result_t log(const char* fmt, const v8::Arguments& args);
	static result_t info(const char* fmt, const v8::Arguments& args);
	static result_t debug(const char* fmt, const v8::Arguments& args);
	static result_t warn(const char* fmt, const v8::Arguments& args);
	static result_t error(const char* fmt, const v8::Arguments& args);
	static result_t time(const char* label = "");
	static result_t timeEnd(const char* label = "");
	static result_t trace(const char* label = "");

public:
	static ClassInfo& info()
	{
		static ClassMethod s_smethod[] = 
		{
			{"log", CLASS_METHOD, {s_log}},
			{"info", CLASS_METHOD, {s_info}},
			{"debug", CLASS_METHOD, {s_debug}},
			{"warn", CLASS_METHOD, {s_warn}},
			{"error", CLASS_METHOD, {s_error}},
			{"time", CLASS_METHOD, {s_time}},
			{"timeEnd", CLASS_METHOD, {s_timeEnd}},
			{"trace", CLASS_METHOD, {s_trace}}
		};

		static ClassInfo s_ci("console", 0, NULL, 8, s_smethod);

		return s_ci;
	}

private:
	static v8::Handle<v8::Value> s_log(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_string(0);

		hr = log(v0, args);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_info(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_string(0);

		hr = info(v0, args);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_debug(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_string(0);

		hr = debug(v0, args);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_warn(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_string(0);

		hr = warn(v0, args);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_error(const v8::Arguments& args)
	{
		METHOD_ENTER(-1, 1);

		ARG_string(0);

		hr = error(v0, args);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_time(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG_string(0, "");

		hr = time(v0);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_timeEnd(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG_string(0, "");

		hr = timeEnd(v0);

		METHOD_VOID();
	}

	static v8::Handle<v8::Value> s_trace(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG_string(0, "");

		hr = trace(v0);

		METHOD_VOID();
	}

};

}

#endif

