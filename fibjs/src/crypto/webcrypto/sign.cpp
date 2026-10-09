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

static result_t get_options(v8::Local<v8::Object> algorithm, CryptoKey* key, AsyncHandle& ac)
{
    result_t hr;
    Isolate* isolate = ac.isolate();
    v8::Local<v8::Context> context = isolate->context();

    ac.ctxv().resize(2);

    exlib::string name;
    hr = GetConfigValue(algorithm, "name", name, true);
    if (hr < 0)
        return hr;

    ac.ctxv()[0] = name;

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

                hr = GetConfigValue(_hash_obj, "name", hash, true);
                if (hr < 0)
                    return hr;
            }
        }

        ac.ctxv()[1] = hash;
    } else if (qstricmp(name.c_str(), "ed25519") == 0) {
        // Ed25519 doesn't use hash parameter - it has built-in SHA-512
        ac.ctxv()[1] = exlib::string("");
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
            hr = GetConfigValue(_hash_obj, "name", hash, true);
            if (hr < 0)
                return hr;
        }

        ac.ctxv()[1] = hash;
    }

    return 0;
}

// The string form of the algorithm is pure C++: check_name and the HMAC hash
// come from the key's C++ algorithm object (m_algorithm), so the whole path
// runs in the async phase and a cc_ caller can use it
// (plans/async-phase-discipline-audit-2026-10-05.md §3-F10/§4.5).
static result_t name_hash_from_key(CryptoKey* key, const exlib::string& name, exlib::string& hash)
{
    result_t hr = key->check_name(name);
    if (hr < 0)
        return hr;

    hash.clear();

    if (qstricmp(name.c_str(), "hmac") == 0) {
        Variant v;
        obj_ptr<NObject> hashObj;

        if (key->m_algorithm->get("hash", v) >= 0 && (hashObj = (NObject*)v.object()) != NULL) {
            Variant hashName;

            if (hashObj->get("name", hashName) >= 0 && hashName.type() == Variant::VT_String)
                hash = hashName.string();
        }
    }

    return 0;
}


result_t subtle_base::sign(Union_sign_algorithm algorithm, CryptoKey_base* key, Union_sign_data data, std::shared_ptr<v8::BackingStore>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        // only the object alternative reads JS values; the string form is
        // C++-only (check_name + the key's C++ algorithm object) and is
        // resolved in the async phase, so a cc_ caller can use it
        // (plans/async-phase-discipline-audit-2026-10-05.md §3-F10)
        if (std::holds_alternative<v8::Local<v8::Object>>(algorithm)) {
            result_t hr = get_options(std::get<v8::Local<v8::Object>>(algorithm), (CryptoKey*)key, ac);
            if (hr < 0)
                return hr;
        }

        return CALL_E_NOSYNC;
    }

    obj_ptr<Buffer_base> data_buffer;

    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        data_buffer = std::get<obj_ptr<Buffer_base>>(data);
    else {
        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", data_buffer);
        if (hr < 0)
            return hr;
    }

    CryptoKey* _key = (CryptoKey*)key;
    exlib::string name, hash;

    if (std::holds_alternative<exlib::string>(algorithm)) {
        name = std::get<exlib::string>(algorithm);

        result_t hr = name_hash_from_key(_key, name, hash);
        if (hr < 0)
            return hr;
    } else {
        result_t ctx_hr = ac.ctx(1);
        if (ctx_hr < 0)
            return ctx_hr;

        name = ac.ctxv()[0].string();
        hash = ac.ctxv()[1].string();
    }

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

        Buffer* buf = Buffer::Cast(data_buffer);
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
        result_t hr = _sign("", data_buffer, _key->m_key, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, sig_buffer);
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
    result_t hr = _sign(hash, data_buffer, _key->m_key, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, sig_buffer);
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


result_t subtle_base::verify(Union_verify_algorithm algorithm, CryptoKey_base* key, Union_verify_signature signature, Union_verify_data data, bool& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        // only the object alternative reads JS values; the string form is
        // C++-only and is resolved in the async phase (see sign above)
        if (std::holds_alternative<v8::Local<v8::Object>>(algorithm)) {
            result_t hr = get_options(std::get<v8::Local<v8::Object>>(algorithm), (CryptoKey*)key, ac);
            if (hr < 0)
                return hr;
        }

        return CALL_E_NOSYNC;
    }

    obj_ptr<Buffer_base> signature_buffer, data_buffer;

    if (std::holds_alternative<obj_ptr<Buffer_base>>(signature))
        signature_buffer = std::get<obj_ptr<Buffer_base>>(signature);
    else {
        result_t hr = Buffer_base::from(std::get<exlib::string>(signature), "utf8", signature_buffer);
        if (hr < 0)
            return hr;
    }

    if (std::holds_alternative<obj_ptr<Buffer_base>>(data))
        data_buffer = std::get<obj_ptr<Buffer_base>>(data);
    else {
        result_t hr = Buffer_base::from(std::get<exlib::string>(data), "utf8", data_buffer);
        if (hr < 0)
            return hr;
    }

    CryptoKey* _key = (CryptoKey*)key;
    exlib::string name, hash;

    if (std::holds_alternative<exlib::string>(algorithm)) {
        name = std::get<exlib::string>(algorithm);

        result_t hr = name_hash_from_key(_key, name, hash);
        if (hr < 0)
            return hr;
    } else {
        result_t ctx_hr = ac.ctx(1);
        if (ctx_hr < 0)
            return ctx_hr;

        name = ac.ctxv()[0].string();
        hash = ac.ctxv()[1].string();
    }

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

        Buffer* buf = Buffer::Cast(data_buffer);
        if (EVP_DigestSignUpdate(context, buf->data(), buf->length()) <= 0)
            return openssl_error();

        size_t computed_sig_len = EVP_MD_size(md);
        obj_ptr<Buffer> computed_sig = new Buffer(NULL, computed_sig_len);
        if (EVP_DigestSignFinal(context, computed_sig->data(), &computed_sig_len) <= 0)
            return openssl_error();

        // Compare computed signature with provided signature
        Buffer* sig = Buffer::Cast(signature_buffer);
        retVal = (computed_sig_len == sig->length()) && (memcmp(computed_sig->data(), sig->data(), computed_sig_len) == 0);
        return 0;
    }

    // Ed25519 uses DER encoding and empty hash (built-in SHA-512)
    if (qstricmp(name.c_str(), "ed25519") == 0) {
        return _verify("", data_buffer, _key->m_key, signature_buffer, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
    }

    return _verify(hash, data_buffer, _key->m_key, signature_buffer, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, retVal);
}



}
