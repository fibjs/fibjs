/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _SandBox_base_H_
#define _SandBox_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class SandBox_base : public object_base
{
public:
	// SandBox_base
	static result_t _new(v8::Local<v8::Object> mods, const char* name, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	static result_t _new(v8::Local<v8::Object> mods, v8::Local<v8::Function> require, const char* name, obj_ptr<SandBox_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t add(const char* id, v8::Local<v8::Value> mod) = 0;
	virtual result_t add(v8::Local<v8::Object> mods) = 0;
	virtual result_t addScript(const char* srcname, const char* script, v8::Local<v8::Value>& retVal) = 0;
	virtual result_t remove(const char* id) = 0;
	virtual result_t run(const char* fname) = 0;
	virtual result_t require(const char* id, v8::Local<v8::Value>& retVal) = 0;

	DECLARE_CLASSINFO(SandBox_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_addScript(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_require(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
	inline ClassInfo& SandBox_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"add", s_add},
			{"addScript", s_addScript},
			{"remove", s_remove},
			{"run", s_run},
			{"require", s_require}
		};

		static ClassData s_cd = 
		{ 
			"SandBox", s__new, 
			5, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void SandBox_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<SandBox_base> vr;

		CONSTRUCT_ENTER(2, 1);

		ARG(v8::Local<v8::Object>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = _new(v0, v1, vr, args.This());

		METHOD_OVER(3, 2);

		ARG(v8::Local<v8::Object>, 0);
		ARG(v8::Local<v8::Function>, 1);
		OPT_ARG(arg_string, 2, "");

		hr = _new(v0, v1, v2, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void SandBox_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(v8::Local<v8::Value>, 1);

		hr = pInst->add(v0, v1);

		METHOD_OVER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->add(v0);

		METHOD_VOID();
	}

	inline void SandBox_base::s_addScript(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(2, 2);

		ARG(arg_string, 0);
		ARG(arg_string, 1);

		hr = pInst->addScript(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void SandBox_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->remove(v0);

		METHOD_VOID();
	}

	inline void SandBox_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->run(v0);

		METHOD_VOID();
	}

	inline void SandBox_base::s_require(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(SandBox_base);
		METHOD_ENTER(1, 1);

		ARG(arg_string, 0);

		hr = pInst->require(v0, vr);

		METHOD_RETURN();
	}

}

#endif

