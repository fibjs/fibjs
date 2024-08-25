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

class crypto_constants_base;
class KeyObject_base;
class X509Certificate_base;
class Digest_base;
class Buffer_base;
class Cipher_base;
class Sign_base;
class Verify_base;
class X509CertificateRequest_base;
class webcrypto_base;

class crypto_base : public object_base {
    DECLARE_CLASS(crypto_base);

public:
    class GenerateKeyPairType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("publicKey"), GetReturnValue(isolate, publicKey)).Check();
            retVal->Set(context, isolate->NewString("privateKey"), GetReturnValue(isolate, privateKey)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, publicKey));
            args.push_back(GetReturnValue(isolate, privateKey));
        }

    public:
        Variant publicKey;
        Variant privateKey;
    };

public:
    // crypto_base
    static result_t getHashes(v8::Local<v8::Array>& retVal);
    static result_t createHash(exlib::string algo, obj_ptr<Digest_base>& retVal);
    static result_t createHmac(exlib::string algo, Buffer_base* key, obj_ptr<Digest_base>& retVal);
    static result_t getCiphers(v8::Local<v8::Array>& retVal);
    static result_t createCipher(exlib::string algorithm, Buffer_base* key, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createCipheriv(exlib::string algorithm, Buffer_base* key, Buffer_base* iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createCipheriv(exlib::string algorithm, KeyObject_base* key, Buffer_base* iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipher(exlib::string algorithm, Buffer_base* key, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipheriv(exlib::string algorithm, Buffer_base* key, Buffer_base* iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipheriv(exlib::string algorithm, KeyObject_base* key, Buffer_base* iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t getCurves(v8::Local<v8::Array>& retVal);
    static result_t createPrivateKey(Buffer_base* key, obj_ptr<KeyObject_base>& retVal);
    static result_t createPrivateKey(v8::Local<v8::Object> key, obj_ptr<KeyObject_base>& retVal);
    static result_t createPublicKey(Buffer_base* key, obj_ptr<KeyObject_base>& retVal);
    static result_t createPublicKey(KeyObject_base* key, obj_ptr<KeyObject_base>& retVal);
    static result_t createPublicKey(v8::Local<v8::Object> key, obj_ptr<KeyObject_base>& retVal);
    static result_t createSign(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Sign_base>& retVal);
    static result_t createVerify(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Verify_base>& retVal);
    static result_t createSecretKey(Buffer_base* key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal);
    static result_t createSecretKey(exlib::string key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal);
    static result_t createCertificateRequest(Buffer_base* csr, obj_ptr<X509CertificateRequest_base>& retVal);
    static result_t createCertificateRequest(v8::Local<v8::Object> options, obj_ptr<X509CertificateRequest_base>& retVal);
    static result_t diffieHellman(v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal);
    static result_t hash(exlib::string algorithm, Buffer_base* data, exlib::string outputEncoding, v8::Local<v8::Value>& retVal);
    static result_t randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t randomFill(Buffer_base* buffer, int32_t offset, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t generateKeyPair(exlib::string type, v8::Local<v8::Object> options, obj_ptr<GenerateKeyPairType>& retVal, AsyncEvent* ac);
    static result_t hkdf(exlib::string algoName, Buffer_base* password, Buffer_base* salt, Buffer_base* info, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf2(Buffer_base* password, Buffer_base* salt, int32_t iterations, int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t privateDecrypt(Buffer_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t privateDecrypt(KeyObject_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t privateDecrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal);
    static result_t privateEncrypt(Buffer_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t privateEncrypt(KeyObject_base* privateKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t privateEncrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicDecrypt(Buffer_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicDecrypt(KeyObject_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicDecrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicEncrypt(Buffer_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicEncrypt(KeyObject_base* publicKey, Buffer_base* buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicEncrypt(v8::Local<v8::Object> key, v8::Local<v8::Value> buffer, obj_ptr<Buffer_base>& retVal);
    static result_t sign(v8::Local<v8::Value> algorithm, Buffer_base* data, Buffer_base* privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t sign(v8::Local<v8::Value> algorithm, Buffer_base* data, KeyObject_base* privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t sign(v8::Local<v8::Value> algorithm, Buffer_base* data, v8::Local<v8::Object> key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t verify(v8::Local<v8::Value> algorithm, Buffer_base* data, Buffer_base* publicKey, Buffer_base* signature, bool& retVal, AsyncEvent* ac);
    static result_t verify(v8::Local<v8::Value> algorithm, Buffer_base* data, KeyObject_base* publicKey, Buffer_base* signature, bool& retVal, AsyncEvent* ac);
    static result_t verify(v8::Local<v8::Value> algorithm, Buffer_base* data, v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal, AsyncEvent* ac);
    static result_t bbsSign(v8::Local<v8::Array> messages, Buffer_base* privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t bbsSign(v8::Local<v8::Array> messages, KeyObject_base* privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t bbsSign(v8::Local<v8::Array> messages, v8::Local<v8::Object> key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t bbsVerify(v8::Local<v8::Array> messages, Buffer_base* publicKey, Buffer_base* signature, bool& retVal, AsyncEvent* ac);
    static result_t bbsVerify(v8::Local<v8::Array> messages, KeyObject_base* publicKey, Buffer_base* signature, bool& retVal, AsyncEvent* ac);
    static result_t bbsVerify(v8::Local<v8::Array> messages, v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal, AsyncEvent* ac);
    static result_t proofGen(Buffer_base* signature, v8::Local<v8::Array> messages, v8::Local<v8::Array> index, Buffer_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t proofGen(Buffer_base* signature, v8::Local<v8::Array> messages, v8::Local<v8::Array> index, KeyObject_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t proofGen(Buffer_base* signature, v8::Local<v8::Array> messages, v8::Local<v8::Array> index, v8::Local<v8::Object> key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t proofVerify(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, Buffer_base* publicKey, Buffer_base* proof, bool& retVal, AsyncEvent* ac);
    static result_t proofVerify(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, KeyObject_base* publicKey, Buffer_base* proof, bool& retVal, AsyncEvent* ac);
    static result_t proofVerify(v8::Local<v8::Array> messages, v8::Local<v8::Array> index, v8::Local<v8::Object> key, Buffer_base* proof, bool& retVal, AsyncEvent* ac);

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
    static void s_static_getCurves(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createSign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createVerify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createSecretKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createCertificateRequest(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_diffieHellman(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomFill(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_generateKeyPair(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hkdf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_privateDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_privateEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_publicDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_publicEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_verify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bbsSign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bbsVerify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_proofGen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_proofVerify(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(crypto_base, randomBytes, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, randomFill, Buffer_base*, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE3(crypto_base, generateKeyPair, exlib::string, v8::Local<v8::Object>, obj_ptr<GenerateKeyPairType>);
    ASYNC_STATICVALUE6(crypto_base, hkdf, exlib::string, Buffer_base*, Buffer_base*, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Buffer_base*, Buffer_base*, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, sign, v8::Local<v8::Value>, Buffer_base*, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, sign, v8::Local<v8::Value>, Buffer_base*, KeyObject_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, sign, v8::Local<v8::Value>, Buffer_base*, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, verify, v8::Local<v8::Value>, Buffer_base*, Buffer_base*, Buffer_base*, bool);
    ASYNC_STATICVALUE5(crypto_base, verify, v8::Local<v8::Value>, Buffer_base*, KeyObject_base*, Buffer_base*, bool);
    ASYNC_STATICVALUE5(crypto_base, verify, v8::Local<v8::Value>, Buffer_base*, v8::Local<v8::Object>, Buffer_base*, bool);
    ASYNC_STATICVALUE3(crypto_base, bbsSign, v8::Local<v8::Array>, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE3(crypto_base, bbsSign, v8::Local<v8::Array>, KeyObject_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE3(crypto_base, bbsSign, v8::Local<v8::Array>, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, bbsVerify, v8::Local<v8::Array>, Buffer_base*, Buffer_base*, bool);
    ASYNC_STATICVALUE4(crypto_base, bbsVerify, v8::Local<v8::Array>, KeyObject_base*, Buffer_base*, bool);
    ASYNC_STATICVALUE4(crypto_base, bbsVerify, v8::Local<v8::Array>, v8::Local<v8::Object>, Buffer_base*, bool);
    ASYNC_STATICVALUE5(crypto_base, proofGen, Buffer_base*, v8::Local<v8::Array>, v8::Local<v8::Array>, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, proofGen, Buffer_base*, v8::Local<v8::Array>, v8::Local<v8::Array>, KeyObject_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, proofGen, Buffer_base*, v8::Local<v8::Array>, v8::Local<v8::Array>, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, proofVerify, v8::Local<v8::Array>, v8::Local<v8::Array>, Buffer_base*, Buffer_base*, bool);
    ASYNC_STATICVALUE5(crypto_base, proofVerify, v8::Local<v8::Array>, v8::Local<v8::Array>, KeyObject_base*, Buffer_base*, bool);
    ASYNC_STATICVALUE5(crypto_base, proofVerify, v8::Local<v8::Array>, v8::Local<v8::Array>, v8::Local<v8::Object>, Buffer_base*, bool);
};
}

#include "ifs/crypto_constants.h"
#include "ifs/KeyObject.h"
#include "ifs/X509Certificate.h"
#include "ifs/Digest.h"
#include "ifs/Buffer.h"
#include "ifs/Cipher.h"
#include "ifs/Sign.h"
#include "ifs/Verify.h"
#include "ifs/X509CertificateRequest.h"
#include "ifs/webcrypto.h"

namespace fibjs {
inline ClassInfo& crypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getHashes", s_static_getHashes, true, ClassData::ASYNC_SYNC },
        { "createHash", s_static_createHash, true, ClassData::ASYNC_SYNC },
        { "createHmac", s_static_createHmac, true, ClassData::ASYNC_SYNC },
        { "getCiphers", s_static_getCiphers, true, ClassData::ASYNC_SYNC },
        { "createCipher", s_static_createCipher, true, ClassData::ASYNC_SYNC },
        { "createCipheriv", s_static_createCipheriv, true, ClassData::ASYNC_SYNC },
        { "createDecipher", s_static_createDecipher, true, ClassData::ASYNC_SYNC },
        { "createDecipheriv", s_static_createDecipheriv, true, ClassData::ASYNC_SYNC },
        { "getCurves", s_static_getCurves, true, ClassData::ASYNC_SYNC },
        { "createPrivateKey", s_static_createPrivateKey, true, ClassData::ASYNC_SYNC },
        { "createPublicKey", s_static_createPublicKey, true, ClassData::ASYNC_SYNC },
        { "createSign", s_static_createSign, true, ClassData::ASYNC_SYNC },
        { "createVerify", s_static_createVerify, true, ClassData::ASYNC_SYNC },
        { "createSecretKey", s_static_createSecretKey, true, ClassData::ASYNC_SYNC },
        { "createCertificateRequest", s_static_createCertificateRequest, true, ClassData::ASYNC_SYNC },
        { "diffieHellman", s_static_diffieHellman, true, ClassData::ASYNC_SYNC },
        { "hash", s_static_hash, true, ClassData::ASYNC_SYNC },
        { "randomBytes", s_static_randomBytes, true, ClassData::ASYNC_ASYNC },
        { "randomBytesSync", s_static_randomBytes, true, ClassData::ASYNC_SYNC },
        { "randomFill", s_static_randomFill, true, ClassData::ASYNC_ASYNC },
        { "randomFillSync", s_static_randomFill, true, ClassData::ASYNC_SYNC },
        { "generateKeyPair", s_static_generateKeyPair, true, ClassData::ASYNC_ASYNC },
        { "generateKeyPairSync", s_static_generateKeyPair, true, ClassData::ASYNC_SYNC },
        { "hkdf", s_static_hkdf, true, ClassData::ASYNC_ASYNC },
        { "hkdfSync", s_static_hkdf, true, ClassData::ASYNC_SYNC },
        { "pbkdf2", s_static_pbkdf2, true, ClassData::ASYNC_ASYNC },
        { "pbkdf2Sync", s_static_pbkdf2, true, ClassData::ASYNC_SYNC },
        { "privateDecrypt", s_static_privateDecrypt, true, ClassData::ASYNC_SYNC },
        { "privateEncrypt", s_static_privateEncrypt, true, ClassData::ASYNC_SYNC },
        { "publicDecrypt", s_static_publicDecrypt, true, ClassData::ASYNC_SYNC },
        { "publicEncrypt", s_static_publicEncrypt, true, ClassData::ASYNC_SYNC },
        { "sign", s_static_sign, true, ClassData::ASYNC_ASYNC },
        { "signSync", s_static_sign, true, ClassData::ASYNC_SYNC },
        { "verify", s_static_verify, true, ClassData::ASYNC_ASYNC },
        { "verifySync", s_static_verify, true, ClassData::ASYNC_SYNC },
        { "bbsSign", s_static_bbsSign, true, ClassData::ASYNC_ASYNC },
        { "bbsSignSync", s_static_bbsSign, true, ClassData::ASYNC_SYNC },
        { "bbsVerify", s_static_bbsVerify, true, ClassData::ASYNC_ASYNC },
        { "bbsVerifySync", s_static_bbsVerify, true, ClassData::ASYNC_SYNC },
        { "proofGen", s_static_proofGen, true, ClassData::ASYNC_ASYNC },
        { "proofGenSync", s_static_proofGen, true, ClassData::ASYNC_SYNC },
        { "proofVerify", s_static_proofVerify, true, ClassData::ASYNC_ASYNC },
        { "proofVerifySync", s_static_proofVerify, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", crypto_constants_base::class_info },
        { "KeyObject", KeyObject_base::class_info },
        { "X509Certificate", X509Certificate_base::class_info },
        { "webcrypto", webcrypto_base::class_info }
    };

    static ClassData s_cd = {
        "crypto", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
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

    METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<KeyObject_base>, 1);
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

    METHOD_OVER(4, 3);

    ARG(exlib::string, 0);
    ARG(obj_ptr<KeyObject_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    hr = createDecipheriv(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_getCurves(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getCurves(vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = createPrivateKey(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = createPrivateKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = createPublicKey(v0, vr);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<KeyObject_base>, 0);

    hr = createPublicKey(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = createPublicKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createSign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Sign_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = createSign(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Verify_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = createVerify(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createSecretKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "buffer");

    hr = createSecretKey(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = createSecretKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createCertificateRequest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509CertificateRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = createCertificateRequest(v0, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = createCertificateRequest(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_diffieHellman(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = diffieHellman(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_hash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(exlib::string, 2, "hex");

    hr = hash(v0, v1, v2, vr);

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

inline void crypto_base::s_static_generateKeyPair(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GenerateKeyPairType> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_generateKeyPair(v0, v1, cb, args);
    else
        hr = ac_generateKeyPair(v0, v1, vr);

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

inline void crypto_base::s_static_privateDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = privateDecrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<KeyObject_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = privateDecrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = privateDecrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_privateEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = privateEncrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<KeyObject_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = privateEncrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = privateEncrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_publicDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = publicDecrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<KeyObject_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = publicDecrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = publicDecrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_publicEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = publicEncrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<KeyObject_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = publicEncrypt(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Value>, 1);

    hr = publicEncrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_sign(v0, v1, v2, cb, args);
    else
        hr = ac_sign(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<KeyObject_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_sign(v0, v1, v2, cb, args);
    else
        hr = ac_sign(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(v8::Local<v8::Object>, 2);

    if (!cb.IsEmpty())
        hr = acb_sign(v0, v1, v2, cb, args);
    else
        hr = ac_sign(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Value>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_verify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_verify(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Value>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<KeyObject_base>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_verify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_verify(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Value>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(v8::Local<v8::Object>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_verify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_verify(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_bbsSign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_bbsSign(v0, v1, cb, args);
    else
        hr = ac_bbsSign(v0, v1, vr);

    ASYNC_METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(obj_ptr<KeyObject_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_bbsSign(v0, v1, cb, args);
    else
        hr = ac_bbsSign(v0, v1, vr);

    ASYNC_METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Object>, 1);

    if (!cb.IsEmpty())
        hr = acb_bbsSign(v0, v1, cb, args);
    else
        hr = ac_bbsSign(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_bbsVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Array>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_bbsVerify(v0, v1, v2, cb, args);
    else
        hr = ac_bbsVerify(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Array>, 0);
    ARG(obj_ptr<KeyObject_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_bbsVerify(v0, v1, v2, cb, args);
    else
        hr = ac_bbsVerify(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_bbsVerify(v0, v1, v2, cb, args);
    else
        hr = ac_bbsVerify(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_proofGen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(4, 4);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(v8::Local<v8::Array>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_proofGen(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofGen(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(4, 4);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(v8::Local<v8::Array>, 2);
    ARG(obj_ptr<KeyObject_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_proofGen(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofGen(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(4, 4);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(v8::Local<v8::Array>, 2);
    ARG(v8::Local<v8::Object>, 3);

    if (!cb.IsEmpty())
        hr = acb_proofGen(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofGen(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_proofVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_proofVerify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofVerify(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(obj_ptr<KeyObject_base>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_proofVerify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofVerify(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(v8::Local<v8::Object>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    if (!cb.IsEmpty())
        hr = acb_proofVerify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofVerify(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}
}
