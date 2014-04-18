/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _X509Req_base_H_
#define _X509Req_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class PKey_base;
class hash_base;

class X509Req_base : public object_base
{
public:
	// X509Req_base
	static result_t _new(obj_ptr<X509Req_base>& retVal);
	virtual result_t load(Buffer_base* derReq) = 0;
	virtual result_t load(const char* pemReq) = 0;
	virtual result_t loadFile(const char* filename) = 0;
	virtual result_t exportPem(std::string& retVal) = 0;
	virtual result_t exportDer(obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t create(const char* subject, PKey_base* key, int32_t hash) = 0;

	DECLARE_CLASSINFO(X509Req_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_create(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "PKey.h"
#include "hash.h"

namespace fibjs
{
	inline ClassInfo& X509Req_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"load", s_load},
			{"loadFile", s_loadFile},
			{"exportPem", s_exportPem},
			{"exportDer", s_exportDer},
			{"create", s_create}
		};

		static ClassData s_cd = 
		{ 
			"X509Req", s__new, 
			5, s_method, 0, NULL, 0, NULL, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}


	inline void X509Req_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<X509Req_base> vr;

		CONSTRUCT_ENTER(0, 0);

		hr = _new(vr);

		CONSTRUCT_RETURN();
	}

	inline void X509Req_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->load(v0);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = pInst->load(v0);

		METHOD_VOID();
	}

	inline void X509Req_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(1, 1);

		ARG_String(0);

		hr = pInst->loadFile(v0);

		METHOD_VOID();
	}

	inline void X509Req_base::s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		std::string vr;

		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(0, 0);

		hr = pInst->exportPem(vr);

		METHOD_RETURN();
	}

	inline void X509Req_base::s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(0, 0);

		hr = pInst->exportDer(vr);

		METHOD_RETURN();
	}

	inline void X509Req_base::s_create(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(X509Req_base);
		METHOD_ENTER(3, 2);

		ARG_String(0);
		ARG(obj_ptr<PKey_base>, 1);
		OPT_ARG(int32_t, 2, hash_base::_SHA1);

		hr = pInst->create(v0, v1, v2);

		METHOD_VOID();
	}

}

#endif

