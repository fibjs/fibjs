/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _re_base_H_
#define _re_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Regex_base;

class re_base : public object_base
{
	DECLARE_CLASS(re_base);

public:
	// re_base
	static result_t compile(const char* pattern, const char* opt, obj_ptr<Regex_base>& retVal);

public:
	static void s_compile(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Regex.h"

namespace fibjs
{
	inline ClassInfo& re_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"compile", s_compile, true}
		};

		static ClassData s_cd = 
		{ 
			"re", NULL, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			NULL
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void re_base::s_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Regex_base> vr;

		METHOD_ENTER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(arg_string, 1, "");

		hr = compile(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

