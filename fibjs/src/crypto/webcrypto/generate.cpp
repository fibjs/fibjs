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

namespace fibjs {

result_t CryptoKey::generate()
{
    if (m_key_type == kKeyNameECDSA)
        return generate_ecdsa();
    return 0;
}

result_t CryptoKey::createPublicKey()
{
    obj_ptr<KeyObject_base> publicKey;
    result_t hr = crypto_base::createPublicKey(m_key, publicKey);
    if (hr < 0)
        return hr;

    m_publicKey = new CryptoKey();

    m_publicKey->m_key_type = m_key_type;
    m_publicKey->m_key = publicKey.As<KeyObject>();
    m_publicKey->m_algorithm = m_algorithm;
    m_publicKey->m_extractable = true;

    auto it = m_usageMap.find("verify");
    if (it != m_usageMap.end()) {
        m_publicKey->m_usageMap.emplace("verify", true);
        m_usageMap.erase(it);
    }

    return 0;
}

result_t CryptoKey::generate_ecdsa()
{
    Isolate* isolate = holder();
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

result_t subtle_base::generateKey(v8::Local<v8::Object> algorithm, bool extractable, v8::Local<v8::Array> usages,
    Variant& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = ac->isolate();
        result_t hr;

        ac->m_ctx.resize(1);

        obj_ptr<CryptoKey> key = new CryptoKey();
        hr = key->get_param(algorithm, extractable, usages);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = key;

        return CALL_E_NOSYNC;
    }

    obj_ptr<CryptoKey> key = (CryptoKey*)ac->m_ctx[0].object();

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
