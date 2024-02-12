/*
 * hash.cpp
 *
 *  Created on: Aug 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "ifs/hash.h"
#include "ifs/crypto.h"
#include "Digest.h"
#include "Buffer.h"
#include "crypto.h"
#include <openssl/kdf.h>

namespace fibjs {

DECLARE_MODULE(hash);

obj_ptr<NArray> g_hashes;
class init_hashes {
public:
    init_hashes()
    {
        g_hashes = new NArray();

        EVP_MD_do_all_sorted([](const EVP_MD* md,
                                 const char* from, const char* to,
                                 void* x) {
            if (from)
                g_hashes->append(from);
        },
            NULL);
    }
} s_init_hashes;

static const struct {
    const char* name;
    const EVP_MD* md;
} s_algos[] = {
    { "sha3_256", EVP_sha3_256() },
    { "sha3_384", EVP_sha3_384() },
    { "sha3_512", EVP_sha3_512() },
    { "blake2s", EVP_blake2s256() },
    { "blake2b", EVP_blake2b512() }
};

static const char* s_md_names[] = {
    "md5",
    "sha1",
    "sha224",
    "sha256",
    "sha384",
    "sha512",
    "ripemd160",
    "sm3",
    "sha3_256",
    "sha3_384",
    "sha3_512",
    "shake128",
    "shake256",
    "blake2s",
    "blake2b"
};

const EVP_MD* _evp_md_type(const char* algo)
{
    for (int i = 0; i < ARRAYSIZE(s_algos); i++)
        if (!qstricmp(algo, s_algos[i].name))
            return s_algos[i].md;

    return EVP_get_digestbyname(algo);
}

result_t crypto_base::createHash(exlib::string algo, obj_ptr<Digest_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algo.c_str());
    if (md) {
        retVal = new Digest(md);
        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t crypto_base::createHmac(exlib::string algo, Buffer_base* key,
    obj_ptr<Digest_base>& retVal)
{
    const EVP_MD* md = _evp_md_type(algo.c_str());
    if (md) {
        if (EVP_MD_get_flags(md) & EVP_MD_FLAG_XOF)
            return CHECK_ERROR(CALL_E_INVALID_CALL);

        Buffer* buf = Buffer::Cast(key);
        retVal = new Digest(md, (const char*)buf->data(), buf->length());
        return 0;
    }

    return CHECK_ERROR(CALL_E_INVALID_CALL);
}

result_t hash_base::digest(int32_t algo, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    if (algo < C_MD5 || algo > ARRAYSIZE(s_md_names))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    result_t hr = crypto_base::createHash(s_md_names[algo - 1], retVal);
    if (hr < 0)
        return hr;

    if (data) {
        obj_ptr<Digest_base> r;
        retVal->update(data, r);
    }

    return 0;
}

result_t hash_base::hmac(int32_t algo, Buffer_base* key, Buffer_base* data,
    obj_ptr<Digest_base>& retVal)
{
    if (algo < C_MD5 || algo > ARRAYSIZE(s_md_names))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    result_t hr = crypto_base::createHmac(s_md_names[algo - 1], key, retVal);
    if (hr < 0)
        return hr;

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
DEF_FUNC(sm3, SM3);
DEF_FUNC(sha3_256, SHA3_256);
DEF_FUNC(sha3_384, SHA3_384);
DEF_FUNC(sha3_512, SHA3_512);
DEF_FUNC(shake128, SHAKE128);
DEF_FUNC(shake256, SHAKE256);
DEF_FUNC(blake2s, BLAKE2S);
DEF_FUNC(blake2b, BLAKE2B);

result_t crypto_base::hkdf(exlib::string algoName, Buffer_base* password, Buffer_base* salt, Buffer_base* info,
    int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (size < 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    const EVP_MD* md = _evp_md_type(algoName.c_str());
    if (!md)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Buffer* buf = Buffer::Cast(password);
    Buffer* saltBuf = Buffer::Cast(salt);
    Buffer* infoBuf = Buffer::Cast(info);
    obj_ptr<Buffer> ret = new Buffer(NULL, size);
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    size_t keylen = size;

    if (EVP_PKEY_derive_init(pctx) <= 0
        || EVP_PKEY_CTX_set_hkdf_md(pctx, md) <= 0
        || EVP_PKEY_CTX_set1_hkdf_salt(pctx, (const unsigned char*)saltBuf->data(), saltBuf->length()) <= 0
        || EVP_PKEY_CTX_set1_hkdf_key(pctx, (const unsigned char*)buf->data(), buf->length()) <= 0
        || EVP_PKEY_CTX_add1_hkdf_info(pctx, (const unsigned char*)infoBuf->data(), infoBuf->length()) <= 0
        || EVP_PKEY_derive(pctx, (unsigned char*)ret->data(), &keylen) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        return openssl_error();
    }

    EVP_PKEY_CTX_free(pctx);
    retVal = ret;

    return 0;
}

result_t crypto_base::pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations,
    int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal,
    AsyncEvent* ac)
{
    if (iterations < 1 || size < 1)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    const EVP_MD* md = _evp_md_type(algoName.c_str());
    if (!md)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    Buffer* buf = Buffer::Cast(password);
    Buffer* saltBuf = Buffer::Cast(salt);
    obj_ptr<Buffer> ret = new Buffer(NULL, size);

    int32_t hr = PKCS5_PBKDF2_HMAC((const char*)buf->data(), buf->length(),
        (const unsigned char*)saltBuf->data(), saltBuf->length(),
        iterations, md, size, ret->data());
    if (hr != 1)
        return openssl_error();

    retVal = ret;
    return 0;
}

result_t crypto_base::getHashes(v8::Local<v8::Array>& retVal)
{
    v8::Local<v8::Value> v;
    g_hashes->valueOf(v);

    retVal = v8::Local<v8::Array>::Cast(v);

    return 0;
}

} /* namespace fibjs */
