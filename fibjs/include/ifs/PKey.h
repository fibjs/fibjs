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

class Buffer_base;

class PKey_base : public object_base
{
public:
	// PKey_base
	static result_t _new(obj_ptr<PKey_base>& retVal);
	virtual result_t genRsaKey(int32_t size, exlib::AsyncEvent* ac) = 0;
	virtual result_t genEcKey(const char* curve, exlib::AsyncEvent* ac) = 0;
	virtual result_t import(Buffer_base* DerKey, const char* password) = 0;
	virtual result_t import(const char* pemKey, const char* password) = 0;
	virtual result_t exportPem(std::string& retVal) = 0;
	virtual result_t exportDer(obj_ptr<Buffer_base>& retVal) = 0;

	DECLARE_CLASSINFO(PKey_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_import(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(PKey_base, genRsaKey, int32_t);
	ASYNC_MEMBER1(PKey_base, genEcKey, const char*);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& PKey_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"genRsaKey", s_genRsaKey},
			{"genEcKey", s_genEcKey},
			{"import", s_import},
			{"exportPem", s_exportPem},
			{"exportDer", s_exportDer}
		};

		static ClassData s_cd = 
		{ 
			"PKey", s__new, 
			5, s_method, 0, NULL, 0, NULL, NULL, NULL,
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
		METHOD_ENTER(1, 0);

		OPT_ARG_String(0, "secp521r1");

		hr = pInst->ac_genEcKey(v0);

		METHOD_VOID();
	}

	inline void PKey_base::s_import(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Buffer_base>, 0);
		OPT_ARG_String(1, "");

		hr = pInst->import(v0, v1);

		METHOD_OVER(2, 1);

		ARG_String(0);
		OPT_ARG_String(1, "");

		hr = pInst->import(v0, v1);

		METHOD_VOID();
	}

	inline void PKey_base::s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(0, 0);

		hr = pInst->exportPem(vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(0, 0);

		hr = pInst->exportDer(vr);

		METHOD_RETURN();
	}

}

#endif

