/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Redis_base_H_
#define _Redis_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Redis_base : public object_base
{
public:
	// Redis_base
	virtual result_t command(const char* cmd, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal) = 0;
	virtual result_t close(exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(Redis_base);

public:
	static void s_command(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER0(Redis_base, close);
};

}

namespace fibjs
{
	inline ClassInfo& Redis_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"command", s_command},
			{"close", s_close}
		};

		static ClassData s_cd = 
		{ 
			"Redis", NULL, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Redis_base::s_command(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Local<v8::Value> vr;

		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(-1, 1);

		ARG(arg_string, 0);

		hr = pInst->command(v0, args, vr);

		METHOD_RETURN();
	}

	inline void Redis_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Redis_base);
		METHOD_ENTER(0, 0);

		hr = pInst->ac_close();

		METHOD_VOID();
	}

}

#endif

