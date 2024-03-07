/*
 * SecureContext.h
 *
 *  Created on: Mar 4, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/SecureContext.h"
#include "crypto_util.h"

namespace fibjs {

class SecureContext : public SecureContext_base {
public:
    // SecureContext_base
    virtual result_t get_ca(obj_ptr<X509Certificate_base>& retVal);
    virtual result_t get_key(obj_ptr<KeyObject_base>& retVal);
    virtual result_t get_cert(obj_ptr<X509Certificate_base>& retVal);
    virtual result_t get_maxVersion(exlib::string& retVal);
    virtual result_t get_minVersion(exlib::string& retVal);
    virtual result_t get_secureProtocol(exlib::string& retVal);
    virtual result_t get_rejectUnauthorized(bool& retVal);
    virtual result_t get_sessionTimeout(int32_t& retVal);

public:
    result_t SetRootCerts();
    result_t init(v8::Local<v8::Object> options, bool is_server);
    SSL_CTX* ctx() { return m_ctx; }

private:
    void init_ctx(const SSL_METHOD* method);

    result_t set_secureProtocol(v8::Local<v8::Object> options, bool is_server);
    result_t set_ca(v8::Local<v8::Object> options, bool is_server);
    result_t set_ca(X509Certificate_base* cas);
    result_t set_cert(v8::Local<v8::Object> options);
    result_t set_key(v8::Local<v8::Object> options);
    result_t set_verify(v8::Local<v8::Object> options);
    result_t set_sessionTimeout(v8::Local<v8::Object> options);

private:
    SSLCtxPointer m_ctx;
    obj_ptr<X509Certificate_base> m_ca;
    obj_ptr<X509Certificate_base> m_cert;
    obj_ptr<KeyObject_base> m_key;
};

}
