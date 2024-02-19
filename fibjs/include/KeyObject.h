/*
 * KeyObject.h
 *
 *  Created on: Feb 19, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/KeyObject.h"

namespace fibjs {

class KeyObject : public KeyObject_base {
public:
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

private:
    KeyType m_keyType;
    std::vector<unsigned char> m_key;
};

}
