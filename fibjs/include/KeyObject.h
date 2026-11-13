/*
 * KeyObject.h
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#pragma once

#include "crypto_util.h"
#include "ifs/KeyObject.h"
#include <optional>

namespace fibjs {

#define GET_OPT_PARAM(name)                             \
    hr = GetConfigValue(options, #name, o->name, true); \
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))        \
        return hr;

struct KeyObjectOptions;

class generateKeyPairParam : public obj_base {
public:
    static result_t load(v8::Local<v8::Object> options, obj_ptr<generateKeyPairParam>& retVal)
    {
        obj_ptr<generateKeyPairParam> o = new generateKeyPairParam();
        result_t hr;

        GET_OPT_PARAM(modulusLength);
        GET_OPT_PARAM(publicExponent);
        GET_OPT_PARAM(hashAlgorithm);
        GET_OPT_PARAM(mgf1Algorithm);
        GET_OPT_PARAM(saltLength);
        GET_OPT_PARAM(divisorLength);
        GET_OPT_PARAM(namedCurve);
        GET_OPT_PARAM(paramEncoding);

        retVal = o;

        return result_t();
    }

public:
    int32_t modulusLength = 0;
    int32_t publicExponent = 0x10001;
    exlib::string hashAlgorithm;
    exlib::string mgf1Algorithm;
    int32_t saltLength = -1;
    int32_t divisorLength = -1;
    exlib::string namedCurve;
    exlib::string paramEncoding;
};

class keyEncodingParam : public obj_base {
public:
    static result_t load(v8::Local<v8::Object> options, obj_ptr<keyEncodingParam>& retVal)
    {
        obj_ptr<keyEncodingParam> o = new keyEncodingParam();
        result_t hr;

        GET_OPT_PARAM(format);
        GET_OPT_PARAM(type);
        GET_OPT_PARAM(cipher);
        GET_OPT_PARAM(passphrase);

        retVal = o;

        return result_t();
    }

    static result_t load(v8::Local<v8::Object> options, const char* key, obj_ptr<keyEncodingParam>& retVal)
    {
        result_t hr;

        v8::Local<v8::Object> opt;
        hr = GetConfigValue(options, key, opt, true);
        if (hr.is_param_missing())
            return result_t();
        if (hr.failed())
            return hr;

        return load(opt, retVal);
    }

public:
    exlib::string format;
    exlib::string type;
    exlib::string cipher;
    obj_ptr<Buffer_base> passphrase;
};

class KeyObject : public KeyObject_base {
public:
    enum PKEncodingType {
        kKeyEncodingPKCS1,
        kKeyEncodingPKCS8,
        kKeyEncodingSPKI,
        kKeyEncodingSEC1
    };

    enum KeyType {
        kKeyTypeUnknown,
        kKeyTypeSecret,
        kKeyTypePublic,
        kKeyTypePrivate
    };

public:
    // KeyObject_base
    virtual result_t get_asymmetricKeyDetails(v8::Local<v8::Object>& retVal);
    virtual result_t get_asymmetricKeyType(exlib::string& retVal);
    virtual result_t get_symmetricKeySize(int32_t& retVal);
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t _export(KeyEncodingOptions* options, v8::Local<v8::Value>& retVal);
    virtual result_t equals(KeyObject_base* otherKey, bool& retVal);

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    result_t createSecretKey(const unsigned char* key, size_t size);
    result_t createAsymmetricKey(v8::Local<v8::Object> key, KeyType type);
    result_t createAsymmetricKey(const KeyObjectOptions& options, KeyType type);
    result_t ImportJWKKey(NObject* hwk, KeyType type);
    result_t ImportJWKRsaKey(NObject* key, KeyType type);
    result_t ImportJWKEcKey(NObject* key, KeyType type);
    result_t ImportJWKOKPKey(NObject* key, KeyType type);

    result_t ImportJWKKey(v8::Local<v8::Object> jwk, KeyType type)
    {
        obj_ptr<NObject> key = new NObject();
        key->add(jwk);
        return ImportJWKKey(key, type);
    }

public:
    result_t TryParsePublicKey(const BIOPointer& bp, const char* name, const std::function<EVP_PKEY*(const unsigned char** p, long l)>& parse);
    result_t ParsePublicKeyPEM(const char* key_pem, int key_pem_len);
    result_t createPublicKeyFromKeyObject(KeyObject_base* key);
    result_t createPublicKeyFromPKey(EVP_PKEY* key);
    result_t fixSM2PublicKey();

public:
    result_t ParsePrivateKeyPEM(const char* key_pem, int key_pem_len, Buffer* passphrase);

public:
    result_t ExportKey(keyEncodingParam* param, Variant& retVal);

public:
    result_t ExportPublicKey(exlib::string format, exlib::string type, Variant& retVal, bool useBackingStore = false);
    result_t ExportPublicKey(keyEncodingParam* param, Variant& retVal, bool useBackingStore = false);
    result_t ExportPublicKey(keyEncodingParam* param, v8::Local<v8::Value>& retVal);

    result_t ParsePublicKey(exlib::string format, exlib::string type, exlib::string namedCurve, Buffer_base* passphrase, Buffer_base* key);
    result_t ParsePublicKey(v8::Local<v8::Object> key);

public:
    result_t ExportPrivateKey(exlib::string format, exlib::string type, exlib::string cipher_name,
        Buffer_base* passphrase, Variant& retVal, bool useBackingStore = false);
    result_t ExportPrivateKey(keyEncodingParam* param, Variant& retVal, bool useBackingStore = false);
    result_t ExportPrivateKey(keyEncodingParam* param, v8::Local<v8::Value>& retVal);

    result_t ParsePrivateKey(exlib::string format, exlib::string type, exlib::string namedCurve, Buffer_base* passphrase, Buffer_base* key);
    result_t ParsePrivateKey(v8::Local<v8::Object> key);

public:
    result_t export_json(Variant& retVal);
    result_t ExportJWKOKPKey(Variant& retVal);
    result_t ExportJWKRsaKey(Variant& retVal);
    result_t ExportJWKEcKey(Variant& retVal);
    result_t ExportJWKSecretKey(Variant& retVal);

public:
    result_t generateKey(exlib::string type, generateKeyPairParam* param);
    result_t generateRsaKey(int nid, generateKeyPairParam* param);
    result_t generateDsaKey(int nid, generateKeyPairParam* param);
    result_t generateEcKey(int nid, generateKeyPairParam* param);
    result_t generateOKPKey(int nid, generateKeyPairParam* param);

public:
    result_t toX25519_privateKey(v8::Local<v8::Object> options);
    result_t toX25519_privateKey(bool toX25519);
    result_t toX25519_publicKey(v8::Local<v8::Object> options);
    result_t toX25519_publicKey(bool toX25519);

public:
    KeyType type() const
    {
        return m_keyType;
    }

    const unsigned char* data() const
    {
        return m_key.data();
    }

    size_t length() const
    {
        return m_key.size();
    }

    EVP_PKEY* pkey() const
    {
        return m_pkey;
    }

    const char* namedCurve() const
    {
        const EC_KEY* ec = EVP_PKEY_get0_EC_KEY(m_pkey);
        if (!ec)
            return nullptr;

        const EC_GROUP* group = EC_KEY_get0_group(ec);
        int nid = EC_GROUP_get_curve_name(group);

        const char* name = EC_curve_nid2nist(nid);
        if (!name)
            name = OBJ_nid2sn(nid);
        return name;
    }

private:
    result_t GetRsaKeyDetail(v8::Local<v8::Object>& retVal, bool is_pss);
    result_t GetDsaKeyDetail(v8::Local<v8::Object>& retVal);
    result_t GetEcKeyDetail(v8::Local<v8::Object>& retVal);
    result_t GetDhKeyDetail(v8::Local<v8::Object>& retVal);

private:
    KeyType m_keyType;
    std::vector<unsigned char> m_key;
    EVPKeyPointer m_pkey;
};

// The key-construction subset of a function's options record. Keeping it as
// its own record means the operation fields (padding, subject, suite, ...)
// never reach the key parser. The extractor is a template because each API has
// a different record shape; a field is copied only when it exists.
struct KeyObjectOptions {
    std::optional<Variant> key;
    std::optional<exlib::string> format;
    std::optional<exlib::string> type;
    std::optional<Variant> passphrase;
    std::optional<exlib::string> encoding;
    std::optional<exlib::string> namedCurve;
    std::optional<bool> toX25519;
};

template <typename T>
Variant to_key_variant(const T& value)
{
    return Variant(value);
}

template <typename T>
Variant to_key_variant(const obj_ptr<T>& value)
{
    obj_ptr<T> copy = value;
    return Variant(copy);
}

template <typename... Ts>
Variant to_key_variant(const std::variant<Ts...>& value)
{
    return std::visit([](const auto& v) { return Variant(v); }, value);
}

template <typename T>
void assign_key_option(std::optional<Variant>& out, const std::optional<T>& value)
{
    if (value.has_value())
        out = to_key_variant(value.value());
}

template <typename T>
void assign_key_option(std::optional<Variant>& out, const T& value)
{
    out = to_key_variant(value);
}

template <typename T>
void assign_key_option(std::optional<exlib::string>& out, const std::optional<T>& value)
{
    if (value.has_value())
        out = value.value();
}

template <typename T>
void assign_key_option(std::optional<exlib::string>& out, const T& value)
{
    out = value;
}

template <typename T>
void assign_key_option(std::optional<bool>& out, const std::optional<T>& value)
{
    if (value.has_value())
        out = value.value();
}

template <typename T>
void assign_key_option(std::optional<bool>& out, const T& value)
{
    out = value;
}

inline result_t extract_key_options(Isolate* isolate, v8::Local<v8::Object> options, KeyObjectOptions& out, bool include_type = true)
{
    if (options.IsEmpty())
        return result_t::type_mismatch();

    result_t hr = GetConfigValue(options, "key", out.key);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(options, "format", out.format, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    if (include_type) {
        hr = GetConfigValue(options, "type", out.type, true);
        if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
            return hr;
    }

    hr = GetConfigValue(options, "passphrase", out.passphrase);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(options, "encoding", out.encoding, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(options, "namedCurve", out.namedCurve, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    hr = GetConfigValue(options, "toX25519", out.toX25519, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    return result_t();
}

template <typename Options>
result_t extract_key_options(const Options& options, KeyObjectOptions& out, bool include_type = true)
{
#define COPY_KEY_OPTION(name) \
    if constexpr (requires { options.name; }) assign_key_option(out.name, options.name);

    COPY_KEY_OPTION(key);
    COPY_KEY_OPTION(format);
    if (include_type) {
        // `type` is the DER key type for most records, but a certificate
        // record uses the same name for its string array of certificate
        // types; only copy the field when it is a string option.
        if constexpr (requires { options.type; }
            && std::is_constructible_v<std::optional<exlib::string>, decltype(options.type)>)
            assign_key_option(out.type, options.type);
    }
    COPY_KEY_OPTION(passphrase);
    COPY_KEY_OPTION(encoding);
    COPY_KEY_OPTION(namedCurve);
    COPY_KEY_OPTION(toX25519);

#undef COPY_KEY_OPTION

    return result_t();
}

inline result_t extract_key_options(v8::Local<v8::Object> options, KeyObjectOptions& out, bool include_type = true)
{
    return extract_key_options(Isolate::current(options), options, out, include_type);
}

template <typename T>
result_t set_key_option(Isolate* isolate, v8::Local<v8::Object> obj, const char* name,
    const std::optional<T>& value)
{
    if (!value.has_value())
        return result_t();

    T copy = value.value();
    v8::Local<v8::Value> v = GetReturnValue(isolate, copy);
    if (v.IsEmpty())
        return result_t::type_mismatch();

    obj->Set(isolate->context(), isolate->NewString(name), v).IsJust();
    return result_t();
}

// Builds a private or public KeyObject from a structured key-import record.
// The record carries owned key material only, so the construction runs in the
// native phase (no V8 object is assembled for the parser).
inline result_t create_key_object(Isolate* isolate, const KeyObjectOptions& options, KeyObject::KeyType type,
    obj_ptr<KeyObject_base>& retVal)
{
    obj_ptr<KeyObject> key = new KeyObject();
    result_t hr = key->createAsymmetricKey(options, type);
    if (hr.failed())
        return hr;

    bool toX25519 = options.toX25519.value_or(false);
    if (type == KeyObject::kKeyTypePrivate)
        hr = key->toX25519_privateKey(toX25519);
    else if (type == KeyObject::kKeyTypePublic)
        hr = key->toX25519_publicKey(toX25519);
    if (hr.failed())
        return hr;

    retVal = key;
    return result_t();
}

// Any function-specific options shape that exposes the key-import fields can
// be handed to this overload; the operation fields stay in that record.
template <typename Options>
result_t create_key_object(Isolate* isolate, const Options& options, KeyObject::KeyType type,
    obj_ptr<KeyObject_base>& retVal, bool include_type = true)
{
    KeyObjectOptions key_options;
    result_t hr = extract_key_options(options, key_options, include_type);
    if (hr.failed())
        return hr;

    return create_key_object(isolate, key_options, type, retVal);
}

template <typename Options>
result_t create_key_object(Isolate* isolate, const obj_ptr<Options>& options, KeyObject::KeyType type,
    obj_ptr<KeyObject_base>& retVal, bool include_type = true)
{
    if (!options)
        return result_t::type_mismatch();

    return create_key_object(isolate, *options, type, retVal, include_type);
}

}
