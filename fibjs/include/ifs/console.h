/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
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
class TextColor_base;

class console_base : public module_base
{
public:
	enum{
		_FATAL = 0,
		_ALERT = 1,
		_CRIT = 2,
		_ERROR = 3,
		_WARN = 4,
		_NOTICE = 5,
		_INFO = 6,
		_DEBUG = 7,
		_NOTSET = 10
	};

public:
	// console_base
	static result_t get_loglevel(int32_t& retVal);
	static result_t set_loglevel(int32_t newVal);
	static result_t get_colors(obj_ptr<TextColor_base>& retVal);
	static result_t add(v8::Local<v8::Array> cfg);
	static result_t add(v8::Local<v8::Value> cfg);
	static result_t reset();
	static result_t log(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t log(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t debug(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t debug(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t info(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t info(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t notice(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t notice(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t warn(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t warn(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t error(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t error(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t crit(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t crit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t alert(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t alert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t dir(v8::Local<v8::Value> obj);
	static result_t time(const char* label);
	static result_t timeEnd(const char* label);
	static result_t trace(const char* label);
	static result_t assert(v8::Local<v8::Value> value, const char* msg);
	static result_t print(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t print(const v8::FunctionCallbackInfo<v8::Value>& args);
	static result_t readLine(const char* msg, std::string& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(console_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_get_FATAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ALERT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CRIT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ERROR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_WARN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_NOTICE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_INFO(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_DEBUG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_NOTSET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_loglevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_colors(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_reset(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_log(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_debug(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_info(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notice(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_warn(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_error(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_crit(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_dir(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_time(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_trace(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_assert(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_print(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_readLine(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE2(console_base, readLine, const char*, std::string);
};

}

#include "TextColor.h"

namespace fibjs
{
	inline ClassInfo& console_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"add", s_add, true},
			{"reset", s_reset, true},
			{"log", s_log, true},
			{"debug", s_debug, true},
			{"info", s_info, true},
			{"notice", s_notice, true},
			{"warn", s_warn, true},
			{"error", s_error, true},
			{"crit", s_crit, true},
			{"alert", s_alert, true},
			{"dir", s_dir, true},
			{"time", s_time, true},
			{"timeEnd", s_timeEnd, true},
			{"trace", s_trace, true},
			{"assert", s_assert, true},
			{"print", s_print, true},
			{"readLine", s_readLine, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"FATAL", s_get_FATAL, block_set, true},
			{"ALERT", s_get_ALERT, block_set, true},
			{"CRIT", s_get_CRIT, block_set, true},
			{"ERROR", s_get_ERROR, block_set, true},
			{"WARN", s_get_WARN, block_set, true},
			{"NOTICE", s_get_NOTICE, block_set, true},
			{"INFO", s_get_INFO, block_set, true},
			{"DEBUG", s_get_DEBUG, block_set, true},
			{"NOTSET", s_get_NOTSET, block_set, true},
			{"loglevel", s_get_loglevel, s_set_loglevel},
			{"colors", s_get_colors, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"console", NULL, 
			17, s_method, 0, NULL, 11, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void console_base::s_get_FATAL(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _FATAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_ALERT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ALERT;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_CRIT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CRIT;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_ERROR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ERROR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_WARN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _WARN;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_NOTICE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _NOTICE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_INFO(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _INFO;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_DEBUG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DEBUG;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_NOTSET(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _NOTSET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void console_base::s_get_loglevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_loglevel(vr);

		METHOD_RETURN();
	}

	inline void console_base::s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_VAL(int32_t);

		hr = set_loglevel(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void console_base::s_get_colors(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<TextColor_base> vr;

		PROPERTY_ENTER();

		hr = get_colors(vr);

		METHOD_RETURN();
	}

	inline void console_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Array>, 0);

		hr = add(v0);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = add(v0);

		METHOD_VOID();
	}

	inline void console_base::s_reset(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(0, 0);

		hr = reset();

		METHOD_VOID();
	}

	inline void console_base::s_log(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = log(v0, args);

		METHOD_OVER(-1, 0);

		hr = log(args);

		METHOD_VOID();
	}

	inline void console_base::s_debug(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = debug(v0, args);

		METHOD_OVER(-1, 0);

		hr = debug(args);

		METHOD_VOID();
	}

	inline void console_base::s_info(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = info(v0, args);

		METHOD_OVER(-1, 0);

		hr = info(args);

		METHOD_VOID();
	}

	inline void console_base::s_notice(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = notice(v0, args);

		METHOD_OVER(-1, 0);

		hr = notice(args);

		METHOD_VOID();
	}

	inline void console_base::s_warn(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = warn(v0, args);

		METHOD_OVER(-1, 0);

		hr = warn(args);

		METHOD_VOID();
	}

	inline void console_base::s_error(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = error(v0, args);

		METHOD_OVER(-1, 0);

		hr = error(args);

		METHOD_VOID();
	}

	inline void console_base::s_crit(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = crit(v0, args);

		METHOD_OVER(-1, 0);

		hr = crit(args);

		METHOD_VOID();
	}

	inline void console_base::s_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = alert(v0, args);

		METHOD_OVER(-1, 0);

		hr = alert(args);

		METHOD_VOID();
	}

	inline void console_base::s_dir(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = dir(v0);

		METHOD_VOID();
	}

	inline void console_base::s_time(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "time");

		hr = time(v0);

		METHOD_VOID();
	}

	inline void console_base::s_timeEnd(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "time");

		hr = timeEnd(v0);

		METHOD_VOID();
	}

	inline void console_base::s_trace(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "trace");

		hr = trace(v0);

		METHOD_VOID();
	}

	inline void console_base::s_assert(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = assert(v0, v1);

		METHOD_VOID();
	}

	inline void console_base::s_print(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = print(v0, args);

		METHOD_OVER(-1, 0);

		hr = print(args);

		METHOD_VOID();
	}

	inline void console_base::s_readLine(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "");

		hr = ac_readLine(v0, vr);

		METHOD_RETURN();
	}

}

#endif

