/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _crypto_base_H_
#define _crypto_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Cipher_base;
class PKey_base;
class X509Cert_base;
class X509Crl_base;
class Buffer_base;

class crypto_base : public module_base
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
		_GCM = 7,
		_PKCS7 = 0,
		_ONE_AND_ZEROS = 1,
		_ZEROS_AND_LEN = 2,
		_ZEROS = 3,
		_NOPADDING = 4
	};

public:
	// crypto_base
	static result_t randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);
	static result_t pseudoRandomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, exlib::AsyncEvent* ac);

	DECLARE_CLASSINFO(crypto_base);

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
	static void s_get_PKCS7(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ONE_AND_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ZEROS_AND_LEN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_NOPADDING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
	ASYNC_STATICVALUE2(crypto_base, randomBytes, int32_t, obj_ptr<Buffer_base>);
	ASYNC_STATICVALUE2(crypto_base, pseudoRandomBytes, int32_t, obj_ptr<Buffer_base>);
};

}

#include "Cipher.h"
#include "PKey.h"
#include "X509Cert.h"
#include "X509Crl.h"
#include "Buffer.h"

namespace fibjs
{
	inline ClassInfo& crypto_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"randomBytes", s_randomBytes, true},
			{"pseudoRandomBytes", s_pseudoRandomBytes, true}
		};

		static ClassData::ClassObject s_object[] = 
		{
			{"Cipher", Cipher_base::class_info},
			{"PKey", PKey_base::class_info},
			{"X509Cert", X509Cert_base::class_info},
			{"X509Crl", X509Crl_base::class_info}
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
			{"PKCS7", s_get_PKCS7, block_set, true},
			{"ONE_AND_ZEROS", s_get_ONE_AND_ZEROS, block_set, true},
			{"ZEROS_AND_LEN", s_get_ZEROS_AND_LEN, block_set, true},
			{"ZEROS", s_get_ZEROS, block_set, true},
			{"NOPADDING", s_get_NOPADDING, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"crypto", NULL, 
			2, s_method, 4, s_object, 20, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void crypto_base::s_get_AES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _AES;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_CAMELLIA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CAMELLIA;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_DES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DES;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_DES_EDE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DES_EDE;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_DES_EDE3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _DES_EDE3;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_BLOWFISH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _BLOWFISH;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_ARC4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ARC4;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _STREAM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_ECB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ECB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_CBC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CBC;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_CFB64(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CFB64;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_CFB128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CFB128;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_OFB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _OFB;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_CTR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _CTR;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_GCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _GCM;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_PKCS7(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _PKCS7;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_ONE_AND_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ONE_AND_ZEROS;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_ZEROS_AND_LEN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ZEROS_AND_LEN;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _ZEROS;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_get_NOPADDING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _NOPADDING;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void crypto_base::s_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = ac_randomBytes(v0, vr);

		METHOD_RETURN();
	}

	inline void crypto_base::s_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Buffer_base> vr;

		METHOD_ENTER(1, 1);

		ARG(int32_t, 0);

		hr = ac_pseudoRandomBytes(v0, vr);

		METHOD_RETURN();
	}

}

#endif

