/*
 * PKey_rsa.h
 *
 *  Created on: Jan 28, 2023
 *      Author: lion
 */

#pragma once

#include "PKey.h"

namespace fibjs {

class PKey_rsa : public PKey_impl<PKey_base> {
public:
    PKey_rsa(int32_t size);
    PKey_rsa(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal);
    virtual result_t clone(obj_ptr<PKey_base>& retVal);
    result_t equals(object_base* key, bool& retVal);
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);

public:
    static result_t generateKey(int32_t size, obj_ptr<PKey_base>& retVal);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};
}