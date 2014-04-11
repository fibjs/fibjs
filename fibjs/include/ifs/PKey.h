/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _PKey_base_H_
#define _PKey_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class PKey_base : public object_base
{
public:
	// PKey_base
	static result_t _new(obj_ptr<PKey_base>& retVal);
	virtual result_t genRsaKey(int32_t size, exlib::AsyncEvent* ac) = 0;
	virtual result_t genEcKey(int32_t size, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(PKey_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(PKey_base, genRsaKey, int32_t);
	ASYNC_MEMBER1(PKey_base, genEcKey, int32_t);
};

}

namespace fibjs
{
	inline ClassInfo& PKey_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"genRsaKey", s_genRsaKey},
			{"genEcKey", s_genEcKey}
		};

		static ClassData s_cd = 
		{ 
			"PKey", s__new, 
			2, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void PKey_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<PKey_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline void PKey_base::s_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_genRsaKey(v0);

		METHOD_VOID();
	}

	inline void PKey_base::s_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = pInst->ac_genEcKey(v0);

		METHOD_VOID();
	}

}

#endif

