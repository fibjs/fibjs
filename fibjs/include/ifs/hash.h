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
        C_SM3 = 8
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

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

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
    static void s_static_hmac(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_md5(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha224(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha256(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha384(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sha512(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hmac_sm3(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Digest.h"
#include "ifs/Buffer.h"
#include "ifs/PKey.h"

namespace fibjs {
inline ClassInfo& hash_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "digest", s_static_digest, true },
        { "md5", s_static_md5, true },
        { "sha1", s_static_sha1, true },
        { "sha224", s_static_sha224, true },
        { "sha256", s_static_sha256, true },
        { "sha384", s_static_sha384, true },
        { "sha512", s_static_sha512, true },
        { "ripemd160", s_static_ripemd160, true },
        { "sm3", s_static_sm3, true },
        { "hmac", s_static_hmac, true },
        { "hmac_md5", s_static_hmac_md5, true },
        { "hmac_sha1", s_static_hmac_sha1, true },
        { "hmac_sha224", s_static_hmac_sha224, true },
        { "hmac_sha256", s_static_hmac_sha256, true },
        { "hmac_sha384", s_static_hmac_sha384, true },
        { "hmac_sha512", s_static_hmac_sha512, true },
        { "hmac_ripemd160", s_static_hmac_ripemd160, true },
        { "hmac_sm3", s_static_hmac_sm3, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "MD5", C_MD5 },
        { "SHA1", C_SHA1 },
        { "SHA224", C_SHA224 },
        { "SHA256", C_SHA256 },
        { "SHA384", C_SHA384 },
        { "SHA512", C_SHA512 },
        { "RIPEMD160", C_RIPEMD160 },
        { "SM3", C_SM3 }
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

    METHOD_NAME("hash.digest");
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

    METHOD_NAME("hash.md5");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = md5(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.sha1");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha1(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha224(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.sha224");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha224(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha256(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.sha256");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha256(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha384(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.sha384");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha384(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sha512(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.sha512");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = sha512(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_ripemd160(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.ripemd160");
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(obj_ptr<Buffer_base>, 0, NULL);

    hr = ripemd160(v0, vr);

    METHOD_RETURN();
}

inline void hash_base::s_static_sm3(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.sm3");
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

inline void hash_base::s_static_hmac(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("hash.hmac");
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

    METHOD_NAME("hash.hmac_md5");
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

    METHOD_NAME("hash.hmac_sha1");
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

    METHOD_NAME("hash.hmac_sha224");
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

    METHOD_NAME("hash.hmac_sha256");
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

    METHOD_NAME("hash.hmac_sha384");
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

    METHOD_NAME("hash.hmac_sha512");
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

    METHOD_NAME("hash.hmac_ripemd160");
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

    METHOD_NAME("hash.hmac_sm3");
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
}
