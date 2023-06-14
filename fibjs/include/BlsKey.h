/*
 * BlsKey.h
 *
 *  Created on: Jan 29, 2023
 *      Author: lion
 */

#pragma once

#include "ECKey.h"
#include "ifs/BlsKey.h"

namespace fibjs {

class BlsKey_g1 : public ECKey_impl<BlsKey_base> {
public:
    BlsKey_g1();
    BlsKey_g1(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    // ECKey
    result_t computeSecret(ECKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

class BlsKey_g2 : public ECKey_impl<BlsKey_base> {
public:
    BlsKey_g2();
    BlsKey_g2(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    // ECKey
    result_t computeSecret(ECKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

}
