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

    static ClassData::ClassConst s_const[] = {
        { "AES", _AES },
        { "CAMELLIA", _CAMELLIA },
        { "DES", _DES },
        { "DES_EDE", _DES_EDE },
        { "DES_EDE3", _DES_EDE3 },
        { "BLOWFISH", _BLOWFISH },
        { "ARC4", _ARC4 },
        { "ECB", _ECB },
        { "CBC", _CBC },
        { "CFB64", _CFB64 },
        { "CFB128", _CFB128 },
        { "OFB", _OFB },
        { "CTR", _CTR },
        { "GCM", _GCM },
        { "STREAM", _STREAM },
        { "CCM", _CCM },
        { "PKCS7", _PKCS7 },
        { "ONE_AND_ZEROS", _ONE_AND_ZEROS },
        { "ZEROS_AND_LEN", _ZEROS_AND_LEN },
        { "ZEROS", _ZEROS },
        { "NOPADDING", _NOPADDING }
    };

    static ClassData s_cd = {
        "crypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
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
