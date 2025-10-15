/*
 * sign.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: lion
 */

#include "object.h"
#include "ifs/subtle.h"
#include "CryptoKey.h"

namespace fibjs {

static result_t get_options(v8::Local<v8::Object> algorithm, CryptoKey* key, AsyncEvent* ac)
{
    result_t hr;
    Isolate* isolate = ac->isolate();
    v8::Local<v8::Context> context = isolate->context();

    ac->m_ctx.resize(2);

    exlib::string name;
    hr = GetConfigValue(isolate, algorithm, "name", name, true);
    if (hr < 0)
        return hr;

    ac->m_ctx[0] = name;

    hr = key->check_name(name);
    if (hr < 0)
        return hr;

    if (qstricmp(name.c_str(), "ecdsa") == 0) {
        exlib::string hash;

        v8::Local<v8::Value> _hash = algorithm->Get(context, isolate->NewString("hash")).FromMaybe(v8::Local<v8::Value>());
        if (!_hash.IsEmpty()) {
            if (_hash->IsString() || _hash->IsStringObject())
                hash = isolate->toString(_hash);
            else {
                v8::Local<v8::Object> _hash_obj;
                hr = GetArgumentValue(isolate, _hash, _hash_obj, true);
                if (hr < 0)
                    return hr;

                hr = GetConfigValue(isolate, _hash_obj, "name", hash, true);
                if (hr < 0)
                    return hr;
            }
        }

        ac->m_ctx[1] = hash;
    } else if (qstricmp(name.c_str(), "ed25519") == 0) {
        // Ed25519 doesn't use hash parameter - it has built-in SHA-512
        ac->m_ctx[1] = exlib::string("");
    } else if (qstricmp(name.c_str(), "hmac") == 0) {
        // HMAC uses the hash from the key algorithm
        exlib::string hash;
        v8::Local<v8::Object> key_algorithm;
        result_t hr = key->get_algorithm(key_algorithm);
        if (hr < 0)
            return hr;

        v8::Local<v8::Value> _hash = key_algorithm->Get(context, isolate->NewString("hash")).FromMaybe(v8::Local<v8::Value>());
        if (!_hash.IsEmpty() && _hash->IsObject()) {
            v8::Local<v8::Object> _hash_obj = v8::Local<v8::Object>::Cast(_hash);
            hr = GetConfigValue(isolate, _hash_obj, "name", hash, true);
            if (hr < 0)
                return hr;
        }

        ac->m_ctx[1] = hash;
    }

    return 0;
}

result_t subtle_base::sign(v8::Local<v8::Object> algorithm, CryptoKey_base* key, Buffer_base* data, std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = get_options(algorithm, (CryptoKey*)key, ac);
        if (hr < 0)
            return hr;

        return CALL_E_NOSYNC;
    }

    CryptoKey* _key = (CryptoKey*)key;
    exlib::string hash = ac->m_ctx[1].string();
    exlib::string name = ac->m_ctx[0].string();

    // Check if the key has 'sign' usage
    if (_key->m_usageMap.find("sign") == _key->m_usageMap.end())
        return Runtime::setError("WebCrypto: key does not have 'sign' usage");

    // HMAC uses the hash from the key algorithm
    if (qstricmp(name.c_str(), "hmac") == 0) {
        // HMAC requires special handling with secret key
        const EVP_MD* md = _evp_md_type(hash.c_str());
        if (!md)
            return Runtime::setError("Invalid hash algorithm: " + hash);

        KeyObject* key_obj = (KeyObject*)_key->m_key.get();
        EVPMDPointer context = EVP_MD_CTX_new();
        EVP_DigestInit_ex(context, md, NULL);

        EVPKeyPointer pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, key_obj->data(), key_obj->length());
        if (EVP_DigestSignInit(context, NULL, md, NULL, pkey) <= 0)
            return openssl_error();

        Buffer* buf = Buffer::Cast(data);
        if (EVP_DigestSignUpdate(context, buf->data(), buf->length()) <= 0)
            return openssl_error();

        size_t sig_len = EVP_MD_size(md);
        std::shared_ptr<v8::BackingStore> sig_store = NewBackingStore(sig_len);
        if (EVP_DigestSignFinal(context, (unsigned char*)sig_store->Data(), &sig_len) <= 0)
            return openssl_error();

        retVal = sig_store;
        return 0;
    }

    // Ed25519 uses DER encoding and empty hash (built-in SHA-512)
    if (qstricmp(name.c_str(), "ed25519") == 0) {
        obj_ptr<Buffer_base> sig_buffer;
        result_t hr = _sign("", data, _key->m_key, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, sig_buffer);
        if (hr < 0)
            return hr;

        Buffer* buf = Buffer::Cast(sig_buffer);
        std::shared_ptr<v8::BackingStore> sig_store = NewBackingStore(buf->length());
        if (buf->length() > 0 && sig_store->Data() && buf->data()) {
            memcpy(sig_store->Data(), buf->data(), buf->length());
        }
        retVal = sig_store;
        return 0;
    }

    obj_ptr<Buffer_base> sig_buffer;
    result_t hr = _sign(hash, data, _key->m_key, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, sig_buffer);
    if (hr < 0)
        return hr;

    Buffer* buf = Buffer::Cast(sig_buffer);
    std::shared_ptr<v8::BackingStore> sig_store = NewBackingStore(buf->length());
    if (buf->length() > 0 && sig_store->Data() && buf->data()) {
        memcpy(sig_store->Data(), buf->data(), buf->length());
    }
    retVal = sig_store;
    return 0;
}

result_t subtle_base::verify(v8::Local<v8::Object> algorithm, CryptoKey_base* key, Buffer_base* signature, Buffer_base* data, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = get_options(algorithm, (CryptoKey*)key, ac);
        if (hr < 0)
            return hr;

        return CALL_E_NOSYNC;
    }

    CryptoKey* _key = (CryptoKey*)key;
    exlib::string hash = ac->m_ctx[1].string();
    exlib::string name = ac->m_ctx[0].string();

    // Check if the key has 'verify' usage
    if (_key->m_usageMap.find("verify") == _key->m_usageMap.end())
        return Runtime::setError("WebCrypto: key does not have 'verify' usage");

    // HMAC uses the hash from the key algorithm
    if (qstricmp(name.c_str(), "hmac") == 0) {
        // HMAC verification: sign the data and compare with provided signature
        const EVP_MD* md = _evp_md_type(hash.c_str());
        if (!md)
            return Runtime::setError("Invalid hash algorithm: " + hash);

        KeyObject* key_obj = (KeyObject*)_key->m_key.get();
        EVPMDPointer context = EVP_MD_CTX_new();
        EVP_DigestInit_ex(context, md, NULL);

        EVPKeyPointer pkey = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, key_obj->data(), key_obj->length());
        if (EVP_DigestSignInit(context, NULL, md, NULL, pkey) <= 0)
            return openssl_error();

        Buffer* buf = Buffer::Cast(data);
        if (EVP_DigestSignUpdate(context, buf->data(), buf->length()) <= 0)
            return openssl_error();

        size_t computed_sig_len = EVP_MD_size(md);
        obj_ptr<Buffer> computed_sig = new Buffer(NULL, computed_sig_len);
        if (EVP_DigestSignFinal(context, computed_sig->data(), &computed_sig_len) <= 0)
            return openssl_error();

        // Compare computed signature with provided signature
        Buffer* sig = Buffer::Cast(signature);
        retVal = (computed_sig_len == sig->length()) && (memcmp(computed_sig->data(), sig->data(), computed_sig_len) == 0);
        return 0;
    }

    // Ed25519 uses DER encoding and empty hash (built-in SHA-512)
    if (qstricmp(name.c_str(), "ed25519") == 0) {
        return _verify("", data, _key->m_key, signature, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
    }

    return _verify(hash, data, _key->m_key, signature, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t subtle_base::sign(exlib::string algorithm, CryptoKey_base* key, Buffer_base* data, std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);
        ac->m_ctx[0] = algorithm;

        // Add algorithm validation for string version
        CryptoKey* _key = (CryptoKey*)key;
        result_t hr = _key->check_name(algorithm);
        if (hr < 0)
            return hr;

        // For HMAC, extract hash from key algorithm
        if (qstricmp(algorithm.c_str(), "hmac") == 0) {
            Isolate* isolate = ac->isolate();
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Object> key_algorithm;
            hr = _key->get_algorithm(key_algorithm);
            if (hr < 0)
                return hr;

            v8::Local<v8::Value> _hash = key_algorithm->Get(context, isolate->NewString("hash")).FromMaybe(v8::Local<v8::Value>());
            if (!_hash.IsEmpty() && _hash->IsObject()) {
                v8::Local<v8::Object> _hash_obj = v8::Local<v8::Object>::Cast(_hash);
                exlib::string hash;
                hr = GetConfigValue(isolate, _hash_obj, "name", hash, true);
                if (hr < 0)
                    return hr;
                ac->m_ctx[1] = hash;
            } else {
                ac->m_ctx[1] = exlib::string("");
            }
        } else {
            ac->m_ctx[1] = exlib::string("");
        }

        return CALL_E_NOSYNC;
    }

    return sign(v8::Local<v8::Object>(), key, data, retVal, ac);
}

result_t subtle_base::verify(exlib::string algorithm, CryptoKey_base* key, Buffer_base* signature, Buffer_base* data, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);
        ac->m_ctx[0] = algorithm;

        // Add algorithm validation for string version
        CryptoKey* _key = (CryptoKey*)key;
        result_t hr = _key->check_name(algorithm);
        if (hr < 0)
            return hr;

        // For HMAC, extract hash from key algorithm
        if (qstricmp(algorithm.c_str(), "hmac") == 0) {
            Isolate* isolate = ac->isolate();
            v8::Local<v8::Context> context = isolate->context();
            v8::Local<v8::Object> key_algorithm;
            hr = _key->get_algorithm(key_algorithm);
            if (hr < 0)
                return hr;

            v8::Local<v8::Value> _hash = key_algorithm->Get(context, isolate->NewString("hash")).FromMaybe(v8::Local<v8::Value>());
            if (!_hash.IsEmpty() && _hash->IsObject()) {
                v8::Local<v8::Object> _hash_obj = v8::Local<v8::Object>::Cast(_hash);
                exlib::string hash;
                hr = GetConfigValue(isolate, _hash_obj, "name", hash, true);
                if (hr < 0)
                    return hr;
                ac->m_ctx[1] = hash;
            } else {
                ac->m_ctx[1] = exlib::string("");
            }
        } else {
            ac->m_ctx[1] = exlib::string("");
        }

        return CALL_E_NOSYNC;
    }

    return verify(v8::Local<v8::Object>(), key, signature, data, retVal, ac);
}

}
