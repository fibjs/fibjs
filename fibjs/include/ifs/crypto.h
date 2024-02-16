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

class Cipher_base;
class PKey_base;
class ECKey_base;
class BlsKey_base;
class X509Cert_base;
class X509Crl_base;
class X509Req_base;
class Digest_base;
class Buffer_base;

class crypto_base : public object_base {
    DECLARE_CLASS(crypto_base);

public:
    enum {
        C_AES = 1,
        C_DES = 2,
        C_DES_EDE3 = 3,
        C_CAMELLIA = 4,
        C_ARIA = 5,
        C_CHACHA20 = 6,
        C_SM4 = 7,
        C_ECB = 1,
        C_CBC = 2,
        C_CFB64 = 3,
        C_CFB128 = 4,
        C_OFB = 5,
        C_CTR = 6,
        C_GCM = 7,
        C_STREAM = 8,
        C_CCM = 9,
        C_XTS = 10,
        C_POLY1305 = 11,
        C_PKCS7 = 0,
        C_ONE_AND_ZEROS = 1,
        C_ZEROS_AND_LEN = 2,
        C_ZEROS = 3,
        C_NOPADDING = 4
    };

public:
    // crypto_base
    static result_t getHashes(v8::Local<v8::Array>& retVal);
    static result_t createHash(exlib::string algo, obj_ptr<Digest_base>& retVal);
    static result_t createHmac(exlib::string algo, Buffer_base* key, obj_ptr<Digest_base>& retVal);
    static result_t getCiphers(v8::Local<v8::Array>& retVal);
    static result_t createCipher(exlib::string algorithm, Buffer_base* key, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createCipheriv(exlib::string algorithm, Buffer_base* key, Buffer_base* iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipher(exlib::string algorithm, Buffer_base* key, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipheriv(exlib::string algorithm, Buffer_base* key, Buffer_base* iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t loadCert(exlib::string filename, obj_ptr<X509Cert_base>& retVal);
    static result_t loadCrl(exlib::string filename, obj_ptr<X509Crl_base>& retVal);
    static result_t loadReq(exlib::string filename, obj_ptr<X509Req_base>& retVal);
    static result_t loadPKey(exlib::string filename, obj_ptr<PKey_base>& retVal);
    static result_t randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t randomFill(Buffer_base* buffer, int32_t offset, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t generateKey(int32_t size, obj_ptr<PKey_base>& retVal, AsyncEvent* ac);
    static result_t generateKey(exlib::string curve, obj_ptr<PKey_base>& retVal, AsyncEvent* ac);
    static result_t hkdf(exlib::string algoName, Buffer_base* password, Buffer_base* salt, Buffer_base* info, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations, int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_getHashes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createHash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getCiphers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createCipher(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createCipheriv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createDecipher(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createDecipheriv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadReq(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadPKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomFill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_generateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hkdf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(crypto_base, randomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, randomFill, Buffer_base*, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE2(crypto_base, generateKey, int32_t, obj_ptr<PKey_base>);
    ASYNC_STATICVALUE2(crypto_base, generateKey, exlib::string, obj_ptr<PKey_base>);
    ASYNC_STATICVALUE6(crypto_base, hkdf, exlib::string, Buffer_base*, Buffer_base*, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Buffer_base*, Buffer_base*, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
};
}

#include "ifs/Cipher.h"
#include "ifs/PKey.h"
#include "ifs/ECKey.h"
#include "ifs/BlsKey.h"
#include "ifs/X509Cert.h"
#include "ifs/X509Crl.h"
#include "ifs/X509Req.h"
#include "ifs/Digest.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& crypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getHashes", s_static_getHashes, true, false },
        { "createHash", s_static_createHash, true, false },
        { "createHmac", s_static_createHmac, true, false },
        { "getCiphers", s_static_getCiphers, true, false },
        { "createCipher", s_static_createCipher, true, false },
        { "createCipheriv", s_static_createCipheriv, true, false },
        { "createDecipher", s_static_createDecipher, true, false },
        { "createDecipheriv", s_static_createDecipheriv, true, false },
        { "loadCert", s_static_loadCert, true, false },
        { "loadCrl", s_static_loadCrl, true, false },
        { "loadReq", s_static_loadReq, true, false },
        { "loadPKey", s_static_loadPKey, true, false },
        { "randomBytes", s_static_randomBytes, true, true },
        { "randomBytesSync", s_static_randomBytes, true, false },
        { "randomFill", s_static_randomFill, true, true },
        { "randomFillSync", s_static_randomFill, true, false },
        { "generateKey", s_static_generateKey, true, true },
        { "generateKeySync", s_static_generateKey, true, false },
        { "hkdf", s_static_hkdf, true, true },
        { "hkdfSync", s_static_hkdf, true, false },
        { "pbkdf2", s_static_pbkdf2, true, true },
        { "pbkdf2Sync", s_static_pbkdf2, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Cipher", Cipher_base::class_info },
        { "PKey", PKey_base::class_info },
        { "ECKey", ECKey_base::class_info },
        { "BlsKey", BlsKey_base::class_info },
        { "X509Cert", X509Cert_base::class_info },
        { "X509Crl", X509Crl_base::class_info },
        { "X509Req", X509Req_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "AES", C_AES },
        { "DES", C_DES },
        { "DES_EDE3", C_DES_EDE3 },
        { "CAMELLIA", C_CAMELLIA },
        { "ARIA", C_ARIA },
        { "CHACHA20", C_CHACHA20 },
        { "SM4", C_SM4 },
        { "ECB", C_ECB },
        { "CBC", C_CBC },
        { "CFB64", C_CFB64 },
        { "CFB128", C_CFB128 },
        { "OFB", C_OFB },
        { "CTR", C_CTR },
        { "GCM", C_GCM },
        { "STREAM", C_STREAM },
        { "CCM", C_CCM },
        { "XTS", C_XTS },
        { "POLY1305", C_POLY1305 },
        { "PKCS7", C_PKCS7 },
        { "ONE_AND_ZEROS", C_ONE_AND_ZEROS },
        { "ZEROS_AND_LEN", C_ZEROS_AND_LEN },
        { "ZEROS", C_ZEROS },
        { "NOPADDING", C_NOPADDING }
    };

    static ClassData s_cd = {
        "crypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void crypto_base::s_static_getHashes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHashes(vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createHash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = createHash(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = createHmac(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_getCiphers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getCiphers(vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createCipher(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = createCipher(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createCipheriv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    hr = createCipheriv(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createDecipher(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = createDecipher(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createDecipheriv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    hr = createDecipheriv(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadCert(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Crl_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadCrl(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadReq(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Req_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadReq(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadPKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadPKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 16);

    if (!cb.IsEmpty())
        hr = acb_randomBytes(v0, cb, args);
    else
        hr = ac_randomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomFill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_randomFill(v0, v1, v2, cb, args);
    else
        hr = ac_randomFill(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_generateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_generateKey(v0, cb, args);
    else
        hr = ac_generateKey(v0, vr);

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "secp521r1");

    if (!cb.IsEmpty())
        hr = acb_generateKey(v0, cb, args);
    else
        hr = ac_generateKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_hkdf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    ARG(obj_ptr<Buffer_base>, 3);
    ARG(int32_t, 4);

    if (!cb.IsEmpty())
        hr = acb_hkdf(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_hkdf(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(exlib::string, 4);

    if (!cb.IsEmpty())
        hr = acb_pbkdf2(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_pbkdf2(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}
}
