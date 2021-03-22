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
class X509Cert_base;
class X509Crl_base;
class X509Req_base;
class Digest_base;
class Buffer_base;

class crypto_base : public object_base {
    DECLARE_CLASS(crypto_base);

public:
    enum {
        __AES = 1,
        __CAMELLIA = 2,
        __DES = 3,
        __DES_EDE = 4,
        __DES_EDE3 = 5,
        __BLOWFISH = 6,
        __ARC4 = 7,
        __ARIA = 8,
        __CHACHA20 = 9,
        __SM4 = 10,
        __ECB = 1,
        __CBC = 2,
        __CFB64 = 3,
        __CFB128 = 4,
        __OFB = 5,
        __CTR = 6,
        __GCM = 7,
        __STREAM = 8,
        __CCM = 9,
        __XTS = 10,
        __POLY1305 = 11,
        __PKCS7 = 0,
        __ONE_AND_ZEROS = 1,
        __ZEROS_AND_LEN = 2,
        __ZEROS = 3,
        __NOPADDING = 4
    };

public:
    // crypto_base
    static result_t createHash(exlib::string algo, obj_ptr<Digest_base>& retVal);
    static result_t createHmac(exlib::string algo, Buffer_base* key, obj_ptr<Digest_base>& retVal);
    static result_t loadPKey(exlib::string filename, exlib::string password, obj_ptr<PKey_base>& retVal);
    static result_t loadCert(exlib::string filename, obj_ptr<X509Cert_base>& retVal);
    static result_t loadCrl(exlib::string filename, obj_ptr<X509Crl_base>& retVal);
    static result_t loadReq(exlib::string filename, obj_ptr<X509Req_base>& retVal);
    static result_t randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t simpleRandomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pseudoRandomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t randomArt(Buffer_base* data, exlib::string title, int32_t size, exlib::string& retVal);
    static result_t genRsaKey(int32_t size, obj_ptr<PKey_base>& retVal, AsyncEvent* ac);
    static result_t genEcKey(exlib::string curve, obj_ptr<PKey_base>& retVal, AsyncEvent* ac);
    static result_t genSm2Key(obj_ptr<PKey_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf1(Buffer_base* password, Buffer_base* salt, int32_t iterations, int32_t size, int32_t algo, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf1(Buffer_base* password, Buffer_base* salt, int32_t iterations, int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations, int32_t size, int32_t algo, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations, int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_createHash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadPKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_loadReq(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_simpleRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomArt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_genSm2Key(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pbkdf1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(crypto_base, randomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE2(crypto_base, simpleRandomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE2(crypto_base, pseudoRandomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE2(crypto_base, genRsaKey, int32_t, obj_ptr<PKey_base>);
    ASYNC_STATICVALUE2(crypto_base, genEcKey, exlib::string, obj_ptr<PKey_base>);
    ASYNC_STATICVALUE1(crypto_base, genSm2Key, obj_ptr<PKey_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf1, Buffer_base*, Buffer_base*, int32_t, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf1, Buffer_base*, Buffer_base*, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Buffer_base*, Buffer_base*, int32_t, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Buffer_base*, Buffer_base*, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
};
}

#include "ifs/Cipher.h"
#include "ifs/PKey.h"
#include "ifs/X509Cert.h"
#include "ifs/X509Crl.h"
#include "ifs/X509Req.h"
#include "ifs/Digest.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& crypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createHash", s_static_createHash, true },
        { "createHmac", s_static_createHmac, true },
        { "loadPKey", s_static_loadPKey, true },
        { "loadCert", s_static_loadCert, true },
        { "loadCrl", s_static_loadCrl, true },
        { "loadReq", s_static_loadReq, true },
        { "randomBytes", s_static_randomBytes, true },
        { "randomBytesSync", s_static_randomBytes, true },
        { "simpleRandomBytes", s_static_simpleRandomBytes, true },
        { "simpleRandomBytesSync", s_static_simpleRandomBytes, true },
        { "pseudoRandomBytes", s_static_pseudoRandomBytes, true },
        { "pseudoRandomBytesSync", s_static_pseudoRandomBytes, true },
        { "randomArt", s_static_randomArt, true },
        { "genRsaKey", s_static_genRsaKey, true },
        { "genRsaKeySync", s_static_genRsaKey, true },
        { "genEcKey", s_static_genEcKey, true },
        { "genEcKeySync", s_static_genEcKey, true },
        { "genSm2Key", s_static_genSm2Key, true },
        { "genSm2KeySync", s_static_genSm2Key, true },
        { "pbkdf1", s_static_pbkdf1, true },
        { "pbkdf1Sync", s_static_pbkdf1, true },
        { "pbkdf2", s_static_pbkdf2, true },
        { "pbkdf2Sync", s_static_pbkdf2, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Cipher", Cipher_base::class_info },
        { "PKey", PKey_base::class_info },
        { "X509Cert", X509Cert_base::class_info },
        { "X509Crl", X509Crl_base::class_info },
        { "X509Req", X509Req_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "AES", __AES },
        { "CAMELLIA", __CAMELLIA },
        { "DES", __DES },
        { "DES_EDE", __DES_EDE },
        { "DES_EDE3", __DES_EDE3 },
        { "BLOWFISH", __BLOWFISH },
        { "ARC4", __ARC4 },
        { "ARIA", __ARIA },
        { "CHACHA20", __CHACHA20 },
        { "SM4", __SM4 },
        { "ECB", __ECB },
        { "CBC", __CBC },
        { "CFB64", __CFB64 },
        { "CFB128", __CFB128 },
        { "OFB", __OFB },
        { "CTR", __CTR },
        { "GCM", __GCM },
        { "STREAM", __STREAM },
        { "CCM", __CCM },
        { "XTS", __XTS },
        { "POLY1305", __POLY1305 },
        { "PKCS7", __PKCS7 },
        { "ONE_AND_ZEROS", __ONE_AND_ZEROS },
        { "ZEROS_AND_LEN", __ZEROS_AND_LEN },
        { "ZEROS", __ZEROS },
        { "NOPADDING", __NOPADDING }
    };

    static ClassData s_cd = {
        "crypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void crypto_base::s_static_createHash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("crypto.createHash");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = createHash(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("crypto.createHmac");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = createHmac(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadPKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_NAME("crypto.loadPKey");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = loadPKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_NAME("crypto.loadCert");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadCert(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Crl_base> vr;

    METHOD_NAME("crypto.loadCrl");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadCrl(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_loadReq(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Req_base> vr;

    METHOD_NAME("crypto.loadReq");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadReq(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.randomBytes");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_randomBytes(v0, cb, args);
    else
        hr = ac_randomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_simpleRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.simpleRandomBytes");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_simpleRandomBytes(v0, cb, args);
    else
        hr = ac_simpleRandomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.pseudoRandomBytes");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_pseudoRandomBytes(v0, cb, args);
    else
        hr = ac_pseudoRandomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomArt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("crypto.randomArt");
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(int32_t, 2, 8);

    hr = randomArt(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_NAME("crypto.genRsaKey");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = acb_genRsaKey(v0, cb, args);
    else
        hr = ac_genRsaKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_NAME("crypto.genEcKey");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "secp521r1");

    if (!cb.IsEmpty())
        hr = acb_genEcKey(v0, cb, args);
    else
        hr = ac_genEcKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_genSm2Key(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_NAME("crypto.genSm2Key");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = acb_genSm2Key(cb, args);
    else
        hr = ac_genSm2Key(vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_pbkdf1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.pbkdf1");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(int32_t, 4);

    if (!cb.IsEmpty())
        hr = acb_pbkdf1(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_pbkdf1(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(exlib::string, 4);

    if (!cb.IsEmpty())
        hr = acb_pbkdf1(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_pbkdf1(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.pbkdf2");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(int32_t, 4);

    if (!cb.IsEmpty())
        hr = acb_pbkdf2(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_pbkdf2(v0, v1, v2, v3, v4, vr);

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
