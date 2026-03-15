/*
 * param.cpp
 *
 *  Created on: Aug 25, 2014
 *      Author: lion
 */

#include "object.h"
#include "CryptoKey.h"

namespace fibjs {

result_t CryptoKey::get_param(v8::Local<v8::Object> params, bool extractable, v8::Local<v8::Array> usages)
{
    result_t hr;
    exlib::string name;

    m_extractable = extractable;

    int32_t len = usages->Length();

    for (int32_t i = 0; i < len; i++) {
        exlib::string usage;
        hr = GetConfigValue(usages, i, usage, true);
        if (hr < 0)
            return hr;

        if (qstrcmp(usage.c_str(), "sign")
            && qstrcmp(usage.c_str(), "verify")
            && qstrcmp(usage.c_str(), "encrypt")
            && qstrcmp(usage.c_str(), "decrypt")
            && qstrcmp(usage.c_str(), "wrapKey")
            && qstrcmp(usage.c_str(), "unwrapKey")
            && qstrcmp(usage.c_str(), "deriveKey")
            && qstrcmp(usage.c_str(), "deriveBits"))
            return Runtime::setError("WebCrypto: unknown key usage: " + usage);

        m_usageMap.emplace(usage, true);
    }

    hr = GetConfigValue(params, "name", name, true);
    if (hr < 0)
        return hr;

    if (qstricmp(name.c_str(), "ECDSA") == 0)
        return get_ecdsa_param(params);

    if (qstricmp(name.c_str(), "Ed25519") == 0)
        return get_ed25519_param(params);

    if (qstricmp(name.c_str(), "ECDH") == 0)
        return get_ecdh_param(params);

    if (qstricmp(name.c_str(), "HMAC") == 0)
        return get_hmac_param(params);

    return Runtime::setError("WebCrypto: unknown key name: " + name);
}

result_t CryptoKey::check_asymmetric_usage()
{
    for (auto& it : m_usageMap) {
        if (qstrcmp(it.first.c_str(), "sign")
            && qstrcmp(it.first.c_str(), "verify"))
            return Runtime::setError("WebCrypto: usage " + it.first + " is not allowed for ECDSA key");
    }

    return 0;
}

result_t CryptoKey::get_ecdsa_param(v8::Local<v8::Object> params)
{
    result_t hr;
    exlib::string namedCurve;

    m_key_type = kKeyNameECDSA;
    m_algorithm->add("name", "ECDSA");

    hr = GetConfigValue(params, "namedCurve", namedCurve, true);
    if (hr < 0)
        return hr;

    if (qstricmp(namedCurve.c_str(), "P-256") == 0)
        m_algorithm->add("namedCurve", "P-256");
    else if (qstricmp(namedCurve.c_str(), "P-384") == 0)
        m_algorithm->add("namedCurve", "P-384");
    else if (qstricmp(namedCurve.c_str(), "P-521") == 0)
        m_algorithm->add("namedCurve", "P-521");
    else
        return Runtime::setError("WebCrypto: unknown curve name: " + namedCurve);

    return check_asymmetric_usage();
}

result_t CryptoKey::get_ed25519_param(v8::Local<v8::Object> params)
{
    m_key_type = kKeyNameEd25519;
    m_algorithm->add("name", "Ed25519");

    return check_asymmetric_usage();
}

result_t CryptoKey::get_ecdh_param(v8::Local<v8::Object> params)
{
    result_t hr;
    exlib::string namedCurve;

    m_key_type = kKeyNameECDH;
    m_algorithm->add("name", "ECDH");

    hr = GetConfigValue(params, "namedCurve", namedCurve, true);
    if (hr < 0)
        return hr;

    if (qstricmp(namedCurve.c_str(), "P-256") == 0)
        m_algorithm->add("namedCurve", "P-256");
    else if (qstricmp(namedCurve.c_str(), "P-384") == 0)
        m_algorithm->add("namedCurve", "P-384");
    else if (qstricmp(namedCurve.c_str(), "P-521") == 0)
        m_algorithm->add("namedCurve", "P-521");
    else
        return Runtime::setError("WebCrypto: unknown curve name: " + namedCurve);

    // Check ECDH specific usages
    for (auto& it : m_usageMap) {
        if (qstrcmp(it.first.c_str(), "deriveKey")
            && qstrcmp(it.first.c_str(), "deriveBits"))
            return Runtime::setError("WebCrypto: usage " + it.first + " is not allowed for ECDH key");
    }

    return 0;
}

result_t CryptoKey::get_hmac_param(v8::Local<v8::Object> params)
{
    Isolate* isolate = holder();
    result_t hr;
    exlib::string hash;

    m_key_type = kKeyNameHMAC;
    m_algorithm->add("name", "HMAC");

    // Get hash parameter - similar pattern to ECDSA
    v8::Local<v8::Value> _hash;
    hr = GetConfigValue(params, "hash", _hash);
    if (hr < 0 || _hash.IsEmpty() || _hash->IsUndefined())
        return Runtime::setError("WebCrypto: HMAC requires hash parameter");

    if (_hash->IsString() || _hash->IsStringObject()) {
        hash = *v8::String::Utf8Value(isolate->m_isolate, _hash);
    } else {
        v8::Local<v8::Object> _hash_obj;
        hr = GetArgumentValue(isolate, _hash, _hash_obj, true);
        if (hr < 0)
            return hr;

        hr = GetConfigValue(_hash_obj, "name", hash, true);
        if (hr < 0)
            return hr;
    }

    // Validate hash algorithm
    if (qstricmp(hash.c_str(), "SHA-1") && qstricmp(hash.c_str(), "SHA-256")
        && qstricmp(hash.c_str(), "SHA-384") && qstricmp(hash.c_str(), "SHA-512"))
        return Runtime::setError("WebCrypto: unknown hash algorithm: " + hash);

    // Create hash object with name property
    obj_ptr<NObject> hashObj = new NObject();
    hashObj->add("name", hash);
    m_algorithm->add("hash", hashObj);

    // Check HMAC specific usages
    for (auto& it : m_usageMap) {
        if (qstrcmp(it.first.c_str(), "sign")
            && qstrcmp(it.first.c_str(), "verify"))
            return Runtime::setError("WebCrypto: usage " + it.first + " is not allowed for HMAC key");
    }

    return 0;
}

result_t CryptoKey::check_asymmetric_import_usage()
{
    if (m_key->type() == KeyObject::kKeyTypePrivate) {
        if (m_usageMap.find("sign") == m_usageMap.end())
            return Runtime::setError("WebCrypto: private key must have 'sign' usage");
        if (m_usageMap.find("verify") != m_usageMap.end())
            return Runtime::setError("WebCrypto: private key must not have 'verify' usage");
    } else {
        if (m_usageMap.find("verify") == m_usageMap.end())
            return Runtime::setError("WebCrypto: public key must have 'verify' usage");
        if (m_usageMap.find("sign") != m_usageMap.end())
            return Runtime::setError("WebCrypto: public key must not have 'sign' usage");
    }

    return 0;
}

result_t CryptoKey::check_ecdsa_import_param()
{
    const char* namedCurve = m_key->namedCurve();
    if (!namedCurve || qstricmp(namedCurve, m_algorithm->get("namedCurve").string().c_str()) != 0)
        return Runtime::setError("WebCrypto: invalid key algorithm, key curve '%s' does not match '%s'.",
            namedCurve ? namedCurve : "(null)", m_algorithm->get("namedCurve").string().c_str());

    return check_asymmetric_import_usage();
}

result_t CryptoKey::check_ed25519_import_param()
{
    // Ed25519 keys don't have namedCurve parameter to check like ECDSA
    // For Ed25519, we just need to verify the key type matches
    return check_asymmetric_import_usage();
}

result_t CryptoKey::check_ecdh_import_param()
{
    const char* namedCurve = m_key->namedCurve();
    if (!namedCurve || qstricmp(namedCurve, m_algorithm->get("namedCurve").string().c_str()) != 0)
        return Runtime::setError("WebCrypto: invalid key algorithm, key curve '%s' does not match '%s'.",
            namedCurve ? namedCurve : "(null)", m_algorithm->get("namedCurve").string().c_str());

    // Check ECDH specific import usages
    if (m_key->type() == KeyObject::kKeyTypePrivate) {
        bool hasValidUsage = false;
        if (m_usageMap.find("deriveKey") != m_usageMap.end())
            hasValidUsage = true;
        if (m_usageMap.find("deriveBits") != m_usageMap.end())
            hasValidUsage = true;

        if (!hasValidUsage)
            return Runtime::setError("WebCrypto: ECDH private key must have 'deriveKey' or 'deriveBits' usage");

        // Check for invalid usages
        for (auto& it : m_usageMap) {
            if (qstrcmp(it.first.c_str(), "deriveKey")
                && qstrcmp(it.first.c_str(), "deriveBits"))
                return Runtime::setError("WebCrypto: ECDH private key must not have '" + it.first + "' usage");
        }
    } else {
        // Public keys for ECDH should have no usages
        if (!m_usageMap.empty())
            return Runtime::setError("WebCrypto: ECDH public key must not have any usages");
    }

    return 0;
}

result_t CryptoKey::check_import_param()
{
    if (m_key_type == kKeyNameECDSA)
        return check_ecdsa_import_param();

    if (m_key_type == kKeyNameEd25519)
        return check_ed25519_import_param();

    if (m_key_type == kKeyNameECDH)
        return check_ecdh_import_param();

    if (m_key_type == kKeyNameHMAC)
        return check_hmac_import_param();

    return check_asymmetric_import_usage();
}

result_t CryptoKey::check_hmac_import_param()
{
    // HMAC keys are secret keys, not public/private
    // Check for valid usages
    bool hasSign = m_usageMap.find("sign") != m_usageMap.end();
    bool hasVerify = m_usageMap.find("verify") != m_usageMap.end();

    if (!hasSign && !hasVerify)
        return Runtime::setError("WebCrypto: HMAC key must have at least 'sign' or 'verify' usage");

    // Check for invalid usages
    for (auto& it : m_usageMap) {
        if (qstrcmp(it.first.c_str(), "sign")
            && qstrcmp(it.first.c_str(), "verify"))
            return Runtime::setError("WebCrypto: HMAC key must not have '" + it.first + "' usage");
    }

    return 0;
}

result_t CryptoKey::check_name(exlib::string name)
{
    if (m_key_type == kKeyNameECDSA && qstricmp(name.c_str(), "ecdsa"))
        return Runtime::setError("CryptoKey: invalid key algorithm, expected 'ECDSA' but got '%s'.", name.c_str());

    if (m_key_type == kKeyNameEd25519 && qstricmp(name.c_str(), "ed25519"))
        return Runtime::setError("CryptoKey: invalid key algorithm, expected 'Ed25519' but got '%s'.", name.c_str());

    if (m_key_type == kKeyNameECDH && qstricmp(name.c_str(), "ecdh"))
        return Runtime::setError("CryptoKey: invalid key algorithm, expected 'ECDH' but got '%s'.", name.c_str());

    if (m_key_type == kKeyNameHMAC && qstricmp(name.c_str(), "hmac"))
        return Runtime::setError("CryptoKey: invalid key algorithm, expected 'HMAC' but got '%s'.", name.c_str());

    return 0;
}

}
