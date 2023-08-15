/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Digest_base;
class Buffer_base;
class PKey_base;

class hash_base : public object_base {
    DECLARE_CLASS(hash_base);

public:
    enum {
        C_MD5 = 1,
        C_SHA1 = 2,
        C_SHA224 = 3,
        C_SHA256 = 4,
        C_SHA384 = 5,
        C_SHA512 = 6,
        C_RIPEMD160 = 7,
        C_SM3 = 8,
        C_SHA3_256 = 9,
        C_SHA3_384 = 10,
        C_SHA3_512 = 11,
        C_KECCAK256 = 12,
        C_KECCAK384 = 13,
        C_KECCAK512 = 14,
        C_BLAKE2S = 15,
        C_BLAKE2B = 16,
        C_BLAKE2SP = 17,
        C_BLAKE2BP = 18
    };

public:
    // hash_base
    static result_t digest(int32_t algo, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t md5(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha1(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha224(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha256(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha384(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha512(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t ripemd160(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sm3(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sm3(PKey_base* pubKey, exlib::string id, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha3_256(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha3_384(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t sha3_512(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t keccak256(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t keccak384(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t keccak512(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t blake2s(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t blake2b(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t blake2sp(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t blake2bp(Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac(int32_t algo, Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_md5(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha1(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha224(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha256(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha384(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha512(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_ripemd160(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sm3(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sm3(PKey_base* pubKey, exlib::string id, Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha3_256(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha3_384(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_sha3_512(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_keccak256(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_keccak384(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_keccak512(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_blake2s(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_blake2b(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_blake2sp(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);
    static result_t hmac_blake2bp(Buffer_base* key, Buffer_base* data, obj_ptr<Digest_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_digest(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sm3(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha3_256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha3_384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sha3_512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keccak256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keccak384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_keccak512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_blake2s(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_blake2b(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_blake2sp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_blake2bp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sm3(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha3_256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha3_384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha3_512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_keccak256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_keccak384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_keccak512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_blake2s(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_blake2b(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_blake2sp(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_blake2bp(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Digest.h"
#include "ifs/Buffer.h"
#include "ifs/PKey.h"

namespace fibjs {
inline ClassInfo& hash_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "digest", s_static_digest, true, false },
        { "md5", s_static_md5, true, false },
        { "sha1", s_static_sha1, true, false },
        { "sha224", s_static_sha224, true, false },
        { "sha256", s_static_sha256, true, false },
        { "sha384", s_static_sha384, true, false },
        { "sha512", s_static_sha512, true, false },
        { "ripemd160", s_static_ripemd160, true, false },
        { "sm3", s_static_sm3, true, false },
        { "sha3_256", s_static_sha3_256, true, false },
        { "sha3_384", s_static_sha3_384, true, false },
        { "sha3_512", s_static_sha3_512, true, false },
        { "keccak256", s_static_keccak256, true, false },
        { "keccak384", s_static_keccak384, true, false },
        { "keccak512", s_static_keccak512, true, false },
        { "blake2s", s_static_blake2s, true, false },
        { "blake2b", s_static_blake2b, true, false },
        { "blake2sp", s_static_blake2sp, true, false },
        { "blake2bp", s_static_blake2bp, true, false },
        { "hmac", s_static_hmac, true, false },
        { "hmac_md5", s_static_hmac_md5, true, false },
        { "hmac_sha1", s_static_hmac_sha1, true, false },
        { "hmac_sha224", s_static_hmac_sha224, true, false },
        { "hmac_sha256", s_static_hmac_sha256, true, false },
        { "hmac_sha384", s_static_hmac_sha384, true, false },
        { "hmac_sha512", s_static_hmac_sha512, true, false },
        { "hmac_ripemd160", s_static_hmac_ripemd160, true, false },
        { "hmac_sm3", s_static_hmac_sm3, true, false },
        { "hmac_sha3_256", s_static_hmac_sha3_256, true, false },
        { "hmac_sha3_384", s_static_hmac_sha3_384, true, false },
        { "hmac_sha3_512", s_static_hmac_sha3_512, true, false },
        { "hmac_keccak256", s_static_hmac_keccak256, true, false },
        { "hmac_keccak384", s_static_hmac_keccak384, true, false },
        { "hmac_keccak512", s_static_hmac_keccak512, true, false },
        { "hmac_blake2s", s_static_hmac_blake2s, true, false },
        { "hmac_blake2b", s_static_hmac_blake2b, true, false },
        { "hmac_blake2sp", s_static_hmac_blake2sp, true, false },
        { "hmac_blake2bp", s_static_hmac_blake2bp, true, false }
    };

    static ClassData::ClassConst s_const[] = {
        { "MD5", C_MD5 },
        { "SHA1", C_SHA1 },
        { "SHA224", C_SHA224 },
        { "SHA256", C_SHA256 },
        { "SHA384", C_SHA384 },
        { "SHA512", C_SHA512 },
        { "RIPEMD160", C_RIPEMD160 },
        { "SM3", C_SM3 },
        { "SHA3_256", C_SHA3_256 },
        { "SHA3_384", C_SHA3_384 },
        { "SHA3_512", C_SHA3_512 },
        { "KECCAK256", C_KECCAK256 },
        { "KECCAK384", C_KECCAK384 },
        { "KECCAK512", C_KECCAK512 },
        { "BLAKE2S", C_BLAKE2S },
        { "BLAKE2B", C_BLAKE2B },
        { "BLAKE2SP", C_BLAKE2SP },
        { "BLAKE2BP", C_BLAKE2BP }
    };

    static ClassData s_cd = {
        "hash", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void hash_base::s_static_digest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = digest(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = md5(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha1(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha224(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha256(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha384(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha512(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = ripemd160(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sm3(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sm3(v0, vr);

    METHOD_OVER(3, 2);

    ARG(obj_ptr<PKey_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(obj_ptr<Buffer_base>, 2, NULL);

    hr = sm3(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha3_256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha3_256(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha3_384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha3_384(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha3_512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha3_512(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_keccak256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = keccak256(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_keccak384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = keccak384(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_keccak512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = keccak512(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_blake2s(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = blake2s(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_blake2b(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = blake2b(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_blake2sp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = blake2sp(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_blake2bp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = blake2bp(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(int32_t, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(obj_ptr<Buffer_base>, 2, NULL);

    hr = hmac(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_md5(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha1(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha224(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha256(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha384(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha512(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_ripemd160(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sm3(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sm3(v0, v1, vr);

    METHOD_OVER(4, 3);

    ARG(obj_ptr<PKey_base>, 0);
    ARG(exlib::string, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    OPT_ARG(obj_ptr<Buffer_base>, 3, NULL);

    hr = hmac_sm3(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha3_256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha3_256(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha3_384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha3_384(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_sha3_512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_sha3_512(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_keccak256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_keccak256(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_keccak384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_keccak384(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_keccak512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_keccak512(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_blake2s(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_blake2s(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_blake2b(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_blake2b(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_blake2sp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_blake2sp(v0, v1, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_hmac_blake2bp(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(obj_ptr<Buffer_base>, 1, NULL);

    hr = hmac_blake2bp(v0, v1, vr);

    METHOD_RETURN();
}
}
