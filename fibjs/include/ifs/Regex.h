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
	virtual result_t exec(const char* str, v8::Handle<v8::Array>& retVal) = 0;
	virtual result_t test(const char* str, bool& retVal) = 0;
	virtual result_t get_lastIndex(int32_t& retVal) = 0;
	virtual result_t set_lastIndex(int32_t newVal) = 0;
	virtual result_t get_global(bool& retVal) = 0;
	virtual result_t get_ignoreCase(bool& retVal) = 0;
	virtual result_t get_multiline(bool& retVal) = 0;

	DECLARE_CLASSINFO(Regex_base);

	virtual result_t toJSON(const char* key, v8::Handle<v8::Object>& retVal)
	{
		result_t hr = object_base::toJSON(key, retVal);
		if(hr < 0)return hr;

		CLONE(lastIndex, int32_t);
		CLONE(global, bool);
		CLONE(ignoreCase, bool);
		CLONE(multiline, bool);

		return 0;
	}

public:
	static v8::Handle<v8::Value> s_exec(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_test(const v8::Arguments& args);
	static v8::Handle<v8::Value> s_get_lastIndex(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static void s_set_lastIndex(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_global(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_ignoreCase(v8::Local<v8::String> property, const v8::AccessorInfo &info);
	static v8::Handle<v8::Value> s_get_multiline(v8::Local<v8::String> property, const v8::AccessorInfo &info);
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
			{"global", s_get_global},
			{"ignoreCase", s_get_ignoreCase},
			{"multiline", s_get_multiline}
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

	inline v8::Handle<v8::Value> Regex_base::s_get_lastIndex(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_lastIndex(vr);

		METHOD_RETURN();
	}

	inline void Regex_base::s_set_lastIndex(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo &info)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		PROPERTY_VAL(int32_t);
		hr = pInst->set_lastIndex(v0);

		PROPERTY_SET_LEAVE();
	}

	inline v8::Handle<v8::Value> Regex_base::s_get_global(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_global(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Regex_base::s_get_ignoreCase(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_ignoreCase(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Regex_base::s_get_multiline(v8::Local<v8::String> property, const v8::AccessorInfo &info)
	{
		bool vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Regex_base);

		hr = pInst->get_multiline(vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Regex_base::s_exec(const v8::Arguments& args)
	{
		v8::Handle<v8::Array> vr;

		METHOD_INSTANCE(Regex_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->exec(v0, vr);

		METHOD_RETURN();
	}

	inline v8::Handle<v8::Value> Regex_base::s_test(const v8::Arguments& args)
	{
		bool vr;

		METHOD_INSTANCE(Regex_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->test(v0, vr);

		METHOD_RETURN();
	}

}

#endif

