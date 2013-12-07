/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _BlockQueue_base_H_
#define _BlockQueue_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Queue.h"

namespace fibjs
{

class Queue_base;

class BlockQueue_base : public Queue_base
{
public:
	// BlockQueue_base
	static result_t _new(int32_t size, obj_ptr<BlockQueue_base>& retVal);
	virtual result_t put(v8::Handle<v8::Value> e) = 0;
	virtual result_t take(v8::Handle<v8::Value>& retVal) = 0;

	DECLARE_CLASSINFO(BlockQueue_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_put(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_take(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
	inline ClassInfo& BlockQueue_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"put", s_put},
			{"take", s_take}
		};

		static ClassData s_cd = 
		{ 
			"BlockQueue", s__new, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&Queue_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void BlockQueue_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<BlockQueue_base> vr;

		CONSTRUCT_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = _new(v0, vr);

		CONSTRUCT_RETURN();
	}

	inline void BlockQueue_base::s_put(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(BlockQueue_base);
		METHOD_ENTER(1, 1);

		ARG(v8::Handle<v8::Value>, 0);

		hr = pInst->put(v0);

		METHOD_VOID();
	}

	inline void BlockQueue_base::s_take(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		v8::Handle<v8::Value> vr;

		METHOD_INSTANCE(BlockQueue_base);
		METHOD_ENTER(0, 0);

		hr = pInst->take(vr);

		METHOD_RETURN();
	}

}

#endif

