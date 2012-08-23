/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpCollection_base_H_
#define _HttpCollection_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HttpCollection_base : public object_base
{
public:
	// HttpCollection_base
	virtual result_t clear() = 0;
	virtual result_t has(const char* name, bool& retVal) = 0;
	virtual result_t first(const char* name, std::string& retVal) = 0;
	virtual result_t all(const char* name, v8::Handle<v8::Array>& retVal) = 0;
	virtual result_t add(const char* name, const char* value) = 0;
	virtual result_t add(v8::Handle<v8::Object> map) = 0;
	virtual result_t set(const char* name, const char* value) = 0;
	virtual result_t set(v8::Handle<v8::Object> map) = 0;
	virtual result_t remove(const char* name) = 0;
	virtual result_t _named_getter(const char* property, std::string& retVal) = 0;
	virtual result_t _named_setter(const char* property, const char* newVal) = 0;

	DECLARE_CLASSINFO(HttpCollection_base);

public:
	static v8::Handle<v8::Value> s_clear(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_has(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_first(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_all(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_add(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_set(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_remove(const v8::Arguments& args);
	static v8::Handle<v8::Value> i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info);
	static v8::Handle<v8::Value> i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
};

}

namespace fibjs
{
	inline ClassInfo& HttpCollection_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"clear", s_clear},
			{"has", s_has},
			{"first", s_first},
			{"all", s_all},
			{"add", s_add},
			{"set", s_set},
			{"remove", s_remove}
		};

		static ClassData::ClassNamed s_named = 
		{
			i_NamedGetter, i_NamedSetter
		};

		static ClassData s_cd = 
		{ 
			"HttpCollection", NULL, 
			7, s_method, 0, NULL, 0, NULL, NULL, &s_named,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline v8::Handle<v8::Value> HttpCollection_base::i_NamedGetter(v8::Local<v8::String> property, const v8::AccessorInfo& info)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpCollection_base);

		hr = pInst->_named_getter(*v8::String::Utf8Value(property), vr);
		if(hr == CALL_RETURN_NULL)return v8::Handle<v8::Value>();

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpCollection_base);

		PROPERTY_VAL_String();
		hr = pInst->_named_setter(*v8::String::Utf8Value(property), v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_clear(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_has(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->has(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_first(const v8::Arguments& args)
	{
		std::string vr;

		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->first(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_all(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->all(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_add(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = pInst->add(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->add(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_set(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(2, 2);

		ARG_String(0);
		ARG_String(1);

		hr = pInst->set(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Object>, 0);

		hr = pInst->set(v0);

		METHOD_VOID();
	}

	inline v8::Handle<v8::Value> HttpCollection_base::s_remove(const v8::Arguments& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->remove(v0);

		METHOD_VOID();
	}

}

#endif

