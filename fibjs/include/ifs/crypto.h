/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _crypto_base_H_
#define _crypto_base_H_

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
        _AES = 1,
        _CAMELLIA = 2,
        _DES = 3,
        _DES_EDE = 4,
        _DES_EDE3 = 5,
        _BLOWFISH = 6,
        _ARC4 = 7,
        _ECB = 1,
        _CBC = 2,
        _CFB64 = 3,
        _CFB128 = 4,
        _OFB = 5,
        _CTR = 6,
        _GCM = 7,
        _STREAM = 8,
        _CCM = 9,
        _PKCS7 = 0,
        _ONE_AND_ZEROS = 1,
        _ZEROS_AND_LEN = 2,
        _ZEROS = 3,
        _NOPADDING = 4
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
    static void s_get_AES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CAMELLIA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_DES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_DES_EDE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_DES_EDE3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_BLOWFISH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ARC4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ECB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CBC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CFB64(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CFB128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_OFB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CTR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_GCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PKCS7(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ONE_AND_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ZEROS_AND_LEN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_NOPADDING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_createHash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadPKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadReq(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_simpleRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_randomArt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pbkdf1(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(crypto_base, randomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE2(crypto_base, simpleRandomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE2(crypto_base, pseudoRandomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf1, Buffer_base*, Buffer_base*, int32_t, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf1, Buffer_base*, Buffer_base*, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Buffer_base*, Buffer_base*, int32_t, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Buffer_base*, Buffer_base*, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
};
}

#include "Cipher.h"
#include "PKey.h"
#include "X509Cert.h"
#include "X509Crl.h"
#include "X509Req.h"
#include "Digest.h"
#include "Buffer.h"

namespace fibjs {
inline ClassInfo& crypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createHash", s_createHash, true },
        { "createHmac", s_createHmac, true },
        { "loadPKey", s_loadPKey, true },
        { "loadCert", s_loadCert, true },
        { "loadCrl", s_loadCrl, true },
        { "loadReq", s_loadReq, true },
        { "randomBytes", s_randomBytes, true },
        { "randomBytesSync", s_randomBytes, true },
        { "simpleRandomBytes", s_simpleRandomBytes, true },
        { "simpleRandomBytesSync", s_simpleRandomBytes, true },
        { "pseudoRandomBytes", s_pseudoRandomBytes, true },
        { "pseudoRandomBytesSync", s_pseudoRandomBytes, true },
        { "randomArt", s_randomArt, true },
        { "pbkdf1", s_pbkdf1, true },
        { "pbkdf1Sync", s_pbkdf1, true },
        { "pbkdf2", s_pbkdf2, true },
        { "pbkdf2Sync", s_pbkdf2, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Cipher", Cipher_base::class_info },
        { "PKey", PKey_base::class_info },
        { "X509Cert", X509Cert_base::class_info },
        { "X509Crl", X509Crl_base::class_info },
        { "X509Req", X509Req_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "AES", s_get_AES, block_set, true },
        { "CAMELLIA", s_get_CAMELLIA, block_set, true },
        { "DES", s_get_DES, block_set, true },
        { "DES_EDE", s_get_DES_EDE, block_set, true },
        { "DES_EDE3", s_get_DES_EDE3, block_set, true },
        { "BLOWFISH", s_get_BLOWFISH, block_set, true },
        { "ARC4", s_get_ARC4, block_set, true },
        { "ECB", s_get_ECB, block_set, true },
        { "CBC", s_get_CBC, block_set, true },
        { "CFB64", s_get_CFB64, block_set, true },
        { "CFB128", s_get_CFB128, block_set, true },
        { "OFB", s_get_OFB, block_set, true },
        { "CTR", s_get_CTR, block_set, true },
        { "GCM", s_get_GCM, block_set, true },
        { "STREAM", s_get_STREAM, block_set, true },
        { "CCM", s_get_CCM, block_set, true },
        { "PKCS7", s_get_PKCS7, block_set, true },
        { "ONE_AND_ZEROS", s_get_ONE_AND_ZEROS, block_set, true },
        { "ZEROS_AND_LEN", s_get_ZEROS_AND_LEN, block_set, true },
        { "ZEROS", s_get_ZEROS, block_set, true },
        { "NOPADDING", s_get_NOPADDING, block_set, true }
    };

    static ClassData s_cd = {
        "crypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void crypto_base::s_get_AES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.AES");
    int32_t vr = _AES;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_CAMELLIA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.CAMELLIA");
    int32_t vr = _CAMELLIA;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_DES(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.DES");
    int32_t vr = _DES;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_DES_EDE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.DES_EDE");
    int32_t vr = _DES_EDE;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_DES_EDE3(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.DES_EDE3");
    int32_t vr = _DES_EDE3;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_BLOWFISH(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.BLOWFISH");
    int32_t vr = _BLOWFISH;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_ARC4(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.ARC4");
    int32_t vr = _ARC4;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_ECB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.ECB");
    int32_t vr = _ECB;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_CBC(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.CBC");
    int32_t vr = _CBC;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_CFB64(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.CFB64");
    int32_t vr = _CFB64;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_CFB128(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.CFB128");
    int32_t vr = _CFB128;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_OFB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.OFB");
    int32_t vr = _OFB;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_CTR(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.CTR");
    int32_t vr = _CTR;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_GCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.GCM");
    int32_t vr = _GCM;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_STREAM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.STREAM");
    int32_t vr = _STREAM;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_CCM(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.CCM");
    int32_t vr = _CCM;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_PKCS7(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.PKCS7");
    int32_t vr = _PKCS7;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_ONE_AND_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.ONE_AND_ZEROS");
    int32_t vr = _ONE_AND_ZEROS;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_ZEROS_AND_LEN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.ZEROS_AND_LEN");
    int32_t vr = _ZEROS_AND_LEN;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_ZEROS(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.ZEROS");
    int32_t vr = _ZEROS;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_get_NOPADDING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("crypto.NOPADDING");
    int32_t vr = _NOPADDING;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void crypto_base::s_createHash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("crypto.createHash");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = createHash(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void crypto_base::s_loadPKey(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void crypto_base::s_loadCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Cert_base> vr;

    METHOD_NAME("crypto.loadCert");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadCert(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_loadCrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Crl_base> vr;

    METHOD_NAME("crypto.loadCrl");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadCrl(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_loadReq(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Req_base> vr;

    METHOD_NAME("crypto.loadReq");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = loadReq(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.randomBytes");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        acb_randomBytes(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_randomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_simpleRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.simpleRandomBytes");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        acb_simpleRandomBytes(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_simpleRandomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_pseudoRandomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("crypto.pseudoRandomBytes");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        acb_pseudoRandomBytes(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_pseudoRandomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_randomArt(const v8::FunctionCallbackInfo<v8::Value>& args)
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

inline void crypto_base::s_pbkdf1(const v8::FunctionCallbackInfo<v8::Value>& args)
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

    if (!cb.IsEmpty()) {
        acb_pbkdf1(v0, v1, v2, v3, v4, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_pbkdf1(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(exlib::string, 4);

    if (!cb.IsEmpty()) {
        acb_pbkdf1(v0, v1, v2, v3, v4, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_pbkdf1(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args)
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

    if (!cb.IsEmpty()) {
        acb_pbkdf2(v0, v1, v2, v3, v4, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_pbkdf2(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(exlib::string, 4);

    if (!cb.IsEmpty()) {
        acb_pbkdf2(v0, v1, v2, v3, v4, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_pbkdf2(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}
}

#endif
