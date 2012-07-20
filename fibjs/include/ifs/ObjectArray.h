/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _ObjectArray_base_H_
#define _ObjectArray_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class object_base;

class ObjectArray_base : public object_base
{
public:
	// ObjectArray_base
	virtual result_t _indexed_getter(uint32_t index, obj_ptr<object_base>& retVal) = 0;
	virtual result_t _indexed_setter(uint32_t index, obj_ptr<object_base>& newVal) = 0;
	virtual result_t get_length(int32_t& retVal) = 0;
	virtual result_t resize(int32_t sz) = 0;
	virtual result_t slice(int32_t start, int32_t end, obj_ptr<ObjectArray_base>& retVal) = 0;

public:
	static ClassInfo& class_info();

	virtual ClassInfo& Classinfo()
	{
		return class_info();
	}

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(length, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> s_get_length(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_resize(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_slice(const v8::Arguments& args);
};

}


namespace fibjs
{
	inline ClassInfo& ObjectArray_base::class_info()
	{
		static ClassMethod s_method[] = 
		{
			{"resize", s_resize},
			{"slice", s_slice}
		};

		static ClassProperty s_property[] = 
		{
			{"length", s_get_length}
		};

		static ClassIndexed s_indexed = 
		{
			i_IndexedGetter, i_IndexedSetter
		};

		static ClassData s_cd = 
		{ 
			"ObjectArray", NULL, 
			2, s_method, 0, NULL, 1, s_property, &s_indexed,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> ObjectArray_base::i_IndexedGetter(uint32_t index, const v8::AccessorInfo& info)
	{
		obj_ptr<object_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(ObjectArray_base);

		hr = pInst->_indexed_getter(index, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> ObjectArray_base::i_IndexedSetter(uint32_t index, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(ObjectArray_base);

		PROPERTY_VAL(obj_ptr<object_base>);
		hr = pInst->_indexed_setter(index, v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> ObjectArray_base::s_get_length(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(ObjectArray_base);

		hr = pInst->get_length(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> ObjectArray_base::s_resize(const v8::Arguments& args)
	{
		METHOD_INSTANCE(ObjectArray_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->resize(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> ObjectArray_base::s_slice(const v8::Arguments& args)
	{
		obj_ptr<ObjectArray_base> vr;

		METHOD_INSTANCE(ObjectArray_base);
		METHOD_ENTER(2, 0);

		OPT_ARG(int32_t, 0, 0);
		OPT_ARG(int32_t, 1, -1);

		hr = pInst->slice(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

