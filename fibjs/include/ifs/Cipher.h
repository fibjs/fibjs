/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _Cipher_base_H_
#define _Cipher_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;

class Cipher_base : public object_base
{
public:
	// Cipher_base
	static result_t _new(int32_t provider, Buffer_base* key, obj_ptr<Cipher_base>& retVal);
	static result_t _new(int32_t provider, int32_t mode, Buffer_base* key, obj_ptr<Cipher_base>& retVal);
	static result_t _new(int32_t provider, int32_t mode, Buffer_base* key, Buffer_base* iv, obj_ptr<Cipher_base>& retVal);
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t get_keySize(int32_t& retVal) = 0;
	virtual result_t get_ivSize(int32_t& retVal) = 0;
	virtual result_t get_blockSize(int32_t& retVal) = 0;
	virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;
	virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(Cipher_base);

public:
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ivSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_blockSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBERVALUE2(Cipher_base, encrypt, Buffer_base*, obj_ptr<Buffer_base>);
	ASYNC_MEMBERVALUE2(Cipher_base, decrypt, Buffer_base*, obj_ptr<Buffer_base>);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Cipher_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"encrypt", s_encrypt},
			{"decrypt", s_decrypt}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"name", s_get_name, block_set},
			{"keySize", s_get_keySize, block_set},
			{"ivSize", s_get_ivSize, block_set},
			{"blockSize", s_get_blockSize, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Cipher", s__new, 
			2, s_method, 0, NULL, 4, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Cipher_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Cipher_base);

		hr = pInst->get_name(vr);

		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Cipher_base);

		hr = pInst->get_keySize(vr);

		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_ivSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Cipher_base);

		hr = pInst->get_ivSize(vr);

		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_blockSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Cipher_base);

		hr = pInst->get_blockSize(vr);

		METHOD_RETURN();
	}

	inline void Cipher_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Cipher_base> vr;

		CONSTRUCT_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = _new(v0, v1, vr);

		METHOD_OVER(3, 3);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(obj_ptr<Buffer_base>, 2);

		hr = _new(v0, v1, v2, vr);

		METHOD_OVER(4, 4);

		ARG(int32_t, 0);
		ARG(int32_t, 1);
		ARG(obj_ptr<Buffer_base>, 2);
		ARG(obj_ptr<Buffer_base>, 3);

		hr = _new(v0, v1, v2, v3, vr);

		CONSTRUCT_RETURN();
	}

	inline void Cipher_base::s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Cipher_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_encrypt(v0, vr);

		METHOD_RETURN();
	}

	inline void Cipher_base::s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_INSTANCE(Cipher_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = pInst->ac_decrypt(v0, vr);

		METHOD_RETURN();
	}

}

#endif

