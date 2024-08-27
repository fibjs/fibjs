/*
 * KeyObject.h
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#pragma once

#include "crypto_util.h"
#include "ifs/KeyObject.h"

namespace fibjs {

#define GET_OPT_PARAM(name)                                      \
    hr = GetConfigValue(isolate, options, #name, o->name, true); \
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)                 \
        return hr;

class generateKeyPairParam : public obj_base {
public:
    static result_t load(v8::Local<v8::Object> options, obj_ptr<generateKeyPairParam>& retVal)
    {
        obj_ptr<generateKeyPairParam> o = new generateKeyPairParam();
        Isolate* isolate = Isolate::current(options);
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

        return 0;
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
        Isolate* isolate = Isolate::current(options);
        result_t hr;

        GET_OPT_PARAM(format);
        GET_OPT_PARAM(type);
        GET_OPT_PARAM(cipher);
        GET_OPT_PARAM(passphrase);

        retVal = o;

        return 0;
    }

    static result_t load(v8::Local<v8::Object> options, const char* key, obj_ptr<keyEncodingParam>& retVal)
    {
        Isolate* isolate = Isolate::current(options);
        result_t hr;

        v8::Local<v8::Object> opt;
        hr = GetConfigValue(isolate, options, key, opt, true);
        if (hr == CALL_E_PARAMNOTOPTIONAL)
            return 0;
        if (hr < 0)
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
    virtual result_t _export(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal);
    virtual result_t equals(KeyObject_base* otherKey, bool& retVal);

public:
    // object_base
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    result_t createSecretKey(const unsigned char* key, size_t size);
    result_t createAsymmetricKey(v8::Local<v8::Object> key, KeyType type);
    result_t ImportJWKAsymmetricKey(NObject* hwk, KeyType type);
    result_t ImportJWKRsaKey(NObject* key, KeyType type);
    result_t ImportJWKEcKey(NObject* key, KeyType type);
    result_t ImportJWKOKPKey(NObject* key, KeyType type);

    result_t ImportJWKAsymmetricKey(v8::Local<v8::Object> jwk, KeyType type)
    {
        obj_ptr<NObject> key = new NObject();
        key->add(jwk);
        return ImportJWKAsymmetricKey(key, type);
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
    result_t ExportPublicKey(exlib::string format, exlib::string type, Variant& retVal);
    result_t ExportPublicKey(keyEncodingParam* param, Variant& retVal);
    result_t ExportPublicKey(keyEncodingParam* param, v8::Local<v8::Value>& retVal);

    result_t ParsePublicKey(exlib::string format, exlib::string type, exlib::string namedCurve, Buffer_base* passphrase, Buffer_base* key);
    result_t ParsePublicKey(v8::Local<v8::Object> key);

public:
    result_t ExportPrivateKey(exlib::string format, exlib::string type, exlib::string cipher_name,
        Buffer_base* passphrase, Variant& retVal);
    result_t ExportPrivateKey(keyEncodingParam* param, Variant& retVal);
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
    result_t toX25519_publicKey(v8::Local<v8::Object> options);

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

}
