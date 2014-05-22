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
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t get_keySize(int32_t& retVal) = 0;
	virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;
	virtual result_t genRsaKey(int32_t size, exlib::AsyncEvent* ac) = 0;
	virtual result_t genEcKey(const char* curve, exlib::AsyncEvent* ac) = 0;
	virtual result_t isPrivate(bool& retVal) = 0;
	virtual result_t clone(obj_ptr<PKey_base>& retVal) = 0;
	virtual result_t importKey(Buffer_base* DerKey, const char* password) = 0;
	virtual result_t importKey(const char* pemKey, const char* password) = 0;
	virtual result_t exportPem(std::string& retVal) = 0;
	virtual result_t exportDer(obj_ptr<Buffer_base>& retVal) = 0;
	virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t sign(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t verify(Buffer_base* sign, Buffer_base* data, bool& retVal, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(PKey_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_isPrivate(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_importKey(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(PKey_base, genRsaKey, int32_t);
	ASYNC_MEMBER1(PKey_base, genEcKey, const char*);
	ASYNC_MEMBERVALUE2(PKey_base, encrypt, Buffer_base*, obj_ptr<Buffer_base>);
	ASYNC_MEMBERVALUE2(PKey_base, decrypt, Buffer_base*, obj_ptr<Buffer_base>);
	ASYNC_MEMBERVALUE2(PKey_base, sign, Buffer_base*, obj_ptr<Buffer_base>);
	ASYNC_MEMBERVALUE3(PKey_base, verify, Buffer_base*, Buffer_base*, bool);
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
			{"isPrivate", s_isPrivate},
			{"clone", s_clone},
			{"importKey", s_importKey},
			{"exportPem", s_exportPem},
			{"exportDer", s_exportDer},
			{"encrypt", s_encrypt},
			{"decrypt", s_decrypt},
			{"sign", s_sign},
			{"verify", s_verify}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"name", s_get_name, block_set},
			{"keySize", s_get_keySize, block_set},
			{"publicKey", s_get_publicKey, block_set}
		};

		static ClassData s_cd = 
		{ 
			"PKey", s__new, 
			11, s_method, 0, NULL, 3, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void PKey_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PKey_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PKey_base);

		hr = pInst->get_keySize(vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		obj_ptr<PKey_base> vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(PKey_base);

		hr = pInst->get_publicKey(vr);

		METHOD_RETURN();
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

		OPT_ARG(arg_string, 0, "secp521r1");

		hr = pInst->ac_genEcKey(v0);

		METHOD_VOID();
	}

	inline void PKey_base::s_isPrivate(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(0, 0);

		hr = pInst->isPrivate(vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<PKey_base> vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(0, 0);

		hr = pInst->clone(vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_importKey(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(2, 1);

		ARG(obj_ptr<Buffer_base>, 0);
		OPT_ARG(arg_string, 1, "");

		hr = pInst->importKey(v0, v1);

		METHOD_OVER(2, 1);

		ARG(arg_string, 0);
		OPT_ARG(arg_string, 1, "");

		hr = pInst->importKey(v0, v1);

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

	inline void PKey_base::s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_encrypt(v0, vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_decrypt(v0, vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_sign(v0, vr);

		METHOD_RETURN();
	}

	inline void PKey_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		bool vr;

		METHOD_INSTANCE(PKey_base);
		METHOD_ENTER(2, 2);

		ARG(obj_ptr<Buffer_base>, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = pInst->ac_verify(v0, v1, vr);

		METHOD_RETURN();
	}

}

#endif

