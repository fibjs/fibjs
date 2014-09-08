/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _assert_base_H_
#define _assert_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;

class assert_base : public module_base
{
	DECLARE_CLASS(assert_base);

public:
	// assert_base
	static result_t ok(v8::Local<v8::Value> actual, const char* msg);
	static result_t notOk(v8::Local<v8::Value> actual, const char* msg);
	static result_t equal(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t notEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t strictEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t notStrictEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t deepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t notDeepEqual(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t closeTo(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, v8::Local<v8::Value> delta, const char* msg);
	static result_t notCloseTo(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, v8::Local<v8::Value> delta, const char* msg);
	static result_t lessThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t notLessThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t greaterThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t notGreaterThan(v8::Local<v8::Value> actual, v8::Local<v8::Value> expected, const char* msg);
	static result_t exist(v8::Local<v8::Value> actual, const char* msg);
	static result_t notExist(v8::Local<v8::Value> actual, const char* msg);
	static result_t isTrue(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotTrue(v8::Local<v8::Value> actual, const char* msg);
	static result_t isFalse(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotFalse(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNull(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotNull(v8::Local<v8::Value> actual, const char* msg);
	static result_t isUndefined(v8::Local<v8::Value> actual, const char* msg);
	static result_t isDefined(v8::Local<v8::Value> actual, const char* msg);
	static result_t isFunction(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotFunction(v8::Local<v8::Value> actual, const char* msg);
	static result_t isObject(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotObject(v8::Local<v8::Value> actual, const char* msg);
	static result_t isArray(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotArray(v8::Local<v8::Value> actual, const char* msg);
	static result_t isString(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotString(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNumber(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotNumber(v8::Local<v8::Value> actual, const char* msg);
	static result_t isBoolean(v8::Local<v8::Value> actual, const char* msg);
	static result_t isNotBoolean(v8::Local<v8::Value> actual, const char* msg);
	static result_t typeOf(v8::Local<v8::Value> actual, const char* type, const char* msg);
	static result_t notTypeOf(v8::Local<v8::Value> actual, const char* type, const char* msg);
	static result_t property(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, const char* msg);
	static result_t notProperty(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, const char* msg);
	static result_t deepProperty(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, const char* msg);
	static result_t notDeepProperty(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, const char* msg);
	static result_t propertyVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, const char* msg);
	static result_t propertyNotVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, const char* msg);
	static result_t deepPropertyVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, const char* msg);
	static result_t deepPropertyNotVal(v8::Local<v8::Value> object, v8::Local<v8::Value> prop, v8::Local<v8::Value> value, const char* msg);
	static result_t throws(v8::Local<v8::Function> block, const char* msg);
	static result_t doesNotThrow(v8::Local<v8::Function> block, const char* msg);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_ok(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notOk(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_equal(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_strictEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_deepEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notCloseTo(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_lessThan(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notLessThan(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_greaterThan(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notGreaterThan(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exist(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notExist(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isTrue(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotTrue(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isFalse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotFalse(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotNull(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isDefined(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotObject(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotArray(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotNumber(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isNotBoolean(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_typeOf(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notTypeOf(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_property(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_deepProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_notDeepProperty(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_propertyVal(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_propertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_deepPropertyVal(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_deepPropertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_throws(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_doesNotThrow(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& assert_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"ok", s_ok},
			{"notOk", s_notOk},
			{"equal", s_equal},
			{"notEqual", s_notEqual},
			{"strictEqual", s_strictEqual},
			{"notStrictEqual", s_notStrictEqual},
			{"deepEqual", s_deepEqual},
			{"notDeepEqual", s_notDeepEqual},
			{"closeTo", s_closeTo},
			{"notCloseTo", s_notCloseTo},
			{"lessThan", s_lessThan},
			{"notLessThan", s_notLessThan},
			{"greaterThan", s_greaterThan},
			{"notGreaterThan", s_notGreaterThan},
			{"exist", s_exist},
			{"notExist", s_notExist},
			{"isTrue", s_isTrue},
			{"isNotTrue", s_isNotTrue},
			{"isFalse", s_isFalse},
			{"isNotFalse", s_isNotFalse},
			{"isNull", s_isNull},
			{"isNotNull", s_isNotNull},
			{"isUndefined", s_isUndefined},
			{"isDefined", s_isDefined},
			{"isFunction", s_isFunction},
			{"isNotFunction", s_isNotFunction},
			{"isObject", s_isObject},
			{"isNotObject", s_isNotObject},
			{"isArray", s_isArray},
			{"isNotArray", s_isNotArray},
			{"isString", s_isString},
			{"isNotString", s_isNotString},
			{"isNumber", s_isNumber},
			{"isNotNumber", s_isNotNumber},
			{"isBoolean", s_isBoolean},
			{"isNotBoolean", s_isNotBoolean},
			{"typeOf", s_typeOf},
			{"notTypeOf", s_notTypeOf},
			{"property", s_property},
			{"notProperty", s_notProperty},
			{"deepProperty", s_deepProperty},
			{"notDeepProperty", s_notDeepProperty},
			{"propertyVal", s_propertyVal},
			{"propertyNotVal", s_propertyNotVal},
			{"deepPropertyVal", s_deepPropertyVal},
			{"deepPropertyNotVal", s_deepPropertyNotVal},
			{"throws", s_throws},
			{"doesNotThrow", s_doesNotThrow}
		};

		static ClassData s_cd = 
		{ 
			"assert", NULL, 
			48, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void assert_base::s_ok(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = ok(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_notOk(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = notOk(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_equal(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = equal(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_strictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = strictEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notStrictEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notStrictEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_deepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = deepEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notDeepEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notDeepEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		ARG(v8::Local<v8::Value>, 2);
		OPT_ARG(arg_string, 3, "");

		hr = closeTo(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void assert_base::s_notCloseTo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		ARG(v8::Local<v8::Value>, 2);
		OPT_ARG(arg_string, 3, "");

		hr = notCloseTo(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void assert_base::s_lessThan(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = lessThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notLessThan(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notLessThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_greaterThan(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = greaterThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notGreaterThan(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notGreaterThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_exist(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = exist(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_notExist(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = notExist(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isTrue(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isTrue(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotTrue(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotTrue(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isFalse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isFalse(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotFalse(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotFalse(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNull(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNull(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotNull(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotNull(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isUndefined(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isUndefined(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isDefined(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isDefined(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isFunction(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotFunction(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isObject(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isObject(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotObject(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotObject(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isArray(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isArray(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotArray(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotArray(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isString(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotString(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNumber(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotNumber(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotNumber(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isBoolean(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_isNotBoolean(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Value>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = isNotBoolean(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_typeOf(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(arg_string, 1);
		OPT_ARG(arg_string, 2, "");

		hr = typeOf(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notTypeOf(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(arg_string, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notTypeOf(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_property(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = property(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notProperty(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_deepProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = deepProperty(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_notDeepProperty(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = notDeepProperty(v0, v1, v2);

		METHOD_VOID();
	}

	inline void assert_base::s_propertyVal(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		ARG(v8::Local<v8::Value>, 2);
		OPT_ARG(arg_string, 3, "");

		hr = propertyVal(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void assert_base::s_propertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		ARG(v8::Local<v8::Value>, 2);
		OPT_ARG(arg_string, 3, "");

		hr = propertyNotVal(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void assert_base::s_deepPropertyVal(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		ARG(v8::Local<v8::Value>, 2);
		OPT_ARG(arg_string, 3, "");

		hr = deepPropertyVal(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void assert_base::s_deepPropertyNotVal(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);
		ARG(v8::Local<v8::Value>, 2);
		OPT_ARG(arg_string, 3, "");

		hr = deepPropertyNotVal(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline void assert_base::s_throws(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Function>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = throws(v0, v1);

		METHOD_VOID();
	}

	inline void assert_base::s_doesNotThrow(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Local<v8::Function>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = doesNotThrow(v0, v1);

		METHOD_VOID();
	}

}

#endif

