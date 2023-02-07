/*
 * PKey_impl.h
 *
 *  Created on: Apr 30, 2022
 *      Author: lion
 */

#pragma once

#include "PKey.h"
#include "ECKey.h"

namespace fibjs {

class ECKey_p256k1 : public ECKey_impl<ECKey_base> {
public:
    ECKey_p256k1();
    ECKey_p256k1(mbedtls_pk_context& key, exlib::string algo = "");

public:
    // PKey
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);

private:
    result_t check_opts(v8::Local<v8::Object> opts, AsyncEvent* ac);
};

}
