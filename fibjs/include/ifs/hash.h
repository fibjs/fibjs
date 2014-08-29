/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
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
		_MD2 = 1,
		_MD4 = 2,
		_MD5 = 3,
		_SHA1 = 4,
		_SHA224 = 5,
		_SHA256 = 6,
		_SHA384 = 7,
		_SHA512 = 8,
		_RIPEMD160 = 9
	};

public:
	// hash_base
	static result_t digest(int32_t algo, Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t digest(int32_t algo, obj_ptr<Digest_base>& retVal);
	static result_t md2(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t md4(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t md5(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha1(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha224(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha256(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha384(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t sha512(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t ripemd160(Buffer_base* data, obj_ptr<Digest_base>& retVal);
	static result_t hmac(int32_t algo, Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md2(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md4(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_md5(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha1(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha224(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha256(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha384(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_sha512(Buffer_base* key, obj_ptr<Digest_base>& retVal);
	static result_t hmac_ripemd160(Buffer_base* key, obj_ptr<Digest_base>& retVal);

	DECLARE_CLASSINFO(hash_base);

public:
	template<typename T>
	static void __new(const T &args){}

public:
	static void s_get_MD2(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_MD4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_MD5(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA1(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA224(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA256(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA384(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_SHA512(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_get_RIPEMD160(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
	static void s_digest(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md2(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md4(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_md2(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_md4(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
	static void s_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
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
			{"digest", s_digest},
			{"md2", s_md2},
			{"md4", s_md4},
			{"md5", s_md5},
			{"sha1", s_sha1},
			{"sha224", s_sha224},
			{"sha256", s_sha256},
			{"sha384", s_sha384},
			{"sha512", s_sha512},
			{"ripemd160", s_ripemd160},
			{"hmac", s_hmac},
			{"hmac_md2", s_hmac_md2},
			{"hmac_md4", s_hmac_md4},
			{"hmac_md5", s_hmac_md5},
			{"hmac_sha1", s_hmac_sha1},
			{"hmac_sha224", s_hmac_sha224},
			{"hmac_sha256", s_hmac_sha256},
			{"hmac_sha384", s_hmac_sha384},
			{"hmac_sha512", s_hmac_sha512},
			{"hmac_ripemd160", s_hmac_ripemd160}
		};

		static ClassData::ClassProperty s_property[] = 
		{
			{"MD2", s_get_MD2, block_set},
			{"MD4", s_get_MD4, block_set},
			{"MD5", s_get_MD5, block_set},
			{"SHA1", s_get_SHA1, block_set},
			{"SHA224", s_get_SHA224, block_set},
			{"SHA256", s_get_SHA256, block_set},
			{"SHA384", s_get_SHA384, block_set},
			{"SHA512", s_get_SHA512, block_set},
			{"RIPEMD160", s_get_RIPEMD160, block_set}
		};

		static ClassData s_cd = 
		{ 
			"hash", NULL, 
			20, s_method, 0, NULL, 9, s_property, NULL, NULL,
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

	inline void hash_base::s_get_RIPEMD160(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
	{
		int32_t vr = _RIPEMD160;
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

		METHOD_OVER(1, 1);

		ARG(int32_t, 0);

		hr = digest(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_md2(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = md2(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_md4(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = md4(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = md5(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha1(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha224(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha256(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha384(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = sha512(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = ripemd160(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(2, 2);

		ARG(int32_t, 0);
		ARG(obj_ptr<Buffer_base>, 1);

		hr = hmac(v0, v1, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_md2(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_md2(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_md4(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_md4(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_md5(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha1(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha224(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha256(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha384(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_sha512(v0, vr);

		METHOD_RETURN();
	}

	inline void hash_base::s_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
	{
		obj_ptr<Digest_base> vr;

		METHOD_ENTER(1, 1);

		ARG(obj_ptr<Buffer_base>, 0);

		hr = hmac_ripemd160(v0, vr);

		METHOD_RETURN();
	}

}

#endif

