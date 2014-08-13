/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
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
	virtual result_t equal(v8::Local<v8::Value> expected) = 0;
	virtual result_t eql(v8::Local<v8::Value> expected) = 0;
	virtual result_t above(v8::Local<v8::Value> expected) = 0;
	virtual result_t greaterThan(v8::Local<v8::Value> expected) = 0;
	virtual result_t least(v8::Local<v8::Value> expected) = 0;
	virtual result_t below(v8::Local<v8::Value> expected) = 0;
	virtual result_t lessThan(v8::Local<v8::Value> expected) = 0;
	virtual result_t most(v8::Local<v8::Value> expected) = 0;
	virtual result_t property(v8::Local<v8::Value> prop) = 0;
	virtual result_t property(v8::Local<v8::Value> prop, v8::Local<v8::Value> value) = 0;
	virtual result_t closeTo(v8::Local<v8::Value> expected, v8::Local<v8::Value> delta) = 0;

	DECLARE_CLASSINFO(Expect_base);

public:
	static void s_get_to(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_be(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_been(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_is(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_that(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_and(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_have(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_with(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_at(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_of(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_same(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_not(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_deep(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_a(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_an(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_ok(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_true(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_false(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_null(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_undefined(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_function(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_object(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_array(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_string(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_number(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_boolean(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_exist(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_equal(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_eql(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_above(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_greaterThan(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_least(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_below(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_lessThan(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_most(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_property(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args);
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
			{"property", s_property},
			{"closeTo", s_closeTo}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"to", s_get_to, block_set},
			{"be", s_get_be, block_set},
			{"been", s_get_been, block_set},
			{"is", s_get_is, block_set},
			{"that", s_get_that, block_set},
			{"and", s_get_and, block_set},
			{"have", s_get_have, block_set},
			{"with", s_get_with, block_set},
			{"at", s_get_at, block_set},
			{"of", s_get_of, block_set},
			{"same", s_get_same, block_set},
			{"not", s_get_not, block_set},
			{"deep", s_get_deep, block_set},
			{"ok", s_get_ok, block_set},
			{"true", s_get_true, block_set},
			{"false", s_get_false, block_set},
			{"null", s_get_null, block_set},
			{"undefined", s_get_undefined, block_set},
			{"function", s_get_function, block_set},
			{"object", s_get_object, block_set},
			{"array", s_get_array, block_set},
			{"string", s_get_string, block_set},
			{"number", s_get_number, block_set},
			{"boolean", s_get_boolean, block_set},
			{"exist", s_get_exist, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Expect", NULL, 
			12, s_method, 0, NULL, 25, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Expect_base::s_get_to(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_to(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_be(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_be(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_been(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_been(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_is(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_is(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_that(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_that(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_and(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_and(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_have(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_have(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_with(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_with(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_at(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_at(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_of(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_of(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_same(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_same(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_not(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_not(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_deep(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<Expect_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_deep(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_ok(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_ok(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_true(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_true(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_false(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_false(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_null(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_null(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_undefined(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_undefined(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_function(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_function(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_object(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_object(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_array(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_array(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_string(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_string(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_number(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_number(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_boolean(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_boolean(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_get_exist(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Expect_base);

		hr = pInst->get_exist(vr);

		METHOD_RETURN();
	}

	inline void Expect_base::s_a(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->a(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_an(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->an(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_equal(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->equal(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_eql(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->eql(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_above(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->above(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_greaterThan(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->greaterThan(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_least(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->least(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_below(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->below(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_lessThan(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->lessThan(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_most(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->most(v0);

		METHOD_VOID();
	}

	inline void Expect_base::s_property(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Value>, 0);

		hr = pInst->property(v0);

		METHOD_OVER(2, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);

		hr = pInst->property(v0, v1);

		METHOD_VOID();
	}

	inline void Expect_base::s_closeTo(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Expect_base);
		METHOD_ENTER(2, 2);

		ARG(v8::Local<v8::Value>, 0);
		ARG(v8::Local<v8::Value>, 1);

		hr = pInst->closeTo(v0, v1);

		METHOD_VOID();
	}

}

#endif

