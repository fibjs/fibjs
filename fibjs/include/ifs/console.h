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
class BufferedStream_base;

class console_base : public module_base
{
public:
	enum{
		_FATAL = 0,
		_ALERT = 100,
		_CRIT = 200,
		_ERROR = 300,
		_WARN = 400,
		_NOTICE = 500,
		_INFO = 600,
		_DEBUG = 700,
		_NOTSET = 800
	};

public:
	// console_base
	static result_t get_stderr(obj_ptr<BufferedStream_base>& retVal);
	static result_t get_stdin(obj_ptr<BufferedStream_base>& retVal);
	static result_t get_stdout(obj_ptr<BufferedStream_base>& retVal);
	static result_t get_loglevel(int32_t& retVal);
	static result_t set_loglevel(int32_t newVal);
	static result_t log(const char* fmt, const v8::Arguments& args);
	static result_t info(const char* fmt, const v8::Arguments& args);
	static result_t warn(const char* fmt, const v8::Arguments& args);
	static result_t error(const char* fmt, const v8::Arguments& args);
	static result_t dir(v8::Handle<v8::Object> obj);
	static result_t time(const char* label);
	static result_t timeEnd(const char* label);
	static result_t trace(const char* label);
	static result_t assert(v8::Handle<v8::Value> value, const char* msg);

	DECLARE_CLASSINFO(console_base);

public:
	static v8::Handle<v8::Value> s_get_stderr(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_stdin(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_stdout(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_FATAL(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ALERT(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_CRIT(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ERROR(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_WARN(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_NOTICE(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_INFO(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_DEBUG(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_NOTSET(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_loglevel(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
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

#include "BufferedStream.h"

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

		static ClassData::ClassProperty s_property[] = 
		{
			{"stderr", s_get_stderr, NULL, true},
			{"stdin", s_get_stdin, NULL, true},
			{"stdout", s_get_stdout, NULL, true},
			{"FATAL", s_get_FATAL, NULL, true},
			{"ALERT", s_get_ALERT, NULL, true},
			{"CRIT", s_get_CRIT, NULL, true},
			{"ERROR", s_get_ERROR, NULL, true},
			{"WARN", s_get_WARN, NULL, true},
			{"NOTICE", s_get_NOTICE, NULL, true},
			{"INFO", s_get_INFO, NULL, true},
			{"DEBUG", s_get_DEBUG, NULL, true},
			{"NOTSET", s_get_NOTSET, NULL, true},
			{"loglevel", s_get_loglevel, s_set_loglevel}
		};

		static ClassData s_cd = 
		{ 
			"console", NULL, 
			9, s_method, 0, NULL, 13, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> console_base::s_get_stderr(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<BufferedStream_base> vr;

		PROPERTY_ENTER();

		hr = get_stderr(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_stdin(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<BufferedStream_base> vr;

		PROPERTY_ENTER();

		hr = get_stdin(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_stdout(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<BufferedStream_base> vr;

		PROPERTY_ENTER();

		hr = get_stdout(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_FATAL(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _FATAL;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_ALERT(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _ALERT;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_CRIT(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _CRIT;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_ERROR(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _ERROR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_WARN(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _WARN;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_NOTICE(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _NOTICE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_INFO(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _INFO;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_DEBUG(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _DEBUG;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_NOTSET(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr = _NOTSET;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> console_base::s_get_loglevel(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();

		hr = get_loglevel(vr);

		METHOD_RETURN();
	}

	inline void console_base::s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_VAL(int32_t);

		hr = set_loglevel(v0);

		PROPERTY_SET_LEAVE();
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

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = assert(v0, v1);

		METHOD_VOID();
	}

}

#endif

