/*
 * PKey_impl.h
 *
 *  Created on: Apr 30, 2022
 *      Author: lion
 */

#include "PKey.h"
#include "ifs/ECCKey.h"
#include "ifs/Ed25519Key.h"

namespace fibjs {

class ECCKey : public PKey {
public:
    ECCKey()
    {
        mbedtls_pk_setup(&m_key, mbedtls_pk_info_from_type(MBEDTLS_PK_ECKEY));
    }

    ECCKey(mbedtls_pk_context& key)
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
    // ECCKey
    result_t get_curve(exlib::string& retVal);
    result_t computeSecret(ECCKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    static ECCKey_base* create(mbedtls_pk_context& key, exlib::string algo);
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

    virtual result_t set_alg(exlib::string newVal)
    {
        return _PKey::set_alg(newVal);
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

    virtual result_t equals(PKey_base* key, bool& retVal)
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

class PKey_rsa : public PKey_impl<PKey_base> {
public:
    PKey_rsa(int32_t size);
    PKey_rsa(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal);
    virtual result_t clone(obj_ptr<PKey_base>& retVal);
    virtual result_t equals(PKey_base* key, bool& retVal);
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    static result_t generateKey(int32_t size, obj_ptr<PKey_base>& retVal);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

template <class base, class _PKey = ECCKey>
class ECCKey_impl : public PKey_impl<base, _PKey> {
public:
    ECCKey_impl()
    {
    }

    ECCKey_impl(mbedtls_pk_context& key)
        : PKey_impl<base, _PKey>(key)
    {
    }

public:
    ECCKey_impl(int32_t id)
        : PKey_impl<base, _PKey>()
    {
        _PKey::init(id);
    }

    ECCKey_impl(mbedtls_pk_context& key, bool genpub, exlib::string algo = "")
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
    // ECCKey
    virtual result_t get_curve(exlib::string& retVal)
    {
        return _PKey::get_curve(retVal);
    }

    virtual result_t computeSecret(ECCKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return _PKey::computeSecret(publicKey, retVal, ac);
    }
};

class PKey_p256k1 : public ECCKey_impl<ECCKey_base> {
public:
    PKey_p256k1();
    PKey_p256k1(mbedtls_pk_context& key, exlib::string algo = "");

public:
    // PKey
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

class PKey_25519 : public ECCKey_impl<Ed25519Key_base> {
public:
    PKey_25519(int32_t id);
    PKey_25519(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t toX25519(obj_ptr<ECCKey_base>& retVal, AsyncEvent* ac);
    virtual result_t pem(exlib::string& retVal);
    virtual result_t der(obj_ptr<Buffer_base>& retVal);
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);
    virtual result_t computeSecret(ECCKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    static result_t from(Buffer_base* DerKey, obj_ptr<PKey_base>& retVal);
    static result_t from(exlib::string pemKey, obj_ptr<PKey_base>& retVal);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

class PKey_bls_g1 : public ECCKey_impl<ECCKey_base> {
public:
    PKey_bls_g1();
    PKey_bls_g1(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    static result_t mpi_load(Isolate* isolate, mbedtls_mpi* n, v8::Local<v8::Object> o);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

class PKey_bls_g2 : public ECCKey_impl<ECCKey_base> {
public:
    PKey_bls_g2();
    PKey_bls_g2(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    static result_t mpi_load(Isolate* isolate, mbedtls_mpi* n, v8::Local<v8::Object> o);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

}
