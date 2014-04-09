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
class object_base;

class Cipher_base : public object_base
{
public:
	enum{
		_AES = 1,
		_CAMELLIA = 2,
		_DES = 3,
		_DES_EDE = 4,
		_DES_EDE3 = 5,
		_BLOWFISH = 6,
		_ARC4 = 7,
		_STREAM = 0,
		_ECB = 1,
		_CBC = 2,
		_CFB64 = 3,
		_CFB128 = 4,
		_OFB = 5,
		_CTR = 6,
		_GCM = 7
	};

public:
	// Cipher_base
	static result_t _new(int32_t provider, Buffer_base* key, obj_ptr<Cipher_base>& retVal);
	static result_t _new(int32_t provider, Buffer_base* key, Buffer_base* iv, obj_ptr<Cipher_base>& retVal);
	static result_t _new(int32_t provider, int32_t mode, Buffer_base* key, obj_ptr<Cipher_base>& retVal);
	static result_t _new(int32_t provider, int32_t mode, Buffer_base* key, Buffer_base* iv, obj_ptr<Cipher_base>& retVal);
	virtual result_t get_name(std::string& retVal) = 0;
	virtual result_t cripto(object_base* v, exlib::AsyncEvent* ac) = 0;

	DECLARE_CLASSINFO(Cipher_base);

public:
	static void s_get_AES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CAMELLIA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_DES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_DES_EDE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_DES_EDE3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_BLOWFISH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ARC4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ECB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CBC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CFB64(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CFB128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_OFB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_CTR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_GCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_cripto(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_MEMBER1(Cipher_base, cripto, object_base*);
};

}

#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& Cipher_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"cripto", s_cripto}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"AES", s_get_AES, block_set, true},
			{"CAMELLIA", s_get_CAMELLIA, block_set, true},
			{"DES", s_get_DES, block_set, true},
			{"DES_EDE", s_get_DES_EDE, block_set, true},
			{"DES_EDE3", s_get_DES_EDE3, block_set, true},
			{"BLOWFISH", s_get_BLOWFISH, block_set, true},
			{"ARC4", s_get_ARC4, block_set, true},
			{"STREAM", s_get_STREAM, block_set, true},
			{"ECB", s_get_ECB, block_set, true},
			{"CBC", s_get_CBC, block_set, true},
			{"CFB64", s_get_CFB64, block_set, true},
			{"CFB128", s_get_CFB128, block_set, true},
			{"OFB", s_get_OFB, block_set, true},
			{"CTR", s_get_CTR, block_set, true},
			{"GCM", s_get_GCM, block_set, true},
			{"name", s_get_name, block_set}
		};

		static ClassData s_cd = 
		{ 
			"Cipher", s__new, 
			1, s_method, 0, NULL, 16, s_property, NULL, NULL,
			&object_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void Cipher_base::s_get_AES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _AES;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_CAMELLIA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CAMELLIA;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_DES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DES;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_DES_EDE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DES_EDE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_DES_EDE3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DES_EDE3;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_BLOWFISH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BLOWFISH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_ARC4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ARC4;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _STREAM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_ECB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ECB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_CBC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CBC;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_CFB64(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CFB64;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_CFB128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CFB128;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_OFB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _OFB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_CTR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CTR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_GCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _GCM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void Cipher_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		std::string vr;

		PROPERTY_ENTER();
		PROPERTY_INSTANCE(Cipher_base);

		hr = pInst->get_name(vr);

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
		ARG(obj_ptr<Buffer_base>, 1);
		ARG(obj_ptr<Buffer_base>, 2);

		hr = _new(v0, v1, v2, vr);

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

	inline void Cipher_base::s_cripto(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		METHOD_INSTANCE(Cipher_base);
		METHOD_ENTER(1, 1);

		ARG(obj_ptr<object_base>, 0);

		hr = pInst->ac_cripto(v0);

		METHOD_VOID();
	}

}

#endif

