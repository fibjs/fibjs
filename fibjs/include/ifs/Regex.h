/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Regex_base_H_
#define _Regex_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Regex_base : public object_base
{
public:
	// Regex_base
	virtual result_t exec(const char* str, v8::Local<v8::Array>& retVal) = 0;
	virtual result_t test(const char* str, bool& retVal) = 0;
	virtual result_t get_lastIndex(int32_t& retVal) = 0;
	virtual result_t set_lastIndex(int32_t newVal) = 0;
	virtual result_t get_global(bool& retVal) = 0;
	virtual result_t get_ignoreCase(bool& retVal) = 0;
	virtual result_t get_multiline(bool& retVal) = 0;

	DECLARE_CLASSINFO(Regex_base);

public:
	static void s_exec(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_test(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_lastIndex(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_lastIndex(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_global(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ignoreCase(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_multiline(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
};

}

namespace fibjs
{
	inline ClassInfo& Regex_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"exec", s_exec},
			{"test", s_test}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"lastIndex", s_get_lastIndex, s_set_lastIndex},
			{"global", s_get_global, block_set},
			{"ignoreCase", s_get_ignoreCase, block_set},
			{"multiline", s_get_multiline, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Regex", NULL, 
			2, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Regex_base::s_get_lastIndex(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_lastIndex(vr);

		METHOD_RETURN();
	}

	inline void Regex_base::s_set_lastIndex(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_lastIndex(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void Regex_base::s_get_global(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_global(vr);

		METHOD_RETURN();
	}

	inline void Regex_base::s_get_ignoreCase(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_ignoreCase(vr);

		METHOD_RETURN();
	}

	inline void Regex_base::s_get_multiline(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_multiline(vr);

		METHOD_RETURN();
	}

	inline void Regex_base::s_exec(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Array> vr;

		METHOD_INSTANCE(Regex_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->exec(v0, vr);

		METHOD_RETURN();
	}

	inline void Regex_base::s_test(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(Regex_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->test(v0, vr);

		METHOD_RETURN();
	}

}

#endif

