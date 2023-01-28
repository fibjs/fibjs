/*
 * ECKey.h
 *
 *  Created on: Jan 28, 2023
 *      Author: lion
 */

#pragma once

#include "PKey.h"
#include "ifs/ECKey.h"

namespace fibjs {

class ECKey : public PKey {
public:
    ECKey()
    {
        mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    }

    ECKey(mbedtls_pk_context& key)
        : PKey(key)
    {
    }

public:
    void init(int32_t id);
    void init(mbedtls_pk_context& key, bool genpub, exlib::string algo = "");

public:
    // PKey
    result_t set_alg(exlib::string newVal);
    result_t get_publicKey(obj_ptr<PKey_base>& retVal);
    result_t clone(obj_ptr<PKey_base>& retVal);
    result_t equals(PKey_base* key, bool& retVal);
    result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    // ECKey
    result_t get_curve(exlib::string& retVal);
    result_t computeSecret(ECKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    static ECKey_base* create(mbedtls_pk_context& key, exlib::string algo);
    static result_t generateKey(exlib::string curve, obj_ptr<PKey_base>& retVal);

public:
    static int32_t get_curve_id(exlib::string& curve);
    static const char* get_curve_name(int32_t id);

protected:
    result_t der2bin(const exlib::string& der, exlib::string& bin);
    result_t bin2der(const exlib::string& bin, exlib::string& der);

private:
    result_t sign(Buffer_base* data, PKey_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    result_t verify(Buffer_base* data, Buffer_base* sign, PKey_base* key, bool& retVal, AsyncEvent* ac);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);

public:
    static int load_group(mbedtls_ecp_group* grp, int32_t id);
};

template <class base, class _PKey = ECKey>
class ECKey_impl : public PKey_impl<base, _PKey> {
public:
    ECKey_impl()
    {
    }

    ECKey_impl(mbedtls_pk_context& key)
        : PKey_impl<base, _PKey>(key)
    {
    }

public:
    ECKey_impl(int32_t id)
        : PKey_impl<base, _PKey>()
    {
        _PKey::init(id);
    }

    ECKey_impl(mbedtls_pk_context& key, bool genpub, exlib::string algo = "")
        : PKey_impl<base, _PKey>(key)
    {
        _PKey::init(key, genpub, algo);
    }

public:
    // PKey
    virtual result_t set_alg(exlib::string newVal)
    {
        return _PKey::set_alg(newVal);
    }

    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal)
    {
        return _PKey::get_publicKey(retVal);
    }

    virtual result_t clone(obj_ptr<PKey_base>& retVal)
    {
        return _PKey::clone(retVal);
    }

    virtual result_t equals(PKey_base* key, bool& retVal)
    {
        return _PKey::equals(key, retVal);
    }

    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return _PKey::sign(data, opts, retVal, ac);
    }
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac)
    {
        return _PKey::verify(data, sign, opts, retVal, ac);
    }

public:
    // ECKey
    virtual result_t get_curve(exlib::string& retVal)
    {
        return _PKey::get_curve(retVal);
    }

    virtual result_t computeSecret(ECKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return _PKey::computeSecret(publicKey, retVal, ac);
    }
};
}
