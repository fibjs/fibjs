/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
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
public:
	// assert_base
	static result_t ok(v8::Handle<v8::Value> actual, const char* msg);
	static result_t notOk(v8::Handle<v8::Value> actual, const char* msg);
	static result_t equal(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t strictEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notStrictEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t deepEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notDeepEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t closeTo(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, v8::Handle<v8::Value> delta, const char* msg);
	static result_t notCloseTo(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, v8::Handle<v8::Value> delta, const char* msg);
	static result_t lessThan(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notLessThan(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t greaterThan(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notGreaterThan(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t exist(v8::Handle<v8::Value> actual, const char* msg);
	static result_t notExist(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isTrue(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotTrue(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isFalse(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotFalse(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNull(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotNull(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isUndefined(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isDefined(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isFunction(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotFunction(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isObject(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotObject(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isArray(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotArray(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isString(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotString(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNumber(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotNumber(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isBoolean(v8::Handle<v8::Value> actual, const char* msg);
	static result_t isNotBoolean(v8::Handle<v8::Value> actual, const char* msg);
	static result_t typeOf(v8::Handle<v8::Value> actual, const char* type, const char* msg);
	static result_t notTypeOf(v8::Handle<v8::Value> actual, const char* type, const char* msg);
	static result_t property(v8::Handle<v8::Value> object, v8::Handle<v8::Value> prop, const char* msg);
	static result_t notProperty(v8::Handle<v8::Value> object, v8::Handle<v8::Value> prop, const char* msg);
	static result_t throws(v8::Handle<v8::Function> block, const char* msg);
	static result_t doesNotThrow(v8::Handle<v8::Function> block, const char* msg);

	DECLARE_CLASSINFO(assert_base);

public:
	static v8::Handle<v8::Value> s_ok(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notOk(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_equal(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_strictEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notStrictEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_deepEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notDeepEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_closeTo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notCloseTo(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_lessThan(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notLessThan(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_greaterThan(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notGreaterThan(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_exist(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notExist(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isTrue(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotTrue(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isFalse(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotFalse(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNull(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotNull(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isUndefined(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isDefined(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isFunction(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotFunction(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isObject(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotObject(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isArray(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotArray(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isString(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotString(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNumber(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotNumber(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isBoolean(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isNotBoolean(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_typeOf(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notTypeOf(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_property(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notProperty(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_throws(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_doesNotThrow(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& assert_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"ok", s_ok, true},
			{"notOk", s_notOk, true},
			{"equal", s_equal, true},
			{"notEqual", s_notEqual, true},
			{"strictEqual", s_strictEqual, true},
			{"notStrictEqual", s_notStrictEqual, true},
			{"deepEqual", s_deepEqual, true},
			{"notDeepEqual", s_notDeepEqual, true},
			{"closeTo", s_closeTo, true},
			{"notCloseTo", s_notCloseTo, true},
			{"lessThan", s_lessThan, true},
			{"notLessThan", s_notLessThan, true},
			{"greaterThan", s_greaterThan, true},
			{"notGreaterThan", s_notGreaterThan, true},
			{"exist", s_exist, true},
			{"notExist", s_notExist, true},
			{"isTrue", s_isTrue, true},
			{"isNotTrue", s_isNotTrue, true},
			{"isFalse", s_isFalse, true},
			{"isNotFalse", s_isNotFalse, true},
			{"isNull", s_isNull, true},
			{"isNotNull", s_isNotNull, true},
			{"isUndefined", s_isUndefined, true},
			{"isDefined", s_isDefined, true},
			{"isFunction", s_isFunction, true},
			{"isNotFunction", s_isNotFunction, true},
			{"isObject", s_isObject, true},
			{"isNotObject", s_isNotObject, true},
			{"isArray", s_isArray, true},
			{"isNotArray", s_isNotArray, true},
			{"isString", s_isString, true},
			{"isNotString", s_isNotString, true},
			{"isNumber", s_isNumber, true},
			{"isNotNumber", s_isNotNumber, true},
			{"isBoolean", s_isBoolean, true},
			{"isNotBoolean", s_isNotBoolean, true},
			{"typeOf", s_typeOf, true},
			{"notTypeOf", s_notTypeOf, true},
			{"property", s_property, true},
			{"notProperty", s_notProperty, true},
			{"throws", s_throws, true},
			{"doesNotThrow", s_doesNotThrow, true}
		};

		static ClassData s_cd = 
		{ 
			"assert", NULL, 
			42, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline v8::Handle<v8::Value> assert_base::s_ok(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = ok(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notOk(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = notOk(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_equal(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = equal(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notEqual(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = notEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_strictEqual(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = strictEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notStrictEqual(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = notStrictEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_deepEqual(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = deepEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notDeepEqual(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = notDeepEqual(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_closeTo(const v8::Arguments& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		ARG(v8::Handle<v8::Value>, 2);
		OPT_ARG_String(3, "");

		hr = closeTo(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notCloseTo(const v8::Arguments& args)
	{
		METHOD_ENTER(4, 3);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		ARG(v8::Handle<v8::Value>, 2);
		OPT_ARG_String(3, "");

		hr = notCloseTo(v0, v1, v2, v3);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_lessThan(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = lessThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notLessThan(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = notLessThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_greaterThan(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = greaterThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notGreaterThan(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = notGreaterThan(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_exist(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = exist(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notExist(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = notExist(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isTrue(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isTrue(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotTrue(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotTrue(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isFalse(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isFalse(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotFalse(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotFalse(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNull(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNull(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotNull(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotNull(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isUndefined(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isUndefined(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isDefined(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isDefined(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isFunction(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isFunction(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotFunction(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotFunction(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isObject(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isObject(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotObject(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotObject(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isArray(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isArray(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotArray(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotArray(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isString(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isString(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotString(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotString(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNumber(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNumber(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotNumber(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotNumber(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isBoolean(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isBoolean(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_isNotBoolean(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = isNotBoolean(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_typeOf(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG_String(1);
		OPT_ARG_String(2, "");

		hr = typeOf(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notTypeOf(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG_String(1);
		OPT_ARG_String(2, "");

		hr = notTypeOf(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_property(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = property(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_notProperty(const v8::Arguments& args)
	{
		METHOD_ENTER(3, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);
		OPT_ARG_String(2, "");

		hr = notProperty(v0, v1, v2);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_throws(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Function>, 0);
		OPT_ARG_String(1, "");

		hr = throws(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> assert_base::s_doesNotThrow(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Function>, 0);
		OPT_ARG_String(1, "");

		hr = doesNotThrow(v0, v1);

		METHOD_VOID();
	}

}

#endif

