/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _List_base_H_
#define _List_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class List_base : public object_base
{
public:
	// List_base
	static result_t _new(obj_ptr<List_base>& retVal);
	virtual result_t _indexed_getter(uint32_t index, Variant& retVal) = 0;
	virtual result_t _indexed_setter(uint32_t index, Variant newVal) = 0;
	virtual result_t get_length(int32_t& retVal) = 0;
	virtual result_t resize(int32_t sz) = 0;
	virtual result_t push(Variant v) = 0;
	virtual result_t push(Variant v, const v8::Arguments& args) = 0;
	virtual result_t pop(Variant& retVal) = 0;
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal) = 0;
	virtual result_t concat(const v8::Arguments& args, obj_ptr<List_base>& retVal) = 0;
	virtual result_t every(v8::Handle<v8::Function> func, v8::Handle<v8::Object> thisp, bool& retVal) = 0;
	virtual result_t filter(v8::Handle<v8::Function> func, v8::Handle<v8::Object> thisp, obj_ptr<List_base>& retVal) = 0;
	virtual result_t forEach(v8::Handle<v8::Function> func, v8::Handle<v8::Object> thisp) = 0;
	virtual result_t map(v8::Handle<v8::Function> func, v8::Handle<v8::Object> thisp, obj_ptr<List_base>& retVal) = 0;
	virtual result_t toArray(v8::Handle<v8::Array>& retVal) = 0;

	DECLARE_CLASSINFO(List_base);

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> s_get_length(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_resize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_push(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_pop(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_slice(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_concat(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_every(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_filter(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_forEach(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_map(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_toArray(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& List_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"resize", s_resize},
			{"push", s_push},
			{"pop", s_pop},
			{"slice", s_slice},
			{"concat", s_concat},
			{"every", s_every},
			{"filter", s_filter},
			{"forEach", s_forEach},
			{"map", s_map},
			{"toArray", s_toArray}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"length", s_get_length}
		};

		static ClassData::ClassIndexed s_indexed = 
		{
			i_IndexedGetter, i_IndexedSetter
		};

		static ClassData s_cd = 
		{ 
			"List", s__new, 
			10, s_method, 0, NULL, 1, s_property, &s_indexed, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> List_base::i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info)
	{
		Variant vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(List_base);

		hr = pInst->_indexed_getter(index, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(List_base);

		PROPERTY_VAL(Variant);
		hr = pInst->_indexed_setter(index, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> List_base::s_get_length(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(List_base);

		hr = pInst->get_length(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_resize(const v8::Arguments& args)
	{
		METHOD_INSTANCE(List_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->resize(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> List_base::s_push(const v8::Arguments& args)
	{
		METHOD_INSTANCE(List_base);
		METHOD_ENTER(1, 1);

		ARG(Variant, 0);

		hr = pInst->push(v0);

		METHOD_OVER(-1, 1);

		ARG(Variant, 0);

		hr = pInst->push(v0, args);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> List_base::s_pop(const v8::Arguments& args)
	{
		Variant vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(0, 0);

		hr = pInst->pop(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_slice(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, 0);
		OPT_ARG(int32_t, 1, -1);

		hr = pInst->slice(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_concat(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(-1, 0);

		hr = pInst->concat(args, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_every(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Function>, 0);
		OPT_ARG(v8::Handle<v8::Object>, 1, v8::Object::New());

		hr = pInst->every(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_filter(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Function>, 0);
		OPT_ARG(v8::Handle<v8::Object>, 1, v8::Object::New());

		hr = pInst->filter(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_forEach(const v8::Arguments& args)
	{
		METHOD_INSTANCE(List_base);
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Function>, 0);
		OPT_ARG(v8::Handle<v8::Object>, 1, v8::Object::New());

		hr = pInst->forEach(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> List_base::s_map(const v8::Arguments& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(2, 1);

		ARG(v8::Handle<v8::Function>, 0);
		OPT_ARG(v8::Handle<v8::Object>, 1, v8::Object::New());

		hr = pInst->map(v0, v1, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> List_base::s_toArray(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_INSTANCE(List_base);
		METHOD_ENTER(0, 0);

		hr = pInst->toArray(vr);

		METHOD_RETURN();
	}

}

#endif

