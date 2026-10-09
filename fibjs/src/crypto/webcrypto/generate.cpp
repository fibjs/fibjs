/*
 * generate.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/subtle.h"
#include "ifs/crypto.h"
#include "CryptoKey.h"
#include "crypto_util.h"

namespace fibjs {

result_t CryptoKey::generate()
{
    if (m_key_type == kKeyNameECDSA)
        return generate_ecdsa();
    if (m_key_type == kKeyNameEd25519)
        return generate_ed25519();
    if (m_key_type == kKeyNameECDH)
        return generate_ecdh();
    if (m_key_type == kKeyNameHMAC)
        return generate_hmac();
    return 0;
}

// WebCrypto: the key material of an HMAC key is `length` random bits; `length`
// defaults to the block size of the hash, in bits. The parameters (hash, and
// the optional length) were read in the sync phase, so this runs without a JS
// scope.
result_t CryptoKey::generate_hmac()
{
    int32_t length = 0;
    Variant v;

    if (m_algorithm->get("length", v) >= 0)
        length = v.intVal();

    if (length <= 0) {
        Variant hash;
        obj_ptr<NObject> hashObj;

        if (m_algorithm->get("hash", hash) < 0 || (hashObj = (NObject*)hash.object()) == NULL)
            return Runtime::setError("WebCrypto: HMAC requires hash parameter");

        Variant hashName;
        if (hashObj->get("name", hashName) < 0)
            return Runtime::setError("WebCrypto: HMAC requires hash parameter");

        const EVP_MD* md = _evp_md_type(hashName.string().c_str());
        if (md == NULL)
            return Runtime::setError("WebCrypto: unknown hash algorithm: " + hashName.string());

        length = EVP_MD_block_size(md) * 8;
    }

    if (length % 8 != 0)
        return Runtime::setError("WebCrypto: HMAC key length must be a multiple of 8 bits");

    // the resolved length is part of the key algorithm (WebCrypto)
    m_algorithm->add("length", length);

    std::vector<uint8_t> key(length / 8);

    result_t hr = randomBytes(key.data(), (int32_t)key.size());
    if (hr < 0)
        return hr;

    m_key = new KeyObject();

    return m_key->createSecretKey(key.data(), key.size());
}

result_t CryptoKey::createPublicKey()
{
    obj_ptr<KeyObject_base> publicKey;
    result_t hr = crypto_base::createPublicKey(
        crypto_base::Union_createPublicKey_key(obj_ptr<KeyObject_base>(m_key.get())), publicKey);
    if (hr < 0)
        return hr;

    m_publicKey = new CryptoKey();

    m_publicKey->m_key_type = m_key_type;
    m_publicKey->m_key = publicKey.As<KeyObject>();
    m_publicKey->m_algorithm = m_algorithm;
    m_publicKey->m_extractable = true;

    // Handle usage distribution based on key type
    if (m_key_type == kKeyNameECDSA || m_key_type == kKeyNameEd25519) {
        // For signing algorithms, move 'verify' usage to public key
        auto it = m_usageMap.find("verify");
        if (it != m_usageMap.end()) {
            m_publicKey->m_usageMap.emplace("verify", true);
            m_usageMap.erase(it);
        }
    }
    // For ECDH, public key should have no usages (already empty)

    return 0;
}

result_t CryptoKey::generate_ecdsa()
{
    Variant v;

    if (m_usageMap.find("sign") == m_usageMap.end())
        return Runtime::setError("WebCrypto: ECDSA key must have 'sign' usage");

    obj_ptr<generateKeyPairParam> param = new generateKeyPairParam();
    m_algorithm->get("namedCurve", v);
    param->namedCurve = v.string();

    obj_ptr<crypto_base::GenerateKeyPairType> keyPair = new crypto_base::GenerateKeyPairType();

    m_key = new KeyObject();
    result_t hr = m_key->generateKey("EC", param);
    if (hr < 0)
        return hr;

    return createPublicKey();
}

result_t CryptoKey::generate_ed25519()
{
    if (m_usageMap.find("sign") == m_usageMap.end())
        return Runtime::setError("WebCrypto: Ed25519 key must have 'sign' usage");

    m_key = new KeyObject();
    result_t hr = m_key->generateKey("Ed25519", nullptr);
    if (hr < 0)
        return hr;

    return createPublicKey();
}

result_t CryptoKey::generate_ecdh()
{
    bool hasValidUsage = false;
    if (m_usageMap.find("deriveKey") != m_usageMap.end())
        hasValidUsage = true;
    if (m_usageMap.find("deriveBits") != m_usageMap.end())
        hasValidUsage = true;

    if (!hasValidUsage)
        return Runtime::setError("WebCrypto: ECDH key must have 'deriveKey' or 'deriveBits' usage");

    Variant v;
    obj_ptr<generateKeyPairParam> param = new generateKeyPairParam();
    m_algorithm->get("namedCurve", v);
    param->namedCurve = v.string();

    m_key = new KeyObject();
    result_t hr = m_key->generateKey("EC", param);
    if (hr < 0)
        return hr;

    return createPublicKey();
}

result_t subtle_base::generateKey(Union_generateKey_algorithm algorithm, bool extractable, v8::Local<v8::Array> usages,
    Variant& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        Isolate* isolate = ac.isolate();
        result_t hr;

        ac.ctxv().resize(1);

        v8::Local<v8::Object> algObj;

        if (std::holds_alternative<v8::Local<v8::Object>>(algorithm))
            algObj = std::get<v8::Local<v8::Object>>(algorithm);
        else {
            v8::Local<v8::Context> context = isolate->context();
            algObj = v8::Object::New(isolate->m_isolate);
            algObj->Set(context, isolate->NewString("name"), isolate->NewString(std::get<exlib::string>(algorithm))).IsJust();
        }

        obj_ptr<CryptoKey> key = new CryptoKey();
        hr = key->get_param(algObj, extractable, usages);
        if (hr < 0)
            return hr;

        ac.ctxv()[0] = key;

        return CALL_E_NOSYNC;
    }

    // the algorithm object was read in the sync phase
    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;

    obj_ptr<CryptoKey> key = (CryptoKey*)ac.ctxv()[0].object();
    if (key == NULL)
        return Runtime::setError("WebCrypto: the key parameters were not read");

    result_t hr = key->generate();
    if (hr < 0)
        return hr;

    if (key->m_publicKey) {
        obj_ptr<NObject> keyPair = new NObject();

        keyPair->add("publicKey", key->m_publicKey);
        keyPair->add("privateKey", key);

        retVal = keyPair;
    } else
        retVal = key;

    return 0;
}


}
