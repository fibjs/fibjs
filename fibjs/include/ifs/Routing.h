/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Routing_base_H_
#define _Routing_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;

class Routing_base : public Handler_base
{
public:
	// Routing_base
	static result_t _new(v8::Local<v8::Object> map, obj_ptr<Routing_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
	virtual result_t append(v8::Local<v8::Object> map) = 0;
	virtual result_t append(const char* pattern, v8::Local<v8::Value> hdlr) = 0;

	DECLARE_CLASSINFO(Routing_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& Routing_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"append", s_append}
		};

		static ClassData s_cd = 
		{ 
			"Routing", s__new, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Routing_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Routing_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = _new(v0, vr, args.This());

		CONSTRUCT_RETURN();
	}

	inline void Routing_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Routing_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Local<v8::Object>, 0);

		hr = pInst->append(v0);

		METHOD_OVER(2, 2);

		ARG(arg_string, 0);
		ARG(v8::Local<v8::Value>, 1);

		hr = pInst->append(v0, v1);

		METHOD_VOID();
	}

}

#endif

