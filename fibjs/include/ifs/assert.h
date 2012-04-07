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

namespace fibjs
{

class assert_base : public object_base
{
public:
	// assert_base
	static result_t ok(bool value, const char* msg);
	static result_t equal(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t strictEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t notStrictEqual(v8::Handle<v8::Value> actual, v8::Handle<v8::Value> expected, const char* msg);
	static result_t throws(v8::Handle<v8::Function> block, const char* msg);
	static result_t doesNotThrow(v8::Handle<v8::Function> block, const char* msg);
	static result_t throwAssert(bool bThrow);

public:
	static ClassInfo& class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"ok", s_ok},
			{"equal", s_equal},
			{"notEqual", s_notEqual},
			{"strictEqual", s_strictEqual},
			{"notStrictEqual", s_notStrictEqual},
			{"throws", s_throws},
			{"doesNotThrow", s_doesNotThrow},
			{"throwAssert", s_throwAssert}
		};

		static ClassData s_cd = 
		{ 
			"assert", NULL, 
			8, s_method, 0, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

private:
	static v8::Handle<v8::Value> s_ok(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_equal(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_strictEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_notStrictEqual(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_throws(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_doesNotThrow(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_throwAssert(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline v8::Handle<v8::Value> assert_base::s_ok(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 1);

		ARG(bool, 0);
		OPT_ARG_String(1, "");

		hr = ok(v0, v1);

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

	inline v8::Handle<v8::Value> assert_base::s_throwAssert(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(bool, 0, true);

		hr = throwAssert(v0);

		METHOD_VOID();
	}

}

#endif

