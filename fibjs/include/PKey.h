/*
 * PKey.h
 *
 *  Created on: Apr 11, 2014
 *      Author: lion
 */

#pragma once

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "ifs/PKey.h"
#include <mbedtls/mbedtls/mbedtls_config.h>
#include <mbedtls/mbedtls/pk.h>

namespace fibjs {

#define MBEDTLS_ECP_DP_ED25519 1001
#define MBEDTLS_ECP_DP_BLS12381_G1 1002
#define MBEDTLS_ECP_DP_BLS12381_G2 1003

class PKey : public PKey_base {
public:
    PKey();
    ~PKey();

public:
    // object_base
    virtual result_t toString(exlib::string& retVal);
    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    // PKey_base
    virtual result_t get_name(exlib::string& retVal);
    virtual result_t get_curve(exlib::string& retVal);
    virtual result_t get_keySize(int32_t& retVal);
    virtual result_t get_alg(exlib::string& retVal);
    virtual result_t set_alg(exlib::string newVal);
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal);
    virtual result_t isPrivate(bool& retVal);
    virtual result_t clone(obj_ptr<PKey_base>& retVal);
    virtual result_t import(Buffer_base* DerKey, exlib::string password);
    virtual result_t import(exlib::string pemKey, exlib::string password);
    virtual result_t import(v8::Local<v8::Object> jsonKey);
    virtual result_t importFile(exlib::string filename, exlib::string password);
    virtual result_t pem(exlib::string& retVal);
    virtual result_t der(obj_ptr<Buffer_base>& retVal);
    virtual result_t json(v8::Local<v8::Object>& retVal);
    virtual result_t equal(PKey_base* key, bool& retVal);
    virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t sign(Buffer_base* data, int32_t alg, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t sign(Buffer_base* data, PKey_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, int32_t alg, bool& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, PKey_base* key, bool& retVal, AsyncEvent* ac);
    virtual result_t computeSecret(PKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    result_t copy(const mbedtls_pk_context& key);
    result_t generateKey(int32_t size, AsyncEvent* ac);
    result_t generateKey(exlib::string curve, AsyncEvent* ac);

private:
    void clear();

    void def_alg();
    void def_ec_alg(int32_t id);

private:
    result_t ed25519_generateKey();
    int ed25519_get_pubkey();

    int ed25519_parse_key(const unsigned char* key, size_t keylen);
    int ed25519_parse_pub_key(const unsigned char* key, size_t keylen);
    int ed25519_parse_pem(const unsigned char* key);

    int ed25519_write_key(exlib::string& buf);
    int ed25519_write_pub_key(exlib::string& buf);
    int ed25519_write_pem(exlib::string& buf);
    result_t ed25519_write_der(obj_ptr<Buffer_base>& retVal);

    result_t ed25519_sign(Buffer_base* data, obj_ptr<Buffer_base>& retVal);
    result_t ed25519_verify(Buffer_base* data, Buffer_base* sign, bool& retVal);

private:
    result_t bls_generateKey(int32_t type);
    int bls_get_pubkey();
    result_t bls_sign(Buffer_base* data, obj_ptr<Buffer_base>& retVal);
    result_t bls_verify(Buffer_base* data, Buffer_base* sign, bool& retVal);

public:
    mbedtls_pk_context m_key;
    exlib::string m_alg;
};
}
