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

class generateKeyPairParam : public obj_base {
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

class KeyObject : public KeyObject_base {
public:
    enum PKEncodingType {
        kKeyEncodingPKCS1,
        kKeyEncodingPKCS8,
        kKeyEncodingSPKI,
        kKeyEncodingSEC1
    };

    enum KeyType {
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
    result_t createSecretKey(const unsigned char* key, size_t size);
    result_t createAsymmetricKey(v8::Local<v8::Object> key, KeyType type);
    result_t ImportJWKAsymmetricKey(v8::Local<v8::Object> hwk, KeyType type);
    result_t ImportJWKRsaKey(v8::Local<v8::Object> key, KeyType type);
    result_t ImportJWKEcKey(v8::Local<v8::Object> key, KeyType type);
    result_t ImportJWKEdKey(v8::Local<v8::Object> key, KeyType type);

public:
    result_t TryParsePublicKey(const BIOPointer& bp, const char* name, const std::function<EVP_PKEY*(const unsigned char** p, long l)>& parse);
    result_t ParsePublicKeyPEM(const char* key_pem, int key_pem_len);
    result_t createPublicKeyFromPrivateKey(KeyObject_base* key);
    result_t createPublicKeyFromPrivateKey(EVP_PKEY* key);

public:
    result_t ParsePrivateKeyPEM(const char* key_pem, int key_pem_len, Buffer* passphrase);

public:
    result_t ExportPublicKey(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal);
    result_t ParsePublicKey(v8::Local<v8::Object> key);

public:
    result_t ExportPrivateKey(v8::Local<v8::Object> options, v8::Local<v8::Value>& retVal);
    result_t ParsePrivateKey(v8::Local<v8::Object> key);

public:
    result_t export_json(v8::Local<v8::Value>& retVal);
    result_t ExportJWKEdKey(v8::Local<v8::Value>& retVal);
    result_t ExportJWKRsaKey(v8::Local<v8::Value>& retVal);
    result_t ExportJWKEcKey(v8::Local<v8::Value>& retVal);
    result_t ExportJWKSecretKey(v8::Local<v8::Value>& retVal);

public:
    result_t generateKey(exlib::string type, generateKeyPairParam* param);
    result_t generateRsaKey(int nid, generateKeyPairParam* param);
    result_t generateDsaKey(int nid, generateKeyPairParam* param);
    result_t generateEcKey(int nid, generateKeyPairParam* param);
    result_t generateEdKey(int nid, generateKeyPairParam* param);

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
