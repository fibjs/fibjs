/*
 * deriveBits.cpp
 *
 *  Created on: Oct 11, 2025
 *      Author: lion
 */

#include "object.h"
#include "ifs/subtle.h"
#include "ifs/crypto.h"
#include "CryptoKey.h"
#include "crypto_util.h"
#include "Buffer.h"
#include "v8_api.h"
#include <cstring>

namespace fibjs {

static result_t get_ecdh_options(v8::Local<v8::Object> algorithm, CryptoKey* baseKey, AsyncEvent* ac)
{
    result_t hr;
    Isolate* isolate = ac->isolate();
    v8::Local<v8::Context> context = isolate->context();

    ac->m_ctx.resize(2);

    exlib::string name;
    hr = GetConfigValue(algorithm, "name", name, true);
    if (hr < 0)
        return hr;

    ac->m_ctx[0] = name;

    hr = baseKey->check_name(name);
    if (hr < 0)
        return hr;

    if (qstricmp(name.c_str(), "ecdh") == 0) {
        // Get the public key for ECDH
        v8::Local<v8::Value> _public = algorithm->Get(context, isolate->NewString("public")).FromMaybe(v8::Local<v8::Value>());
        if (_public.IsEmpty() || !_public->IsObject())
            return Runtime::setError("WebCrypto: ECDH algorithm must have 'public' property");

        obj_ptr<CryptoKey_base> publicKey;
        hr = GetArgumentValue(isolate, _public, publicKey);
        if (hr < 0)
            return hr;

        ac->m_ctx[1] = publicKey;
    }

    return 0;
}

result_t subtle_base::deriveBits(v8::Local<v8::Object> algorithm, CryptoKey_base* baseKey, int32_t length,
    std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        result_t hr = get_ecdh_options(algorithm, (CryptoKey*)baseKey, ac);
        if (hr < 0)
            return hr;

        return CALL_E_NOSYNC;
    }

    CryptoKey* _baseKey = (CryptoKey*)baseKey;
    exlib::string name = ac->m_ctx[0].string();

    // Check if the baseKey has 'deriveBits' usage
    if (_baseKey->m_usageMap.find("deriveBits") == _baseKey->m_usageMap.end())
        return Runtime::setError("WebCrypto: baseKey does not have deriveBits usage");

    // For ECDH, baseKey must be a private key
    if (qstricmp(name.c_str(), "ecdh") == 0 && _baseKey->type() != KeyObject::kKeyTypePrivate)
        return Runtime::setError("WebCrypto: ECDH baseKey must be a private key");

    if (qstricmp(name.c_str(), "ecdh") == 0) {
        CryptoKey* publicKey = (CryptoKey*)ac->m_ctx[1].object();

        // Validate that the public key is indeed public
        if (publicKey->type() != KeyObject::kKeyTypePublic)
            return Runtime::setError("WebCrypto: algorithm.public must be a public key");

        // Validate that both keys are ECDH keys
        if (publicKey->m_key_type != kKeyNameECDH)
            return Runtime::setError("WebCrypto: algorithm.public must be an ECDH key");

        // Validate that both keys use the same curve
        exlib::string baseCurve = _baseKey->m_algorithm->get("namedCurve").string();
        exlib::string publicCurve = publicKey->m_algorithm->get("namedCurve").string();
        if (qstricmp(baseCurve.c_str(), publicCurve.c_str()) != 0)
            return Runtime::setError("WebCrypto: key curves must match");

        // Perform ECDH key agreement using OpenSSL EVP_PKEY API
        obj_ptr<Buffer_base> derivedBits;
        result_t hr;

        // Create EVP_PKEY_CTX for the private key
        EVPKeyCtxPointer ctx = EVP_PKEY_CTX_new(_baseKey->m_key->pkey(), nullptr);
        if (!ctx)
            return openssl_error();

        // Initialize the derivation operation
        if (EVP_PKEY_derive_init(ctx) <= 0)
            return openssl_error();

        // Set the peer public key
        if (EVP_PKEY_derive_set_peer(ctx, publicKey->m_key->pkey()) <= 0)
            return openssl_error();

        // Get the length of the derived key
        size_t len;
        if (EVP_PKEY_derive(ctx, nullptr, &len) <= 0)
            return openssl_error();

        // Allocate buffer and derive the key
        obj_ptr<Buffer> buf = new Buffer(nullptr, len);
        if (EVP_PKEY_derive(ctx, buf->data(), &len) <= 0)
            return openssl_error();

        buf->resize(len);

        // Convert Buffer to BackingStore for ArrayBuffer
        std::shared_ptr<v8::BackingStore> result = NewBackingStore(len);
        memcpy(result->Data(), buf->data(), len);
        derivedBits = buf;
        retVal = result;

        // Handle length parameter
        // If length is -1 (default value for null/undefined), return full derived bits
        if (length > 0) {
            // Calculate how many bytes we need
            int32_t bytesNeeded = (length + 7) / 8;
            int32_t derivedLength;
            hr = derivedBits->get_length(derivedLength);
            if (hr < 0)
                return hr;

            if (bytesNeeded > derivedLength)
                return Runtime::setError("WebCrypto: requested length exceeds available bits");

            // If we need to truncate
            if (bytesNeeded < derivedLength) {
                obj_ptr<Buffer_base> truncated;
                hr = Buffer_base::_new(bytesNeeded, truncated);
                if (hr < 0)
                    return hr;

                // Get raw data from source buffer
                Buffer* srcBuffer = Buffer::Cast(derivedBits);
                Buffer* dstBuffer = Buffer::Cast(truncated);

                // Copy the needed bytes
                memcpy(dstBuffer->data(), srcBuffer->data(), bytesNeeded);

                // For non-byte-aligned lengths, mask the last byte
                if (length % 8 != 0 && bytesNeeded > 0) {
                    int32_t lastByteBits = length % 8;
                    uint8_t mask = (uint8_t)(0xFF << (8 - lastByteBits));
                    dstBuffer->data()[bytesNeeded - 1] &= mask;
                }

                // Convert truncated Buffer to BackingStore
                std::shared_ptr<v8::BackingStore> result = NewBackingStore(bytesNeeded);
                memcpy(result->Data(), ((Buffer*)truncated.get())->data(), bytesNeeded);
                retVal = result;
                return 0;
            }
        }

        int32_t bufLen;
        hr = derivedBits->get_length(bufLen);
        if (hr < 0)
            return hr;
        std::shared_ptr<v8::BackingStore> finalResult = NewBackingStore(bufLen);
        memcpy(finalResult->Data(), ((Buffer*)derivedBits.get())->data(), bufLen);
        retVal = finalResult;
        return 0;
    }

    return Runtime::setError("WebCrypto: unsupported algorithm for deriveBits: " + name);
}

result_t subtle_base::deriveBits(exlib::string algorithm, CryptoKey_base* baseKey, int32_t length,
    std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        ac->m_ctx[0] = algorithm;
        // No public key in string format, this would be an error for ECDH

        return CALL_E_NOSYNC;
    }

    return deriveBits(v8::Local<v8::Object>(), baseKey, length, retVal, ac);
}

}