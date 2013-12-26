/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Chain_base_H_
#define _Chain_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Handler.h"

namespace fibjs
{

class Handler_base;

class Chain_base : public Handler_base
{
public:
	// Chain_base
	virtual result_t append(v8::Handle<v8::Array> hdlrs) = 0;
	virtual result_t append(v8::Handle<v8::Value> hdlr) = 0;

	DECLARE_CLASSINFO(Chain_base);

public:
	static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& Chain_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"append", s_append}
		};

		static ClassData s_cd = 
		{ 
			"Chain", NULL, 
			1, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Handler_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void Chain_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Chain_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Array>, 0);

		hr = pInst->append(v0);

		METHOD_OVER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->append(v0);

		METHOD_VOID();
	}

}

#endif

