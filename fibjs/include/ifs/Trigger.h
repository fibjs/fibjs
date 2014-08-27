/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Trigger_base_H_
#define _Trigger_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Trigger_base : public object_base
{
public:
	// Trigger_base
	static result_t _new(obj_ptr<Trigger_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t on(const char* ev, v8::Local<v8::Function> func, int32_t& retVal) = 0;
	virtual result_t on(v8::Local<v8::Object> map, int32_t& retVal) = 0;
	virtual result_t once(const char* ev, v8::Local<v8::Function> func, int32_t& retVal) = 0;
	virtual result_t once(v8::Local<v8::Object> map, int32_t& retVal) = 0;
	virtual result_t off(const char* ev, v8::Local<v8::Function> func, int32_t& retVal) = 0;
	virtual result_t off(const char* ev, int32_t& retVal) = 0;
	virtual result_t off(v8::Local<v8::Object> map, int32_t& retVal) = 0;
	virtual result_t trigger(const char* ev, const v8::FunctionCallbackInfo<v8::Value>& args) = 0;

	DECLARE_CLASSINFO(Trigger_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_on(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_once(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_off(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_trigger(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
	inline ClassInfo& Trigger_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"on", s_on},
			{"once", s_once},
			{"off", s_off},
			{"trigger", s_trigger}
		};

		static ClassData s_cd = 
		{ 
			"Trigger", s__new, 
			4, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Trigger_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Trigger_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void Trigger_base::s_on(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = pInst->on(v0, v1, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->on(v0, vr);

		METHOD_RETURN();
	}

	inline void Trigger_base::s_once(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = pInst->once(v0, v1, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->once(v0, vr);

		METHOD_RETURN();
	}

	inline void Trigger_base::s_off(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		int32_t vr;

		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(v8::Local<v8::Function>, 1);

		hr = pInst->off(v0, v1, vr);

		METHOD_OVER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->off(v0, vr);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->off(v0, vr);

		METHOD_RETURN();
	}

	inline void Trigger_base::s_trigger(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Trigger_base);
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = pInst->trigger(v0, args);

		METHOD_VOID();
	}

}

#endif

