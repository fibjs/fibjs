/*
 * BlsKey.h
 *
 *  Created on: Jan 29, 2023
 *      Author: lion
 */

#pragma once

#include "ECKey.h"
#include "ifs/BlsKey.h"
#include <blst/include/blst.h>

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

public:
    // BlsKey
    virtual result_t bbsSign(Buffer_base* header, v8::Local<v8::Array> messages, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t bbsVerify(Buffer_base* header, v8::Local<v8::Array> messages, Buffer_base* sig, bool& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t proofGen(Buffer_base* sig, Buffer_base* header, Buffer_base* proofHeader, v8::Local<v8::Array> messages, v8::Local<v8::Array> idx, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

    virtual result_t proofVerify(Buffer_base* header, Buffer_base* proofHeader, v8::Local<v8::Array> messages, v8::Local<v8::Array> idx, Buffer_base* proof, bool& retVal, AsyncEvent* ac)
    {
        return CALL_E_INVALID_CALL;
    }

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

public:
    // BlsKey
    virtual result_t bbsSign(Buffer_base* header, v8::Local<v8::Array> messages, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t bbsVerify(Buffer_base* header, v8::Local<v8::Array> messages, Buffer_base* sig, bool& retVal, AsyncEvent* ac);
    virtual result_t proofGen(Buffer_base* sig, Buffer_base* header, Buffer_base* proofHeader, v8::Local<v8::Array> messages, v8::Local<v8::Array> idx, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t proofVerify(Buffer_base* header, Buffer_base* proofHeader, v8::Local<v8::Array> messages, v8::Local<v8::Array> idx, Buffer_base* proof, bool& retVal, AsyncEvent* ac);

private:
    blst_p2 get_pk();
    blst_scalar get_sk();
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

}
