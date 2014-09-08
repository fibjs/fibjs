/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
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

class List_base;

class HttpCollection_base : public object_base
{
	DECLARE_CLASS(HttpCollection_base);

public:
	// HttpCollection_base
	virtual result_t clear() = 0;
	virtual result_t has(const char* name, bool& retVal) = 0;
	virtual result_t first(const char* name, Variant& retVal) = 0;
	virtual result_t all(const char* name, obj_ptr<List_base>& retVal) = 0;
	virtual result_t add(v8::Local<v8::Object> map) = 0;
	virtual result_t add(const char* name, Variant value) = 0;
	virtual result_t set(v8::Local<v8::Object> map) = 0;
	virtual result_t set(const char* name, Variant value) = 0;
	virtual result_t remove(const char* name) = 0;
	virtual result_t _named_getter(const char* property, Variant& retVal) = 0;
	virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;
	virtual result_t _named_setter(const char* property, Variant newVal) = 0;
	virtual result_t _named_deleter(const char* property, v8::Local<v8::Boolean>& retVal) = 0;

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_first(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_all(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args);
	static void i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean> &args);
};

}

#include "List.h"

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
			i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
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

	inline void HttpCollection_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		Variant vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpCollection_base);

		v8::String::Utf8Value k(property);
		if(class_info().has(*k))return;

		hr = pInst->_named_getter(*k, vr);
		if(hr == CALL_RETURN_NULL)return;

		METHOD_RETURN();
	}

	inline void HttpCollection_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args)
	{
		v8::Local<v8::Array> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpCollection_base);

		hr = pInst->_named_enumerator(vr);

		METHOD_RETURN1();
	}

	inline void HttpCollection_base::i_NamedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpCollection_base);

		PROPERTY_VAL(Variant);
		v8::String::Utf8Value k(property);
		if(class_info().has(*k))return;

		hr = pInst->_named_setter(*k, v0);

		METHOD_VOID();
	}

	inline void HttpCollection_base::i_NamedDeleter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Boolean> &args)
	{
		v8::Local<v8::Boolean> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(HttpCollection_base);

		v8::String::Utf8Value k(property);
		if(class_info().has(*k)){args.GetReturnValue().Set(v8::False(isolate));return;}

		hr = pInst->_named_deleter(*k, vr);
		METHOD_RETURN1();
	}

	inline void HttpCollection_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clear();

		METHOD_VOID();
	}

	inline void HttpCollection_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->has(v0, vr);

		METHOD_RETURN();
	}

	inline void HttpCollection_base::s_first(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		Variant vr;

		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->first(v0, vr);

		METHOD_RETURN();
	}

	inline void HttpCollection_base::s_all(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<List_base> vr;

		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->all(v0, vr);

		METHOD_RETURN();
	}

	inline void HttpCollection_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->add(v0);

		METHOD_OVER(2, 2);

		ARG(arg_string, 0);
		ARG(Variant, 1);

		hr = pInst->add(v0, v1);

		METHOD_VOID();
	}

	inline void HttpCollection_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->set(v0);

		METHOD_OVER(2, 2);

		ARG(arg_string, 0);
		ARG(Variant, 1);

		hr = pInst->set(v0, v1);

		METHOD_VOID();
	}

	inline void HttpCollection_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(HttpCollection_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->remove(v0);

		METHOD_VOID();
	}

}

#endif

