/*
 * CryptoKey.h
 *
 *  Created on: Aug 25, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/CryptoKey.h"
#include "KeyObject.h"
#include "SimpleObject.h"

namespace fibjs {

enum KeyName {
    kKeyNameNone,
    kKeyNameAES,
    kKeyNameHMAC,
    kKeyNameRSA,
    kKeyNameECDSA
};

class CryptoKey : public CryptoKey_base {
public:
    // CryptoKey_base
    virtual result_t get_type(exlib::string& retVal);
    virtual result_t get_algorithm(v8::Local<v8::Object>& retVal);
    virtual result_t get_extractable(bool& retVal);
    virtual result_t get_usages(v8::Local<v8::Array>& retVal);

public:
    result_t get_param(v8::Local<v8::Object> params, bool extractable, v8::Local<v8::Array> usages);
    result_t get_ecdsa_param(v8::Local<v8::Object> params);
    result_t check_asymmetric_usage();

public:
    result_t check_import_param();
    result_t check_ecdsa_import_param();
    result_t check_asymmetric_import_usage();

public:
    result_t generate();
    result_t generate_ecdsa();

public:
    result_t createPublicKey();

public:
    KeyObject::KeyType type()
    {
        return m_key->type();
    }

public:
    KeyName m_key_type = kKeyNameNone;
    obj_ptr<KeyObject> m_key;
    obj_ptr<NObject> m_algorithm = new NObject();
    std::unordered_map<exlib::string, bool> m_usageMap;
    bool m_extractable = false;

public:
    obj_ptr<CryptoKey> m_publicKey;
};

}
