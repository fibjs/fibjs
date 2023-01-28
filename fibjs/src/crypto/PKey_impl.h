/*
 * PKey_impl.h
 *
 *  Created on: Apr 30, 2022
 *      Author: lion
 */

#include "PKey.h"
#include "ECCKey.h"

namespace fibjs {

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
