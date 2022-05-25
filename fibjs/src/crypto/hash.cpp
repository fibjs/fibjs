/*
 * hash.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/hash.h"
#include "Digest.h"
#include "md_api.h"

namespace fibjs {

DECLARE_MODULE(hash);

obj_ptr<NArray> g_hashes;
class init_hashes {
public:
    init_hashes()
    {
        g_hashes = new NArray();

        g_hashes->append("md5");
        g_hashes->append("sha1");
        g_hashes->append("sha224");
        g_hashes->append("sha256");
        g_hashes->append("sha384");
        g_hashes->append("sha512");
        g_hashes->append("ripemd160");
        g_hashes->append("sm3");
        g_hashes->append("keccak256");
        g_hashes->append("keccak384");
        g_hashes->append("keccak512");
        g_hashes->append("blake2s");
        g_hashes->append("blake2b");
        g_hashes->append("blake2sp");
        g_hashes->append("blake2bp");
        g_hashes->append("md5_hmac");
        g_hashes->append("sha1_hmac");
        g_hashes->append("sha224_hmac");
        g_hashes->append("sha256_hmac");
        g_hashes->append("sha384_hmac");
        g_hashes->append("sha512_hmac");
        g_hashes->append("ripemd160_hmac");
        g_hashes->append("sm3_hmac");
        g_hashes->append("keccak256_hmac");
        g_hashes->append("keccak384_hmac");
        g_hashes->append("keccak512_hmac");
        g_hashes->append("blake2s_hmac");
        g_hashes->append("blake2b_hmac");
        g_hashes->append("blake2sp_hmac");
        g_hashes->append("blake2bp_hmac");
    }
} s_init_hashes;

result_t hash_base::digest(int32_t algo, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    if (algo < MBEDTLS_MD_MD5 || algo >= MBEDTLS_MD_MAX)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    retVal = new Digest((mbedtls_md_type_t)algo);

    if (data) {
        obj_ptr<Digest_base> r;
        retVal->update(data, r);
    }

    return 0;
}

result_t hash_base::hmac(int32_t algo, Buffer_base* key, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    if (algo < MBEDTLS_MD_MD5 || algo >= MBEDTLS_MD_MAX)
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

#define DEF_FUNC(fn, typ)                                                                            \
    result_t hash_base::fn(Buffer_base* data, obj_ptr<Digest_base>& retVal)                          \
    {                                                                                                \
        return digest(hash_base::C_##typ, data, retVal);                                             \
    }                                                                                                \
    result_t hash_base::hmac_##fn(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal) \
    {                                                                                                \
        return hmac(hash_base::C_##typ, key, data, retVal);                                          \
    }

DEF_FUNC(md5, MD5);
DEF_FUNC(sha1, SHA1);
DEF_FUNC(sha224, SHA224);
DEF_FUNC(sha256, SHA256);
DEF_FUNC(sha384, SHA384);
DEF_FUNC(sha512, SHA512);
DEF_FUNC(ripemd160, RIPEMD160);
DEF_FUNC(keccak256, KECCAK256);
DEF_FUNC(keccak384, KECCAK384);
DEF_FUNC(keccak512, KECCAK512);
DEF_FUNC(blake2s, BLAKE2S);
DEF_FUNC(blake2b, BLAKE2B);
DEF_FUNC(blake2sp, BLAKE2SP);
DEF_FUNC(blake2bp, BLAKE2BP);

} /* namespace fibjs */
