/*
 * Ed25519Key.h
 *
 *  Created on: Jan 28, 2023
 *      Author: lion
 */

#pragma once

#include "ECKey.h"
#include "ifs/Ed25519Key.h"

namespace fibjs {

class Ed25519Key : public ECKey_impl<Ed25519Key_base> {
public:
    Ed25519Key(int32_t id);
    Ed25519Key(mbedtls_pk_context& key);

public:
    // PKey
    virtual result_t toX25519(obj_ptr<ECKey_base>& retVal, AsyncEvent* ac);
    virtual result_t pem(exlib::string& retVal);
    virtual result_t der(obj_ptr<Buffer_base>& retVal);
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac);
    virtual result_t computeSecret(ECKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

public:
    static result_t from(Buffer_base* DerKey, obj_ptr<PKey_base>& retVal);
    static result_t from(exlib::string pemKey, obj_ptr<PKey_base>& retVal);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};
}
