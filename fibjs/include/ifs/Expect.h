/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Expect_base_H_
#define _Expect_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Expect_base : public object_base
{
public:
	// Expect_base
	virtual result_t get_to(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_be(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_been(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_is(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_that(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_and(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_have(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_with(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_at(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_of(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_same(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_not(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t get_deep(obj_ptr<Expect_base>& retVal) = 0;
	virtual result_t a(const char* type) = 0;
	virtual result_t an(const char* type) = 0;
	virtual result_t get_ok(bool& retVal) = 0;
	virtual result_t get_true(bool& retVal) = 0;
	virtual result_t get_false(bool& retVal) = 0;
	virtual result_t get_null(bool& retVal) = 0;
	virtual result_t get_undefined(bool& retVal) = 0;
	virtual result_t get_function(bool& retVal) = 0;
	virtual result_t get_object(bool& retVal) = 0;
	virtual result_t get_array(bool& retVal) = 0;
	virtual result_t get_string(bool& retVal) = 0;
	virtual result_t get_number(bool& retVal) = 0;
	virtual result_t get_boolean(bool& retVal) = 0;
	virtual result_t get_exist(bool& retVal) = 0;
	virtual result_t equal(v8::Handle<v8::Value> expected) = 0;
	virtual result_t eql(v8::Handle<v8::Value> expected) = 0;
	virtual result_t above(v8::Handle<v8::Value> expected) = 0;
	virtual result_t greaterThan(v8::Handle<v8::Value> expected) = 0;
	virtual result_t least(v8::Handle<v8::Value> expected) = 0;
	virtual result_t below(v8::Handle<v8::Value> expected) = 0;
	virtual result_t lessThan(v8::Handle<v8::Value> expected) = 0;
	virtual result_t most(v8::Handle<v8::Value> expected) = 0;
	virtual result_t closeTo(v8::Handle<v8::Value> expected, v8::Handle<v8::Value> delta) = 0;

	DECLARE_CLASSINFO(Expect_base);

public:
	static v8::Handle<v8::Value> s_get_to(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_be(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_been(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_is(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_that(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_and(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_have(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_with(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_at(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_of(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_same(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_not(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_deep(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_a(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_an(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_ok(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_true(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_false(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_null(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_undefined(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_function(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_object(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_array(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_string(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_number(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_boolean(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_exist(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_equal(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_eql(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_above(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_greaterThan(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_least(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_below(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_lessThan(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_most(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_closeTo(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& Expect_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"a", s_a},
			{"an", s_an},
			{"equal", s_equal},
			{"eql", s_eql},
			{"above", s_above},
			{"greaterThan", s_greaterThan},
			{"least", s_least},
			{"below", s_below},
			{"lessThan", s_lessThan},
			{"most", s_most},
			{"closeTo", s_closeTo}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"to", s_get_to},
			{"be", s_get_be},
			{"been", s_get_been},
			{"is", s_get_is},
			{"that", s_get_that},
			{"and", s_get_and},
			{"have", s_get_have},
			{"with", s_get_with},
			{"at", s_get_at},
			{"of", s_get_of},
			{"same", s_get_same},
			{"not", s_get_not},
			{"deep", s_get_deep},
			{"ok", s_get_ok},
			{"true", s_get_true},
			{"false", s_get_false},
			{"null", s_get_null},
			{"undefined", s_get_undefined},
			{"function", s_get_function},
			{"object", s_get_object},
			{"array", s_get_array},
			{"string", s_get_string},
			{"number", s_get_number},
			{"boolean", s_get_boolean},
			{"exist", s_get_exist}
		};

		static ClassData s_cd = 
		{ 
			"Expect", NULL, 
			11, s_method, 0, NULL, 25, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_to(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_to(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_be(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_be(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_been(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_been(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_is(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_is(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_that(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_that(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_and(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_and(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_have(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_have(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_with(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_with(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_at(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_at(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_of(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_of(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_same(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_same(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_not(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_not(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_deep(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_deep(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_ok(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_ok(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_true(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_true(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_false(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_false(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_null(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_null(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_undefined(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_undefined(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_function(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_function(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_object(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_object(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_array(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_array(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_string(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_string(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_number(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_number(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_boolean(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_boolean(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_get_exist(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_exist(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Expect_base::s_a(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->a(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_an(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->an(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_equal(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->equal(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_eql(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->eql(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_above(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->above(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_greaterThan(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->greaterThan(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_least(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->least(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_below(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->below(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_lessThan(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->lessThan(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_most(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->most(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Expect_base::s_closeTo(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Handle<v8::Value>, 0);
		ARG(v8::Handle<v8::Value>, 1);

		hr = pInst->closeTo(v0, v1);

		METHOD_VOID();
	}

}

#endif

