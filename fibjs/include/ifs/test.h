/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _test_base_H_
#define _test_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class console_base;
class assert_base;
class Expect_base;

class test_base : public module_base
{
public:
	// test_base
	static result_t describe(const char* name, v8::Handle<v8::Function> block);
	static result_t xdescribe(const char* name, v8::Handle<v8::Function> block);
	static result_t it(const char* name, v8::Handle<v8::Function> block);
	static result_t xit(const char* name, v8::Handle<v8::Function> block);
	static result_t before(v8::Handle<v8::Function> func);
	static result_t after(v8::Handle<v8::Function> func);
	static result_t beforeEach(v8::Handle<v8::Function> func);
	static result_t afterEach(v8::Handle<v8::Function> func);
	static result_t run(int32_t loglevel);
	static result_t get_assert(obj_ptr<assert_base>& retVal);
	static result_t expect(v8::Handle<v8::Value> actual, const char* msg, obj_ptr<Expect_base>& retVal);

	DECLARE_CLASSINFO(test_base);

public:
	static v8::Handle<v8::Value> s_describe(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_xdescribe(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_it(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_xit(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_before(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_after(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_beforeEach(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_afterEach(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_run(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_assert(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_expect(const v8::Arguments& args);
};

}

#include "console.h"
#include "assert.h"
#include "Expect.h"

namespace fibjs
{
	inline ClassInfo& test_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"describe", s_describe, true},
			{"xdescribe", s_xdescribe, true},
			{"it", s_it, true},
			{"xit", s_xit, true},
			{"before", s_before, true},
			{"after", s_after, true},
			{"beforeEach", s_beforeEach, true},
			{"afterEach", s_afterEach, true},
			{"run", s_run, true},
			{"expect", s_expect, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"assert", s_get_assert, NULL, true}
		};

		static ClassData s_cd = 
		{ 
			"test", NULL, 
			10, s_method, 0, NULL, 1, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> test_base::s_get_assert(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<assert_base> vr;

		PROPERTY_ENTER();

		hr = get_assert(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> test_base::s_describe(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = describe(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_xdescribe(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = xdescribe(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_it(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = it(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_xit(const v8::Arguments& args)
	{
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(v8::Handle<v8::Function>, 1);

		hr = xit(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_before(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = before(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_after(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = after(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_beforeEach(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = beforeEach(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_afterEach(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Function>, 0);

		hr = afterEach(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_run(const v8::Arguments& args)
	{
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, console_base::_ERROR);

		hr = run(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> test_base::s_expect(const v8::Arguments& args)
	{
		obj_ptr<Expect_base> vr;

		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Value>, 0);
		OPT_ARG_String(1, "");

		hr = expect(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

