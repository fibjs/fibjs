/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Stats_base_H_
#define _Stats_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Stats_base : public object_base
{
public:
	// Stats_base
	static result_t _new(v8::Handle<v8::Array> keys, obj_ptr<Stats_base>& retVal);
	static result_t _new(v8::Handle<v8::Array> staticKeys, v8::Handle<v8::Array> keys, obj_ptr<Stats_base>& retVal);
	virtual result_t inc(const char* key) = 0;
	virtual result_t dec(const char* key) = 0;
	virtual result_t add(const char* key, int32_t value) = 0;
	virtual result_t reset() = 0;
	virtual result_t _named_getter(const char* property, int32_t& retVal) = 0;
	virtual result_t _named_enumerator(v8::Handle<v8::Array>& retVal) = 0;

	DECLARE_CLASSINFO(Stats_base);

public:
	static v8::Handle<v8::Value> s__new(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_inc(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_dec(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_add(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_reset(const v8::Arguments& args);
	static v8::Handle<v8::Value> i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Array> i_NamedEnumerator(const v8::AccessorInfo& info);
};

}

namespace fibjs
{
	inline ClassInfo& Stats_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"inc", s_inc},
			{"dec", s_dec},
			{"add", s_add},
			{"reset", s_reset}
		};

		static ClassData::ClassNamed s_named = 
		{
			i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
		};

		static ClassData s_cd = 
		{ 
			"Stats", s__new, 
			4, s_method, 0, NULL, 0, NULL, NULL, &s_named,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> Stats_base::i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stats_base);

		v8::String::Utf8Value k(property);
		if(class_info().has(*k))return v8::Handle<v8::Value>();

		hr = pInst->_named_getter(*k, vr);
		if(hr == CALL_RETURN_NULL)return v8::Handle<v8::Value>();

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Array> Stats_base::i_NamedEnumerator(const v8::AccessorInfo& info)
	{
		v8::Handle<v8::Array> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Stats_base);

		hr = pInst->_named_enumerator(vr);

		METHOD_RETURN1();
	}

	inline v8::Handle<v8::Value> Stats_base::s__new(const v8::Arguments& args)
	{
		obj_ptr<Stats_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Handle<v8::Array>, 0);

		hr = _new(v0, vr);

		METHOD_OVER(2, 2);

		ARG(v8::Handle<v8::Array>, 0);
		ARG(v8::Handle<v8::Array>, 1);

		hr = _new(v0, v1, vr);

		CONSTRUCT_RETURN();
	}

	inline v8::Handle<v8::Value> Stats_base::s_inc(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stats_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->inc(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stats_base::s_dec(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stats_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->dec(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stats_base::s_add(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stats_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG(int32_t, 1);

		hr = pInst->add(v0, v1);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> Stats_base::s_reset(const v8::Arguments& args)
	{
		METHOD_INSTANCE(Stats_base);
		METHOD_ENTER(0, 0);

		hr = pInst->reset();

		METHOD_VOID();
	}

}

#endif

