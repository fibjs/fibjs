/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Integer64_base_H_
#define _Integer64_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Integer64_base : public object_base
{
public:
	// Integer64_base
	virtual result_t get_hi(int64_t& retVal) = 0;
	virtual result_t set_hi(int64_t newVal) = 0;
	virtual result_t get_lo(int64_t& retVal) = 0;
	virtual result_t set_lo(int64_t newVal) = 0;
	virtual result_t fromHex(const char* hexStr) = 0;
	virtual result_t fromString(const char* numStr, int32_t base) = 0;
	virtual result_t toString(int32_t base, std::string& retVal) = 0;
	virtual result_t valueOf(double& retVal) = 0;
	virtual result_t toJSON(const char* key, v8::Local<v8::Value>& retVal) = 0;

	DECLARE_CLASSINFO(Integer64_base);

public:
	static void s_get_hi(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_hi(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_get_lo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_set_lo(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
	static void s_fromHex(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_fromString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_toString(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_valueOf(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
	inline ClassInfo& Integer64_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"fromHex", s_fromHex},
			{"fromString", s_fromString},
			{"toString", s_toString},
			{"valueOf", s_valueOf},
			{"toJSON", s_toJSON}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"hi", s_get_hi, s_set_hi},
			{"lo", s_get_lo, s_set_lo}
		};

		static ClassData s_cd = 
		{ 
			"Integer64", NULL, 
			5, s_method, 0, NULL, 2, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Integer64_base::s_get_hi(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Integer64_base);

		hr = pInst->get_hi(vr);

		METHOD_RETURN();
	}

	inline void Integer64_base::s_set_hi(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Integer64_base);

		PROPERTY_VAL(int64_t);
		hr = pInst->set_hi(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void Integer64_base::s_get_lo(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int64_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Integer64_base);

		hr = pInst->get_lo(vr);

		METHOD_RETURN();
	}

	inline void Integer64_base::s_set_lo(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
	{
		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Integer64_base);

		PROPERTY_VAL(int64_t);
		hr = pInst->set_lo(v0);

		PROPERTY_SET_LEAVE();
	}

	inline void Integer64_base::s_fromHex(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Integer64_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(arg_string, 0, "");

		hr = pInst->fromHex(v0);

		METHOD_VOID();
	}

	inline void Integer64_base::s_fromString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Integer64_base);
		METHOD_ENTER(2, 0);

		OPT_ARG(arg_string, 0, "");
		OPT_ARG(int32_t, 1, 64);

		hr = pInst->fromString(v0, v1);

		METHOD_VOID();
	}

	inline void Integer64_base::s_toString(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(Integer64_base);
		METHOD_ENTER(1, 0);

		OPT_ARG(int32_t, 0, 64);

		hr = pInst->toString(v0, vr);

		METHOD_RETURN();
	}

	inline void Integer64_base::s_valueOf(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		double vr;

		METHOD_INSTANCE(Integer64_base);
		METHOD_ENTER(0, 0);

		hr = pInst->valueOf(vr);

		METHOD_RETURN();
	}

	inline void Integer64_base::s_toJSON(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Integer64_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->toJSON(v0, vr);

		METHOD_RETURN();
	}

}

#endif

