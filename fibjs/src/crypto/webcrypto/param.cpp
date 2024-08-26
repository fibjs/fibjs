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
    Isolate* isolate = holder();
    result_t hr;
    exlib::string name;

    m_extractable = extractable;

    int32_t len = usages->Length();

    for (int32_t i = 0; i < len; i++) {
        exlib::string usage;
        hr = GetConfigValue(isolate, usages, i, usage, true);
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

    hr = GetConfigValue(isolate, params, "name", name, true);
    if (hr < 0)
        return hr;

    if (qstricmp(name.c_str(), "ECDSA") == 0)
        return get_ecdsa_param(params);

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
    Isolate* isolate = holder();
    result_t hr;
    exlib::string namedCurve;

    m_key_type = kKeyNameECDSA;
    m_algorithm->add("name", "ECDSA");

    hr = GetConfigValue(isolate, params, "namedCurve", namedCurve, true);
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
        return Runtime::setError("WebCrypto: invalid key algorithm");

    return check_asymmetric_import_usage();
}

result_t CryptoKey::check_import_param()
{
    if (m_key_type == kKeyNameECDSA)
        return check_ecdsa_import_param();

    return check_asymmetric_import_usage();
}

result_t CryptoKey::check_name(exlib::string name)
{
    if (m_key_type == kKeyNameECDSA && qstricmp(name.c_str(), "ecdsa"))
        return Runtime::setError("CryptoKey: invalid key algorithm");

    return 0;
}

}
