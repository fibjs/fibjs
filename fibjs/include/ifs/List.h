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
	virtual result_t append(Variant v) = 0;
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<List_base>& retVal) = 0;

	DECLARE_CLASSINFO(List_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(length, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> s_get_length(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_resize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_append(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_slice(const v8::Arguments& args);
};

}

namespace fibjs
{
	inline ClassInfo& List_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"resize", s_resize},
			{"append", s_append},
			{"slice", s_slice}
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
			3, s_method, 0, NULL, 1, s_property, &s_indexed, NULL,
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

	inline v8::Handle<v8::Value> List_base::s_append(const v8::Arguments& args)
	{
		METHOD_INSTANCE(List_base);
		METHOD_ENTER(1, 1);

		ARG(Variant, 0);

		hr = pInst->append(v0);

		METHOD_VOID();
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

}

#endif

