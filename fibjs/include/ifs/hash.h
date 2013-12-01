/***************************************************************************
 *   Copyright (C) 2012 by Leo Hoo                                         *
 *   lion@9465.net                                                         *
 *                                                                         *
 ***************************************************************************/

#ifndef _hash_base_H_
#define _hash_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "module.h"

namespace fibjs
{

class module_base;
class Buffer_base;
class Digest_base;

class hash_base : public module_base
{
public:
	enum{
		_MD2 = 0,
		_MD4 = 1,
		_MD5 = 2,
		_RIPEMD128 = 3,
		_RIPEMD160 = 4,
		_RIPEMD256 = 5,
		_RIPEMD320 = 6,
		_SHA1 = 7,
		_SHA224 = 8,
		_SHA256 = 9,
		_SHA384 = 10,
		_SHA512 = 11
	};

public:
	// hash_base
	static result_t digest(int32_t algo, Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t digest(int32_t algo, const char* text, obj_ptr<Digest_base>& retVal);
	static result_t md2(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t md2(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t md4(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t md4(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t md5(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t md5(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t sha1(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha1(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t sha224(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha224(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t sha256(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha256(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t sha384(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha384(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t sha512(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha512(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t ripemd128(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t ripemd128(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t ripemd160(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t ripemd160(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t ripemd256(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t ripemd256(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t ripemd320(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t ripemd320(const char* text, obj_ptr<Digest_base>& retVal);
	static result_t hmac(int32_t algo, Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac(int32_t algo, const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md2(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md2(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md4(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md4(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md5(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md5(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha1(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha1(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha224(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha224(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha256(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha256(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha384(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha384(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha512(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha512(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd128(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd128(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd160(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd160(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd256(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd256(const char* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd320(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd320(const char* key, obj_ptr<Digest_base>& retVal);

	DECLARE_CLASSINFO(hash_base);

public:
	static void s_get_MD2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_MD4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_MD5(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_RIPEMD128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_RIPEMD160(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_RIPEMD256(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_RIPEMD320(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA224(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA256(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA384(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA512(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_digest(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md2(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md4(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ripemd128(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ripemd256(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ripemd320(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_md2(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_md4(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_ripemd128(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_ripemd256(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_ripemd320(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "Digest.h"

namespace fibjs
{
	inline ClassInfo& hash_base::class_info()
	{
		static ClassData::ClassMethod s_method[] = 
		{
			{"digest", s_digest, true},
			{"md2", s_md2, true},
			{"md4", s_md4, true},
			{"md5", s_md5, true},
			{"sha1", s_sha1, true},
			{"sha224", s_sha224, true},
			{"sha256", s_sha256, true},
			{"sha384", s_sha384, true},
			{"sha512", s_sha512, true},
			{"ripemd128", s_ripemd128, true},
			{"ripemd160", s_ripemd160, true},
			{"ripemd256", s_ripemd256, true},
			{"ripemd320", s_ripemd320, true},
			{"hmac", s_hmac, true},
			{"hmac_md2", s_hmac_md2, true},
			{"hmac_md4", s_hmac_md4, true},
			{"hmac_md5", s_hmac_md5, true},
			{"hmac_sha1", s_hmac_sha1, true},
			{"hmac_sha224", s_hmac_sha224, true},
			{"hmac_sha256", s_hmac_sha256, true},
			{"hmac_sha384", s_hmac_sha384, true},
			{"hmac_sha512", s_hmac_sha512, true},
			{"hmac_ripemd128", s_hmac_ripemd128, true},
			{"hmac_ripemd160", s_hmac_ripemd160, true},
			{"hmac_ripemd256", s_hmac_ripemd256, true},
			{"hmac_ripemd320", s_hmac_ripemd320, true}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"MD2", s_get_MD2, block_set, true},
			{"MD4", s_get_MD4, block_set, true},
			{"MD5", s_get_MD5, block_set, true},
			{"RIPEMD128", s_get_RIPEMD128, block_set, true},
			{"RIPEMD160", s_get_RIPEMD160, block_set, true},
			{"RIPEMD256", s_get_RIPEMD256, block_set, true},
			{"RIPEMD320", s_get_RIPEMD320, block_set, true},
			{"SHA1", s_get_SHA1, block_set, true},
			{"SHA224", s_get_SHA224, block_set, true},
			{"SHA256", s_get_SHA256, block_set, true},
			{"SHA384", s_get_SHA384, block_set, true},
			{"SHA512", s_get_SHA512, block_set, true}
		};

		static ClassData s_cd = 
		{ 
			"hash", NULL, 
			26, s_method, 0, NULL, 12, s_property, NULL, NULL,
			&module_base::class_info()
		};

		static ClassInfo s_ci(s_cd);
		return s_ci;
	}

	inline void hash_base::s_get_MD2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _MD2;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_MD4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _MD4;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_MD5(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _MD5;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_RIPEMD128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _RIPEMD128;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_RIPEMD160(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _RIPEMD160;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_RIPEMD256(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _RIPEMD256;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_RIPEMD320(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _RIPEMD320;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_SHA1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SHA1;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_SHA224(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SHA224;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_SHA256(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SHA256;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_SHA384(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SHA384;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_get_SHA512(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _SHA512;
		PROPERTY_ENTER();
		METHOD_RETURN();
	}

	inline void hash_base::s_digest(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = digest(v0, v1, vr);

		METHOD_OVER(2, 1);

		ARG(int32_t, 0);
		OPT_ARG_String(1, "");

		hr = digest(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_md2(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = md2(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = md2(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_md4(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = md4(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = md4(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = md5(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = md5(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha1(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = sha1(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha224(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = sha224(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha256(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = sha256(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha384(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = sha384(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha512(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = sha512(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_ripemd128(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ripemd128(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = ripemd128(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ripemd160(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = ripemd160(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_ripemd256(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ripemd256(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = ripemd256(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_ripemd320(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ripemd320(v0, vr);

		METHOD_OVER(1, 0);

		OPT_ARG_String(0, "");

		hr = ripemd320(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = hmac(v0, v1, vr);

		METHOD_OVER(2, 2);

		ARG(int32_t, 0);
		ARG_String(1);

		hr = hmac(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_md2(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_md2(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_md2(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_md4(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_md4(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_md4(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_md5(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_md5(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha1(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_sha1(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha224(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_sha224(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha256(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_sha256(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha384(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_sha384(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha512(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_sha512(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_ripemd128(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_ripemd128(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_ripemd128(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_ripemd160(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_ripemd160(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_ripemd256(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_ripemd256(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_ripemd256(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_ripemd320(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_ripemd320(v0, vr);

		METHOD_OVER(1, 1);

		ARG_String(0);

		hr = hmac_ripemd320(v0, vr);

		METHOD_RETURN();
	}

}

#endif

