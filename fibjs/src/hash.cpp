/*
 * hash.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "ifs/hash.h"
#include "Digest.h"

namespace fibjs
{

result_t hash_base::digest(int32_t algo, Buffer_base* data,
		obj_ptr<Digest_base>& retVal)
{
	if (algo < hash_base::_MD2 || algo > hash_base::_SHA512)
		return CALL_E_INVALIDARG;

	retVal = new Digest(algo);
	retVal->update(data);

	return 0;
}

result_t hash_base::digest(int32_t algo, const char* text,
		obj_ptr<Digest_base>& retVal)
{
	if (algo < hash_base::_MD2 || algo > hash_base::_SHA512)
		return CALL_E_INVALIDARG;

	retVal = new Digest(algo);
	retVal->update(text);

	return 0;
}

result_t hash_base::md2(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_MD2, data, retVal);
}

result_t hash_base::md2(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_MD2, text, retVal);
}

result_t hash_base::md4(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_MD4, data, retVal);
}

result_t hash_base::md4(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_MD4, text, retVal);
}

result_t hash_base::md5(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_MD5, data, retVal);
}

result_t hash_base::md5(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_MD5, text, retVal);
}

result_t hash_base::sha1(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA1, data, retVal);
}

result_t hash_base::sha1(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA1, text, retVal);
}

result_t hash_base::sha224(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA224, data, retVal);
}

result_t hash_base::sha224(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA224, text, retVal);
}

result_t hash_base::sha256(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA256, data, retVal);
}

result_t hash_base::sha256(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA256, text, retVal);
}

result_t hash_base::sha384(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA384, data, retVal);
}

result_t hash_base::sha384(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA384, text, retVal);
}

result_t hash_base::sha512(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA512, data, retVal);
}

result_t hash_base::sha512(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_SHA512, text, retVal);
}

result_t hash_base::ripemd128(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD128, data, retVal);
}

result_t hash_base::ripemd128(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD128, text, retVal);
}

result_t hash_base::ripemd160(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD160, data, retVal);
}

result_t hash_base::ripemd160(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD160, text, retVal);
}

result_t hash_base::ripemd256(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD256, data, retVal);
}

result_t hash_base::ripemd256(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD256, text, retVal);
}

result_t hash_base::ripemd320(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD320, data, retVal);
}

result_t hash_base::ripemd320(const char* text, obj_ptr<Digest_base>& retVal)
{
	return digest(hash_base::_RIPEMD320, text, retVal);
}

result_t hash_base::hmac(int32_t algo, Buffer_base* key,
		obj_ptr<Digest_base>& retVal)
{
	if (algo < hash_base::_MD2 || algo > hash_base::_SHA512)
		return CALL_E_INVALIDARG;

	std::string strBuf;
	key->toString(strBuf);

	retVal = new Digest(algo, strBuf.c_str(), (int) strBuf.length());

	return 0;
}

result_t hash_base::hmac(int32_t algo, const char* key,
		obj_ptr<Digest_base>& retVal)
{
	if (algo < hash_base::_MD2 || algo > hash_base::_SHA512)
		return CALL_E_INVALIDARG;

	retVal = new Digest(algo, key, (int) qstrlen(key));

	return 0;
}

result_t hash_base::hmac_md2(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_MD2, key, retVal);
}

result_t hash_base::hmac_md2(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_MD2, key, retVal);
}

result_t hash_base::hmac_md4(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_MD4, key, retVal);
}

result_t hash_base::hmac_md4(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_MD4, key, retVal);
}

result_t hash_base::hmac_md5(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_MD5, key, retVal);
}

result_t hash_base::hmac_md5(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_MD5, key, retVal);
}

result_t hash_base::hmac_sha1(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA1, key, retVal);
}

result_t hash_base::hmac_sha1(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA1, key, retVal);
}

result_t hash_base::hmac_sha224(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA224, key, retVal);
}

result_t hash_base::hmac_sha224(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA224, key, retVal);
}

result_t hash_base::hmac_sha256(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA256, key, retVal);
}

result_t hash_base::hmac_sha256(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA256, key, retVal);
}

result_t hash_base::hmac_sha384(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA384, key, retVal);
}

result_t hash_base::hmac_sha384(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA384, key, retVal);
}

result_t hash_base::hmac_sha512(Buffer_base* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA512, key, retVal);
}

result_t hash_base::hmac_sha512(const char* key, obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_SHA512, key, retVal);
}

result_t hash_base::hmac_ripemd128(Buffer_base* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD128, key, retVal);
}

result_t hash_base::hmac_ripemd128(const char* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD128, key, retVal);
}

result_t hash_base::hmac_ripemd160(Buffer_base* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD160, key, retVal);
}

result_t hash_base::hmac_ripemd160(const char* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD160, key, retVal);
}

result_t hash_base::hmac_ripemd256(Buffer_base* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD256, key, retVal);
}

result_t hash_base::hmac_ripemd256(const char* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD256, key, retVal);
}

result_t hash_base::hmac_ripemd320(Buffer_base* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD320, key, retVal);
}

result_t hash_base::hmac_ripemd320(const char* key,
		obj_ptr<Digest_base>& retVal)
{
	return hmac(hash_base::_RIPEMD320, key, retVal);
}

} /* namespace fibjs */
