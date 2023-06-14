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

class PKey {
public:
    PKey();
    PKey(mbedtls_pk_context& key);
    virtual ~PKey();

public:
    static PKey_base* create(mbedtls_pk_context& key, bool clone = false);

public:
    // object_base
    result_t toString(exlib::string& retVal);
    result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal);

public:
    // PKey_base
    result_t get_name(exlib::string& retVal);
    result_t get_keySize(int32_t& retVal);
    result_t get_alg(exlib::string& retVal);
    result_t get_publicKey(obj_ptr<PKey_base>& retVal);
    result_t isPrivate(bool& retVal);
    result_t clone(obj_ptr<PKey_base>& retVal);
    result_t pem(exlib::string& retVal);
    result_t der(obj_ptr<Buffer_base>& retVal);
    result_t json(v8::Local<v8::Object> opts, v8::Local<v8::Object>& retVal);
    result_t equals(object_base* key, bool& retVal);
    result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    static mbedtls_pk_context& key(PKey_base* key);

public:
    static result_t loadFile(exlib::string filename, obj_ptr<PKey_base>& retVal);

public:
    mbedtls_pk_context m_key;
    exlib::string m_alg;
};

template <class base, class _PKey = PKey>
class PKey_impl : public base, public _PKey {

public:
    PKey_impl()
    {
    }

    PKey_impl(mbedtls_pk_context& key)
        : _PKey(key)
    {
    }

public:
    // object_base
    virtual result_t toString(exlib::string& retVal)
    {
        return _PKey::toString(retVal);
    }

    virtual result_t toJSON(exlib::string key, v8::Local<v8::Value>& retVal)
    {
        return _PKey::toJSON(key, retVal);
    }

public:
    // PKey_base
    virtual result_t get_name(exlib::string& retVal)
    {
        return _PKey::get_name(retVal);
    }

    virtual result_t get_keySize(int32_t& retVal)
    {
        return _PKey::get_keySize(retVal);
    }

    virtual result_t get_alg(exlib::string& retVal)
    {
        return _PKey::get_alg(retVal);
    }

    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal)
    {
        return _PKey::get_publicKey(retVal);
    }

    virtual result_t isPrivate(bool& retVal)
    {
        return _PKey::isPrivate(retVal);
    }

    virtual result_t clone(obj_ptr<PKey_base>& retVal)
    {
        return _PKey::clone(retVal);
    }

    virtual result_t pem(exlib::string& retVal)
    {
        return _PKey::pem(retVal);
    }

    virtual result_t der(obj_ptr<Buffer_base>& retVal)
    {
        return _PKey::der(retVal);
    }

    virtual result_t json(v8::Local<v8::Object> opts, v8::Local<v8::Object>& retVal)
    {
        return _PKey::json(opts, retVal);
    }

    virtual result_t equals(object_base* key, bool& retVal)
    {
        return _PKey::equals(key, retVal);
    }

    virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return _PKey::encrypt(data, retVal, ac);
    }

    virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return _PKey::decrypt(data, retVal, ac);
    }

    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return _PKey::sign(data, opts, retVal, ac);
    }

    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
    {
        return _PKey::verify(data, sign, opts, retVal, ac);
    }
};

}
