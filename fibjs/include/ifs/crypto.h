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
class ECDH_base;
class Digest_base;
class Buffer_base;
class Cipher_base;
class Sign_base;
class Verify_base;
class X509CertificateRequest_base;
class webcrypto_base;
class subtle_base;

class crypto_base : public object_base {
    DECLARE_CLASS(crypto_base);

public:
    using Union_createHmac_key = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, exlib::string>;
    using Union_getCipherInfo_nameOrNid = std::variant<exlib::string, int32_t>;
    using Union_createCipher_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_createCipheriv_key = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, exlib::string>;
    using Union_createCipheriv_iv = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_createDecipher_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_createDecipheriv_key = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, exlib::string>;
    using Union_createDecipheriv_iv = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_createPrivateKey_key = std::variant<obj_ptr<Buffer_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_createPublicKey_key = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_createSecretKey_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_createCertificateRequest_csr = std::variant<obj_ptr<Buffer_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_hash_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_randomFill_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_hkdf_password = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_hkdf_salt = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_hkdf_info = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_pbkdf2_password = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_pbkdf2_salt = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_scrypt_password = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_scrypt_salt = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_privateDecrypt_privateKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_privateDecrypt_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_privateEncrypt_privateKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_privateEncrypt_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_publicDecrypt_publicKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_publicDecrypt_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_publicEncrypt_publicKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_publicEncrypt_buffer = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_sign_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_sign_key = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_verify_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_verify_key = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_verify_signature = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_timingSafeEqual_a = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_timingSafeEqual_b = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_bbsSign_messages = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_bbsSign_privateKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_bbsVerify_messages = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_bbsVerify_publicKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_bbsVerify_signature = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_proofGen_signature = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_proofGen_messages = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_proofGen_publicKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_proofVerify_messages = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_proofVerify_publicKey = std::variant<obj_ptr<Buffer_base>, obj_ptr<KeyObject_base>, v8::Local<v8::Object>, exlib::string>;
    using Union_proofVerify_proof = std::variant<obj_ptr<Buffer_base>, exlib::string>;

public:
    class GetCipherInfoType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("name"), GetReturnValue(isolate, name)).Check();
            retVal->Set(context, isolate->NewString("nid"), GetReturnValue(isolate, nid)).Check();
            retVal->Set(context, isolate->NewString("blockSize"), GetReturnValue(isolate, blockSize)).Check();
            retVal->Set(context, isolate->NewString("ivLength"), GetReturnValue(isolate, ivLength)).Check();
            retVal->Set(context, isolate->NewString("keyLength"), GetReturnValue(isolate, keyLength)).Check();
            retVal->Set(context, isolate->NewString("mode"), GetReturnValue(isolate, mode)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, name));
            args.push_back(GetReturnValue(isolate, nid));
            args.push_back(GetReturnValue(isolate, blockSize));
            args.push_back(GetReturnValue(isolate, ivLength));
            args.push_back(GetReturnValue(isolate, keyLength));
            args.push_back(GetReturnValue(isolate, mode));
        }

    public:
        exlib::string name;
        int32_t nid;
        int32_t blockSize;
        int32_t ivLength;
        int32_t keyLength;
        exlib::string mode;
    };
    class GenerateKeyPairType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
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
    static result_t getHashes(std::vector<exlib::string>& retVal);
    static result_t createECDH(exlib::string curve, obj_ptr<ECDH_base>& retVal);
    static result_t createHash(exlib::string algo, obj_ptr<Digest_base>& retVal);
    static result_t createHmac(exlib::string algo, Union_createHmac_key key, obj_ptr<Digest_base>& retVal);
    static result_t getCiphers(std::vector<exlib::string>& retVal);
    static result_t getCipherInfo(Union_getCipherInfo_nameOrNid nameOrNid, v8::Local<v8::Object> options, obj_ptr<GetCipherInfoType>& retVal);
    static result_t createCipher(exlib::string algorithm, Union_createCipher_key key, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createCipheriv(exlib::string algorithm, Union_createCipheriv_key key, Union_createCipheriv_iv iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipher(exlib::string algorithm, Union_createDecipher_key key, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t createDecipheriv(exlib::string algorithm, Union_createDecipheriv_key key, Union_createDecipheriv_iv iv, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal);
    static result_t getCurves(std::vector<exlib::string>& retVal);
    static result_t createPrivateKey(Union_createPrivateKey_key key, obj_ptr<KeyObject_base>& retVal);
    static result_t createPublicKey(Union_createPublicKey_key key, obj_ptr<KeyObject_base>& retVal);
    static result_t createSign(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Sign_base>& retVal);
    static result_t createVerify(exlib::string algorithm, v8::Local<v8::Object> options, obj_ptr<Verify_base>& retVal);
    static result_t createSecretKey(Union_createSecretKey_key key, exlib::string encoding, obj_ptr<KeyObject_base>& retVal);
    static result_t createCertificateRequest(Union_createCertificateRequest_csr csr, obj_ptr<X509CertificateRequest_base>& retVal);
    static result_t diffieHellman(v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal);
    static result_t hash(exlib::string algorithm, Union_hash_data data, exlib::string outputEncoding, v8::Local<v8::Value>& retVal);
    static result_t randomBytes(int32_t size, obj_ptr<Buffer_base>& retVal);
    static result_t randomFill(Union_randomFill_buffer buffer, int32_t offset, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t getRandomValues(v8::Local<v8::TypedArray> data, v8::Local<v8::TypedArray>& retVal);
    static result_t randomUUID(v8::Local<v8::Object> options, exlib::string& retVal);
    static result_t generateKeyPair(exlib::string type, v8::Local<v8::Object> options, obj_ptr<GenerateKeyPairType>& retVal, AsyncEvent* ac);
    static result_t hkdf(exlib::string algoName, Union_hkdf_password password, Union_hkdf_salt salt, Union_hkdf_info info, int32_t size, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t pbkdf2(Union_pbkdf2_password password, Union_pbkdf2_salt salt, int32_t iterations, int32_t size, exlib::string algoName, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t scrypt(Union_scrypt_password password, Union_scrypt_salt salt, int32_t keylen, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t privateDecrypt(Union_privateDecrypt_privateKey privateKey, Union_privateDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal);
    static result_t privateEncrypt(Union_privateEncrypt_privateKey privateKey, Union_privateEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicDecrypt(Union_publicDecrypt_publicKey publicKey, Union_publicDecrypt_buffer buffer, obj_ptr<Buffer_base>& retVal);
    static result_t publicEncrypt(Union_publicEncrypt_publicKey publicKey, Union_publicEncrypt_buffer buffer, obj_ptr<Buffer_base>& retVal);
    static result_t sign(v8::Local<v8::Value> algorithm, Union_sign_data data, Union_sign_key key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t verify(v8::Local<v8::Value> algorithm, Union_verify_data data, Union_verify_key key, Union_verify_signature signature, bool& retVal, AsyncEvent* ac);
    static result_t timingSafeEqual(Union_timingSafeEqual_a a, Union_timingSafeEqual_b b, bool& retVal);
    static result_t bbsSign(std::vector<Union_bbsSign_messages>& messages, Union_bbsSign_privateKey privateKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t bbsVerify(std::vector<Union_bbsVerify_messages>& messages, Union_bbsVerify_publicKey publicKey, Union_bbsVerify_signature signature, bool& retVal, AsyncEvent* ac);
    static result_t proofGen(Union_proofGen_signature signature, std::vector<Union_proofGen_messages>& messages, std::vector<int32_t>& index, Union_proofGen_publicKey publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t proofVerify(std::vector<Union_proofVerify_messages>& messages, std::vector<int32_t>& index, Union_proofVerify_publicKey publicKey, Union_proofVerify_proof proof, bool& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<crypto_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_getHashes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createECDH(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createHash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createHmac(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getCiphers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getCipherInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_getRandomValues(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_randomUUID(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_generateKeyPair(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_hkdf(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_scrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_privateDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_privateEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_publicDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_publicEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_verify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_timingSafeEqual(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bbsSign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bbsVerify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_proofGen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_proofVerify(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE4(crypto_base, randomFill, Union_randomFill_buffer, int32_t, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE3(crypto_base, generateKeyPair, exlib::string, v8::Local<v8::Object>, obj_ptr<GenerateKeyPairType>);
    ASYNC_STATICVALUE6(crypto_base, hkdf, exlib::string, Union_hkdf_password, Union_hkdf_salt, Union_hkdf_info, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE6(crypto_base, pbkdf2, Union_pbkdf2_password, Union_pbkdf2_salt, int32_t, int32_t, exlib::string, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, scrypt, Union_scrypt_password, Union_scrypt_salt, int32_t, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, sign, v8::Local<v8::Value>, Union_sign_data, Union_sign_key, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, verify, v8::Local<v8::Value>, Union_verify_data, Union_verify_key, Union_verify_signature, bool);
    ASYNC_STATICVALUE3(crypto_base, bbsSign, std::vector<Union_bbsSign_messages>, Union_bbsSign_privateKey, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE4(crypto_base, bbsVerify, std::vector<Union_bbsVerify_messages>, Union_bbsVerify_publicKey, Union_bbsVerify_signature, bool);
    ASYNC_STATICVALUE5(crypto_base, proofGen, Union_proofGen_signature, std::vector<Union_proofGen_messages>, std::vector<int32_t>, Union_proofGen_publicKey, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE5(crypto_base, proofVerify, std::vector<Union_proofVerify_messages>, std::vector<int32_t>, Union_proofVerify_publicKey, Union_proofVerify_proof, bool);
};
}

#include "ifs/crypto_constants.h"
#include "ifs/KeyObject.h"
#include "ifs/X509Certificate.h"
#include "ifs/ECDH.h"
#include "ifs/Digest.h"
#include "ifs/Buffer.h"
#include "ifs/Cipher.h"
#include "ifs/Sign.h"
#include "ifs/Verify.h"
#include "ifs/X509CertificateRequest.h"
#include "ifs/webcrypto.h"
#include "ifs/subtle.h"

namespace fibjs {
inline ClassInfo& crypto_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getHashes", s_static_getHashes, true, ClassData::ASYNC_SYNC },
        { "createECDH", s_static_createECDH, true, ClassData::ASYNC_SYNC },
        { "createHash", s_static_createHash, true, ClassData::ASYNC_SYNC },
        { "createHmac", s_static_createHmac, true, ClassData::ASYNC_SYNC },
        { "getCiphers", s_static_getCiphers, true, ClassData::ASYNC_SYNC },
        { "getCipherInfo", s_static_getCipherInfo, true, ClassData::ASYNC_SYNC },
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
        { "randomBytes", s_static_randomBytes, true, ClassData::ASYNC_SYNC },
        { "randomFill", s_static_randomFill, true, ClassData::ASYNC_ASYNC },
        { "getRandomValues", s_static_getRandomValues, true, ClassData::ASYNC_SYNC },
        { "randomUUID", s_static_randomUUID, true, ClassData::ASYNC_SYNC },
        { "generateKeyPair", s_static_generateKeyPair, true, ClassData::ASYNC_ASYNC },
        { "hkdf", s_static_hkdf, true, ClassData::ASYNC_ASYNC },
        { "pbkdf2", s_static_pbkdf2, true, ClassData::ASYNC_ASYNC },
        { "scrypt", s_static_scrypt, true, ClassData::ASYNC_ASYNC },
        { "privateDecrypt", s_static_privateDecrypt, true, ClassData::ASYNC_SYNC },
        { "privateEncrypt", s_static_privateEncrypt, true, ClassData::ASYNC_SYNC },
        { "publicDecrypt", s_static_publicDecrypt, true, ClassData::ASYNC_SYNC },
        { "publicEncrypt", s_static_publicEncrypt, true, ClassData::ASYNC_SYNC },
        { "sign", s_static_sign, true, ClassData::ASYNC_ASYNC },
        { "verify", s_static_verify, true, ClassData::ASYNC_ASYNC },
        { "timingSafeEqual", s_static_timingSafeEqual, true, ClassData::ASYNC_SYNC },
        { "bbsSign", s_static_bbsSign, true, ClassData::ASYNC_ASYNC },
        { "bbsVerify", s_static_bbsVerify, true, ClassData::ASYNC_ASYNC },
        { "proofGen", s_static_proofGen, true, ClassData::ASYNC_ASYNC },
        { "proofVerify", s_static_proofVerify, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", crypto_constants_base::class_info },
        { "KeyObject", KeyObject_base::class_info },
        { "X509Certificate", X509Certificate_base::class_info },
        { "webcrypto", webcrypto_base::class_info },
        { "subtle", subtle_base::class_info }
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
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getHashes(vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createECDH(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ECDH_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = createECDH(v0, vr);

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
    ARG(Union_createHmac_key, 1);

    hr = createHmac(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_getCiphers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = getCiphers(vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_getCipherInfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GetCipherInfoType> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(Union_getCipherInfo_nameOrNid, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = getCipherInfo(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createCipher(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(Union_createCipher_key, 1);
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
    ARG(Union_createCipheriv_key, 1);
    ARG(Union_createCipheriv_iv, 2);
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
    ARG(Union_createDecipher_key, 1);
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
    ARG(Union_createDecipheriv_key, 1);
    ARG(Union_createDecipheriv_iv, 2);
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    hr = createDecipheriv(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_getCurves(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

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

    ARG(Union_createPrivateKey_key, 0);

    hr = createPrivateKey(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<KeyObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_createPublicKey_key, 0);

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

    ARG(Union_createSecretKey_key, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = createSecretKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_createCertificateRequest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509CertificateRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_createCertificateRequest_csr, 0);

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
    ARG(Union_hash_data, 1);
    OPT_ARG(exlib::string, 2, "hex");

    hr = hash(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomBytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 16);

    hr = randomBytes(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomFill(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.randomFill");

    METHOD_OVER(3, 1);

    ARG(Union_randomFill_buffer, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_randomFill(v0, v1, v2, cb, args);
    else
        hr = ac_randomFill(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_getRandomValues(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::TypedArray> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::TypedArray>, 0);

    hr = getRandomValues(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_randomUUID(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = randomUUID(v0, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_generateKeyPair(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GenerateKeyPairType> vr;

    ASYNC_METHOD_ENTER("crypto.generateKeyPair");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_generateKeyPair(v0, v1, cb, args);
    else
        hr = ac_generateKeyPair(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_hkdf(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.hkdf");

    METHOD_OVER(5, 5);

    ARG(exlib::string, 0);
    ARG(Union_hkdf_password, 1);
    ARG(Union_hkdf_salt, 2);
    ARG(Union_hkdf_info, 3);
    ARG(int32_t, 4);

    if (!cb.IsEmpty())
        hr = acb_hkdf(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_hkdf(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_pbkdf2(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.pbkdf2");

    METHOD_OVER(5, 5);

    ARG(Union_pbkdf2_password, 0);
    ARG(Union_pbkdf2_salt, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    ARG(exlib::string, 4);

    if (!cb.IsEmpty())
        hr = acb_pbkdf2(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_pbkdf2(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_scrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.scrypt");

    METHOD_OVER(4, 3);

    ARG(Union_scrypt_password, 0);
    ARG(Union_scrypt_salt, 1);
    ARG(int32_t, 2);
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_scrypt(v0, v1, v2, v3, cb, args);
    else
        hr = ac_scrypt(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_privateDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_privateDecrypt_privateKey, 0);
    ARG(Union_privateDecrypt_buffer, 1);

    hr = privateDecrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_privateEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_privateEncrypt_privateKey, 0);
    ARG(Union_privateEncrypt_buffer, 1);

    hr = privateEncrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_publicDecrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_publicDecrypt_publicKey, 0);
    ARG(Union_publicDecrypt_buffer, 1);

    hr = publicDecrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_publicEncrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_publicEncrypt_publicKey, 0);
    ARG(Union_publicEncrypt_buffer, 1);

    hr = publicEncrypt(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.sign");

    METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Value>, 0);
    ARG(Union_sign_data, 1);
    ARG(Union_sign_key, 2);

    if (!cb.IsEmpty())
        hr = acb_sign(v0, v1, v2, cb, args);
    else
        hr = ac_sign(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_ENTER("crypto.verify");

    METHOD_OVER(4, 4);

    ARG(v8::Local<v8::Value>, 0);
    ARG(Union_verify_data, 1);
    ARG(Union_verify_key, 2);
    ARG(Union_verify_signature, 3);

    if (!cb.IsEmpty())
        hr = acb_verify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_verify(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_timingSafeEqual(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_timingSafeEqual_a, 0);
    ARG(Union_timingSafeEqual_b, 1);

    hr = timingSafeEqual(v0, v1, vr);

    METHOD_RETURN();
}

inline void crypto_base::s_static_bbsSign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.bbsSign");

    METHOD_OVER(2, 2);

    ARG(std::vector<Union_bbsSign_messages>, 0);
    ARG(Union_bbsSign_privateKey, 1);

    if (!cb.IsEmpty())
        hr = acb_bbsSign(v0, v1, cb, args);
    else
        hr = ac_bbsSign(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_bbsVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_ENTER("crypto.bbsVerify");

    METHOD_OVER(3, 3);

    ARG(std::vector<Union_bbsVerify_messages>, 0);
    ARG(Union_bbsVerify_publicKey, 1);
    ARG(Union_bbsVerify_signature, 2);

    if (!cb.IsEmpty())
        hr = acb_bbsVerify(v0, v1, v2, cb, args);
    else
        hr = ac_bbsVerify(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_proofGen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("crypto.proofGen");

    METHOD_OVER(4, 4);

    ARG(Union_proofGen_signature, 0);
    ARG(std::vector<Union_proofGen_messages>, 1);
    ARG(std::vector<int32_t>, 2);
    ARG(Union_proofGen_publicKey, 3);

    if (!cb.IsEmpty())
        hr = acb_proofGen(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofGen(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}

inline void crypto_base::s_static_proofVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_ENTER("crypto.proofVerify");

    METHOD_OVER(4, 4);

    ARG(std::vector<Union_proofVerify_messages>, 0);
    ARG(std::vector<int32_t>, 1);
    ARG(Union_proofVerify_publicKey, 2);
    ARG(Union_proofVerify_proof, 3);

    if (!cb.IsEmpty())
        hr = acb_proofVerify(v0, v1, v2, v3, cb, args);
    else
        hr = ac_proofVerify(v0, v1, v2, v3, vr);

    ASYNC_METHOD_RETURN();
}
}
