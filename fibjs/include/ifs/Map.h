/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Map_base_H_
#define _Map_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Map_base : public object_base
{
public:
	// Map_base
	static result_t _new(obj_ptr<Map_base>& retVal);
	virtual result_t get_size(int32_t& retVal) = 0;
	virtual result_t clear() = 0;
	virtual result_t has(const char* name, bool& retVal) = 0;
	virtual result_t get(const char* name, Variant& retVal) = 0;
	virtual result_t put(v8::Handle<v8::Object> map) = 0;
	virtual result_t put(const char* name, Variant value) = 0;
	virtual result_t remove(const char* name) = 0;
	virtual result_t isEmpty(bool& retVal) = 0;
	virtual result_t _named_getter(const char* property, Variant& retVal) = 0;
	virtual result_t _named_setter(const char* property, Variant newVal) = 0;

	DECLARE_CLASSINFO(Map_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(size, int32_t);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_size(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_has(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_put(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_remove(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_isEmpty(const v8::Arguments& args);
	static v8::Handle<v8::Value> i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

}

namespace fibjs
{
	inline ClassInfo& Map_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"clear", s_clear},
			{"has", s_has},
			{"get", s_get},
			{"put", s_put},
			{"remove", s_remove},
			{"isEmpty", s_isEmpty}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"size", s_get_size}
		};

		static ClassData::ClassNamed s_named = 
		{
			i_NamedGetter, i_NamedSetter
		};

		static ClassData s_cd = 
		{ 
			"Map", s__new, 
			6, s_method, 0, NULL, 1, s_property, NULL, &s_named,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Map_base::s_get_size(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Map_base);

		hr = pInst->get_size(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Map_base::i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		Variant vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Map_base);

		hr = pInst->_named_getter(*v8::String::Utf8Value(property), vr);
		if(hr == CALL_RETURN_NULL)return v8::Handle<v8::Value>();

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Map_base::i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Map_base);

		PROPERTY_VAL(Variant);
		hr = pInst->_named_setter(*v8::String::Utf8Value(property), v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Map_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Map_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Map_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Map_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Map_base::s_has(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Map_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->has(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Map_base::s_get(const v8::Arguments& args)
	{
		Variant vr;

		METHOD_INSTANCE(Map_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->get(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Map_base::s_put(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Map_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->put(v0);

		METHOD_OVER(2, 2);

		ARG_String(0);
		ARG(Variant, 1);

		hr = pInst->put(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Map_base::s_remove(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Map_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->remove(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Map_base::s_isEmpty(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Map_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isEmpty(vr);

		METHOD_RETURN();
	}

}

#endif

