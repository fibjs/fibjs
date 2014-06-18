/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _util_base_H_
#define _util_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Stats_base;
class LruCache_base;

class util_base : public module_base
{
public:
	// util_base
	static result_t format(const char* fmt, const v8::FunctionCallbackInfo<v8::Value>& args, std::string& retVal);
	static result_t format(const v8::FunctionCallbackInfo<v8::Value>& args, std::string& retVal);
	static result_t isArray(v8::Local<v8::Value> v, bool& retVal);
	static result_t isBoolean(v8::Local<v8::Value> v, bool& retVal);
	static result_t isNull(v8::Local<v8::Value> v, bool& retVal);
	static result_t isNullOrUndefined(v8::Local<v8::Value> v, bool& retVal);
	static result_t isNumber(v8::Local<v8::Value> v, bool& retVal);
	static result_t isString(v8::Local<v8::Value> v, bool& retVal);
	static result_t isUndefined(v8::Local<v8::Value> v, bool& retVal);
	static result_t isRegExp(v8::Local<v8::Value> v, bool& retVal);
	static result_t isObject(v8::Local<v8::Value> v, bool& retVal);
	static result_t isDate(v8::Local<v8::Value> v, bool& retVal);
	static result_t isFunction(v8::Local<v8::Value> v, bool& retVal);
	static result_t isBuffer(v8::Local<v8::Value> v, bool& retVal);
	static result_t buildInfo(v8::Local<v8::Object>& retVal);

	DECLARE_CLASSINFO(util_base);

public:
	static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isDate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Stats.h"
#include "LruCache.h"

namespace fibjs
{
	inline ClassInfo& util_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"format", s_format, true},
			{"isArray", s_isArray, true},
			{"isBoolean", s_isBoolean, true},
			{"isNull", s_isNull, true},
			{"isNullOrUndefined", s_isNullOrUndefined, true},
			{"isNumber", s_isNumber, true},
			{"isString", s_isString, true},
			{"isUndefined", s_isUndefined, true},
			{"isRegExp", s_isRegExp, true},
			{"isObject", s_isObject, true},
			{"isDate", s_isDate, true},
			{"isFunction", s_isFunction, true},
			{"isBuffer", s_isBuffer, true},
			{"buildInfo", s_buildInfo, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Stats", Stats_base::class_info},
			{"LruCache", LruCache_base::class_info}
		};

		static ClassData s_cd = 
		{ 
			"util", NULL, 
			14, s_method, 2, s_object, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void util_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = format(v0, args, vr);

		METHOD_OVER(-1, 0);

		hr = format(args, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isArray(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isBoolean(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isNull(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isNullOrUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isNullOrUndefined(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isNumber(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isString(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isUndefined(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isRegExp(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isRegExp(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isObject(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isDate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isDate(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isFunction(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_isBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = isBuffer(v0, vr);

		METHOD_RETURN();
	}

	inline void util_base::s_buildInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Object> vr;

		METHOD_ENTER(0, 0);

		hr = buildInfo(vr);

		METHOD_RETURN();
	}

}

#endif

