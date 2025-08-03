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
    }

    return 0;
}

result_t subtle_base::sign(v8::Local<v8::Object> algorithm, CryptoKey_base* key, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
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

    // Ed25519 uses DER encoding and empty hash (built-in SHA-512)
    if (qstricmp(name.c_str(), "ed25519") == 0) {
        return _sign("", data, _key->m_key, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
    }

    return _sign(hash, data, _key->m_key, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, retVal);
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

    // Ed25519 uses DER encoding and empty hash (built-in SHA-512)
    if (qstricmp(name.c_str(), "ed25519") == 0) {
        return _verify("", data, _key->m_key, signature, kSigEncDER, DEFAULT_PADDING, NO_SALTLEN, retVal);
    }

    return _verify(hash, data, _key->m_key, signature, kSigEncP1363, DEFAULT_PADDING, NO_SALTLEN, retVal);
}

result_t subtle_base::sign(exlib::string algorithm, CryptoKey_base* key, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        ac->m_ctx[0] = algorithm;
        ac->m_ctx[1] = exlib::string("");

        return CALL_E_NOSYNC;
    }

    return sign(v8::Local<v8::Object>(), key, data, retVal, ac);
}

result_t subtle_base::verify(exlib::string algorithm, CryptoKey_base* key, Buffer_base* signature, Buffer_base* data, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        ac->m_ctx[0] = algorithm;
        ac->m_ctx[1] = exlib::string("");

        return CALL_E_NOSYNC;
    }

    return verify(v8::Local<v8::Object>(), key, signature, data, retVal, ac);
}

}
