/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Handler_base_H_
#define _Handler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class object_base;

class Handler_base : public object_base
{
	DECLARE_CLASS(Handler_base);

public:
	// Handler_base
	virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal, exlib::AsyncEvent* ac) = 0;

public:
	static void s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE2(Handler_base, invoke, object_base*, obj_ptr<Handler_base>);
};

}


namespace fibjs
{
	inline ClassInfo& Handler_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"invoke", s_invoke, false}
		};

		static ClassData s_cd = 
		{ 
			"Handler", NULL, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Handler_base::s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Handler_base> vr;

		METHOD_INSTANCE(Handler_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<object_base>, 0);

		hr = pInst->ac_invoke(v0, vr);

		METHOD_RETURN();
	}

}

#endif

