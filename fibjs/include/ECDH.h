/*
 * ECDH.h
 *
 *  Created on: Oct 17, 2025
 *      Author: lion
 */

#pragma once

#include "ifs/ECDH.h"
#include "Buffer.h"
#include "crypto_util.h"

namespace fibjs {

class ECDH : public ECDH_base {
public:
    ECDH(exlib::string curve)
        : m_curve(curve)
        , m_ec(nullptr)
        , m_keysGenerated(false)
    {
    }

    ~ECDH()
    {
        if (m_ec)
            EC_KEY_free(m_ec);
    }

public:
    // ECDH_base
    virtual result_t computeSecret(v8::Local<v8::Value> otherKey, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal);
    virtual result_t generateKeys(exlib::string outputEncoding, exlib::string format, v8::Local<v8::Value>& retVal);
    virtual result_t get_curveName(exlib::string& retVal);
    virtual result_t getPrivateKey(exlib::string encoding, v8::Local<v8::Value>& retVal);
    virtual result_t getPublicKey(exlib::string encoding, exlib::string format, v8::Local<v8::Value>& retVal);
    virtual result_t setPrivateKey(v8::Local<v8::Value> privateKey, exlib::string encoding);
    virtual result_t setPublicKey(v8::Local<v8::Value> publicKey, exlib::string encoding);

    // Helper functions for convertKey
    result_t ensureKey();
    result_t validatePointFormat(const exlib::string& format);
    const EC_GROUP* getGroup() const { return m_ec ? EC_KEY_get0_group(m_ec) : nullptr; }

private:
    exlib::string m_curve;
    EC_KEY* m_ec;
    bool m_keysGenerated;
    bool m_publicKeySet;
    bool m_privateKeySet;
};

} /* namespace fibjs */
