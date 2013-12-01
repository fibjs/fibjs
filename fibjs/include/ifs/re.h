/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _re_base_H_
#define _re_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Regex_base;

class re_base : public module_base
{
public:
	// re_base
	static result_t compile(const char* pattern, const char* opt, obj_ptr<Regex_base>& retVal);

	DECLARE_CLASSINFO(re_base);

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
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void re_base::s_compile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Regex_base> vr;

		METHOD_ENTER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "");

		hr = compile(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

