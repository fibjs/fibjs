/*
 * hash.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/hash.h"
#include "Digest.h"

namespace fibjs {

DECLARE_MODULE(hash);

result_t hash_base::digest(int32_t algo, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    if (algo < hash_base::C_MD2 || algo > hash_base::C_SM3)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new Digest((mbedtls_md_type_t)algo);

    if (data) {
        obj_ptr<Digest_base> r;
        retVal->update(data, r);
    }

    return 0;
}

result_t hash_base::md2(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_MD2, data, retVal);
}

result_t hash_base::md4(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_MD4, data, retVal);
}

result_t hash_base::md5(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_MD5, data, retVal);
}

result_t hash_base::sha1(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_SHA1, data, retVal);
}

result_t hash_base::sha224(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_SHA224, data, retVal);
}

result_t hash_base::sha256(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_SHA256, data, retVal);
}

result_t hash_base::sha384(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_SHA384, data, retVal);
}

result_t hash_base::sha512(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_SHA512, data, retVal);
}

result_t hash_base::ripemd160(Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return digest(hash_base::C_RIPEMD160, data, retVal);
}

result_t hash_base::hmac(int32_t algo, Buffer_base* key, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    if (algo < hash_base::C_MD2 || algo > hash_base::C_SM3)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    exlib::string strBuf;
    key->toString(strBuf);

    retVal = new Digest((mbedtls_md_type_t)algo, strBuf.c_str(), (int32_t)strBuf.length());

    if (data) {
        obj_ptr<Digest_base> r;
        retVal->update(data, r);
    }

    return 0;
}

result_t hash_base::hmac_md2(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_MD2, key, data, retVal);
}

result_t hash_base::hmac_md4(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_MD4, key, data, retVal);
}

result_t hash_base::hmac_md5(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_MD5, key, data, retVal);
}

result_t hash_base::hmac_sha1(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_SHA1, key, data, retVal);
}

result_t hash_base::hmac_sha224(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_SHA224, key, data, retVal);
}

result_t hash_base::hmac_sha256(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_SHA256, key, data, retVal);
}

result_t hash_base::hmac_sha384(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_SHA384, key, data, retVal);
}

result_t hash_base::hmac_sha512(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_SHA512, key, data, retVal);
}

result_t hash_base::hmac_ripemd160(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal)
{
    return hmac(hash_base::C_RIPEMD160, key, data, retVal);
}

} /* namespace fibjs */
