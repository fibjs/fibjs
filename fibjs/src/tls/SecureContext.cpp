/*
 * SecureContext.c
 *
 *  Created on: Mar 4, 2024
 *      Author: lion
 */

#include "ifs/tls.h"
#include "ifs/crypto.h"
#include "SecureContext.h"
#include "X509Certificate.h"

namespace fibjs {

result_t tls_base::createSecureContext(v8::Local<v8::Object> options, bool isServer, obj_ptr<SecureContext_base>& retVal)
{
    obj_ptr<SecureContext> ctx = new SecureContext();
    result_t hr = ctx->init(options, isServer);
    if (hr)
        return hr;

    retVal = ctx;

    return 0;
}

result_t tls_base::createSecureContext(bool isServer, obj_ptr<SecureContext_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    return createSecureContext(v8::Object::New(isolate->m_isolate), isServer, retVal);
}

result_t SecureContext::init(v8::Local<v8::Object> options, bool isServer)
{
    Isolate* isolate = holder();
    result_t hr;

    hr = set_secureProtocol(options, isServer);
    if (hr < 0)
        return hr;

    hr = set_ca(options, isServer);
    if (hr < 0)
        return hr;

    hr = set_cert(options);
    if (hr < 0)
        return hr;

    hr = set_key(options);
    if (hr < 0)
        return hr;

    hr = set_verify(options, isServer);
    if (hr < 0)
        return hr;

    hr = set_sessionTimeout(options);
    if (hr < 0)
        return hr;

    return 0;
}

void SecureContext::init_ctx(const SSL_METHOD* method)
{
    m_ctx = SSL_CTX_new(method);

    SSL_CTX_set_options(m_ctx, SSL_OP_NO_SSLv2);
    SSL_CTX_set_options(m_ctx, SSL_OP_NO_SSLv3);
    SSL_CTX_set_options(m_ctx, SSL_OP_ALLOW_CLIENT_RENEGOTIATION);
    SSL_CTX_clear_mode(m_ctx, SSL_MODE_NO_AUTO_CHAIN);

    SSL_CTX_set_session_cache_mode(m_ctx,
        SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_INTERNAL | SSL_SESS_CACHE_NO_AUTO_CLEAR);
}

result_t SecureContext::set_ca(v8::Local<v8::Object> options, bool isServer)
{
    Isolate* isolate = holder();
    result_t hr;

    obj_ptr<X509Certificate_base> ca;
    hr = GetConfigValue(isolate, options, "ca", ca);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: ca must be a valid X509Certificate.");
    if (hr != CALL_E_PARAMNOTOPTIONAL)
        return set_ca(ca);
    else if (!isServer)
        return set_ca(isolate->m_ctx.As<SecureContext>()->m_ca);

    return 0;
}

result_t SecureContext::set_ca(X509Certificate_base* cas)
{
    X509_STORE* cert_store = SSL_CTX_get_cert_store(m_ctx);
    X509Certificate* now = (X509Certificate*)cas;
    while (now) {
        X509_STORE_add_cert(cert_store, now->cert());
        SSL_CTX_add_client_CA(m_ctx, now->cert());

        now = now->next();
    }

    m_ca = cas;

    return 0;
}

result_t SecureContext::SetRootCerts()
{
    init_ctx(TLS_client_method());

    obj_ptr<X509Certificate> root = new X509Certificate();

    root->LoadRootCerts();
    set_ca(root);

    SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);

    return 0;
}

result_t SecureContext::get_ca(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_ca)
        return CALL_RETURN_UNDEFINED;

    retVal = m_ca;

    return 0;
}

result_t SecureContext::set_key(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;
    obj_ptr<KeyObject_base> key;

    if (options->Has(context, isolate->NewString("key")).FromMaybe(false)) {
        hr = crypto_base::createPrivateKey(options, key);
        if (hr < 0)
            return hr;
    }

    if (key) {
        if (!m_cert)
            return Runtime::setError("SecureContext: cert must be set before key.");

        bool matched;
        hr = m_cert->checkPrivateKey(key, matched);
        if (hr < 0)
            return hr;

        if (!matched)
            return Runtime::setError("SecureContext: key and cert do not match.");

        SSL_CTX_use_PrivateKey(m_ctx, key.As<KeyObject>()->pkey());

        m_key = key;
    } else if (m_cert)
        return Runtime::setError("SecureContext: key must be set before cert.");

    return 0;
}

result_t SecureContext::get_key(obj_ptr<KeyObject_base>& retVal)
{
    if (!m_key)
        return CALL_RETURN_UNDEFINED;

    retVal = m_key;

    return 0;
}

result_t SecureContext::set_cert(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    result_t hr;
    obj_ptr<X509Certificate_base> certs;

    hr = GetConfigValue(isolate, options, "cert", certs);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: cert must be a valid X509Certificate.");

    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        X509Certificate* now = certs.As<X509Certificate>();

        SSL_CTX_use_certificate(m_ctx, now->cert());
        now = now->next();

        while (now) {
            SSL_CTX_add1_chain_cert(m_ctx, now->cert());
            now = now->next();
        }

        m_cert = certs;
    }

    return 0;
}

result_t SecureContext::get_cert(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_cert)
        return CALL_RETURN_UNDEFINED;

    retVal = m_cert;

    return 0;
}

inline result_t ver_id(const exlib::string& ver, int32_t& retVal)
{
    if (ver == "SSLv2")
        retVal = SSL2_VERSION;
    else if (ver == "SSLv3")
        retVal = SSL3_VERSION;
    else if (ver == "TLSv1")
        retVal = TLS1_VERSION;
    else if (ver == "TLSv1.1")
        retVal = TLS1_1_VERSION;
    else if (ver == "TLSv1.2")
        retVal = TLS1_2_VERSION;
    else if (ver == "TLSv1.3")
        retVal = TLS1_3_VERSION;
    else
        return Runtime::setError("SecureContext: TLS version \"" + ver + "\" is not supported.");

    return 0;
}

inline result_t ver_string(int32_t ver, exlib::string& retVal)
{
    switch (ver) {
    case SSL2_VERSION:
        retVal = "SSLv2";
        return 0;
    case SSL3_VERSION:
        retVal = "SSLv3";
        return 0;
    case TLS1_VERSION:
        retVal = "TLSv1";
        return 0;
    case TLS1_1_VERSION:
        retVal = "TLSv1.1";
        return 0;
    case TLS1_2_VERSION:
        retVal = "TLSv1.2";
        return 0;
    case TLS1_3_VERSION:
        retVal = "TLSv1.3";
        return 0;
    }

    return CALL_RETURN_UNDEFINED;
}

result_t SecureContext::set_secureProtocol(v8::Local<v8::Object> options, bool isServer)
{
    Isolate* isolate = holder();
    result_t hr;
    exlib::string ver;
    int32_t maxVersion = 0;
    int32_t minVersion = 0;

    hr = GetConfigValue(isolate, options, "maxVersion", ver, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: maxVersion must be a valid string.");

    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        hr = ver_id(ver, maxVersion);
        if (hr < 0)
            return hr;
    }

    hr = GetConfigValue(isolate, options, "minVersion", ver, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: minVersion must be a valid string.");

    if (hr != CALL_E_PARAMNOTOPTIONAL) {
        hr = ver_id(ver, minVersion);
        if (hr < 0)
            return hr;
    }

    const SSL_METHOD* method = nullptr;
    exlib::string secureProtocol;

    hr = GetConfigValue(isolate, options, "secureProtocol", secureProtocol, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: secureProtocol must be a valid string.");

    if (hr == CALL_E_PARAMNOTOPTIONAL || secureProtocol == "TLS_method") {
        method = TLS_method();
    } else if (secureProtocol == "TLS_client_method") {
        method = TLS_client_method();
    } else if (secureProtocol == "TLS_server_method") {
        method = TLS_server_method();
    } else {
        if (minVersion != 0 || maxVersion != 0)
            return Runtime::setError("SecureContext: secureProtocol \" + secureProtocol + \" does not support minVersion or maxVersion.");

        if (secureProtocol == "SSLv23_method") {
            maxVersion = TLS1_2_VERSION;
            method = TLS_method();
        } else if (secureProtocol == "SSLv23_client_method") {
            maxVersion = TLS1_2_VERSION;
            method = TLS_client_method();
        } else if (secureProtocol == "SSLv23_server_method") {
            maxVersion = TLS1_2_VERSION;
            method = TLS_server_method();
        } else if (secureProtocol == "TLSv1_method") {
            minVersion = maxVersion = TLS1_VERSION;
            method = TLS_method();
        } else if (secureProtocol == "TLSv1_client_method") {
            minVersion = maxVersion = TLS1_VERSION;
            method = TLS_client_method();
        } else if (secureProtocol == "TLSv1_server_method") {
            minVersion = maxVersion = TLS1_VERSION;
            method = TLS_server_method();
        } else if (secureProtocol == "TLSv1_1_method") {
            minVersion = maxVersion = TLS1_1_VERSION;
            method = TLS_method();
        } else if (secureProtocol == "TLSv1_1_client_method") {
            minVersion = maxVersion = TLS1_1_VERSION;
            method = TLS_client_method();
        } else if (secureProtocol == "TLSv1_1_server_method") {
            minVersion = maxVersion = TLS1_1_VERSION;
            method = TLS_server_method();
        } else if (secureProtocol == "TLSv1_2_method") {
            minVersion = maxVersion = TLS1_2_VERSION;
            method = TLS_method();
        } else if (secureProtocol == "TLSv1_2_client_method") {
            minVersion = maxVersion = TLS1_2_VERSION;
            method = TLS_client_method();
        } else if (secureProtocol == "TLSv1_2_server_method") {
            minVersion = maxVersion = TLS1_2_VERSION;
            method = TLS_server_method();
        } else if (secureProtocol == "TLSv1_3_method") {
            minVersion = maxVersion = TLS1_3_VERSION;
            method = TLS_method();
        } else if (secureProtocol == "TLSv1_3_client_method") {
            minVersion = maxVersion = TLS1_3_VERSION;
            method = TLS_client_method();
        } else if (secureProtocol == "TLSv1_3_server_method") {
            minVersion = maxVersion = TLS1_3_VERSION;
            method = TLS_server_method();
        } else
            return Runtime::setError("SecureContext: secureProtocol \"" + secureProtocol + "\" is not supported.");
    }

    if (isServer && method == TLS_client_method())
        return Runtime::setError("SecureContext: secureProtocol \"" + secureProtocol + "\" is not supported for server.");

    init_ctx(method);

    if (minVersion)
        SSL_CTX_set_min_proto_version(m_ctx, minVersion);
    if (maxVersion)
        SSL_CTX_set_max_proto_version(m_ctx, maxVersion);

    return 0;
}

result_t SecureContext::get_maxVersion(exlib::string& retVal)
{
    return ver_string(SSL_CTX_get_max_proto_version(m_ctx), retVal);
}

result_t SecureContext::get_minVersion(exlib::string& retVal)
{
    return ver_string(SSL_CTX_get_min_proto_version(m_ctx), retVal);
}

result_t SecureContext::get_secureProtocol(exlib::string& retVal)
{
    const SSL_METHOD* method = SSL_CTX_get_ssl_method(m_ctx);

    if (method == TLS_method())
        retVal = "TLS_method";
    else if (method == TLS_client_method())
        retVal = "TLS_client_method";
    else if (method == TLS_server_method())
        retVal = "TLS_server_method";
    else
        return CALL_RETURN_UNDEFINED;

    return 0;
}

result_t SecureContext::set_verify(v8::Local<v8::Object> options, bool isServer)
{
    Isolate* isolate = holder();
    result_t hr;

    int32_t verify_mode;
    bool requestCert = true;
    bool rejectUnverified = true;
    bool rejectUnauthorized = !isServer;

    hr = GetConfigValue(isolate, options, "requestCert", requestCert);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: requestCert must be a valid boolean.");

    hr = GetConfigValue(isolate, options, "rejectUnverified", rejectUnverified);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: rejectUnverified must be a valid boolean.");

    hr = GetConfigValue(isolate, options, "rejectUnauthorized", rejectUnauthorized);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: rejectUnauthorized must be a valid boolean.");

    if (!requestCert)
        verify_mode = SSL_VERIFY_NONE;
    else {
        verify_mode = SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE;
        if (rejectUnauthorized)
            verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
    }

    SSL_CTX_set_verify(
        m_ctx, verify_mode, rejectUnverified ? nullptr : [](int ok, X509_STORE_CTX* ctx) -> int {
            return 1;
        });

    return 0;
}

result_t SecureContext::get_requestCert(bool& retVal)
{
    retVal = !!(SSL_CTX_get_verify_mode(m_ctx) & SSL_VERIFY_PEER);
    return 0;
}

result_t SecureContext::get_rejectUnverified(bool& retVal)
{
    retVal = SSL_CTX_get_verify_callback(m_ctx) == nullptr;
    return 0;
}

result_t SecureContext::get_rejectUnauthorized(bool& retVal)
{
    retVal = !!(SSL_CTX_get_verify_mode(m_ctx) & SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
    return 0;
}

result_t SecureContext::set_sessionTimeout(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    result_t hr;
    int32_t timeout;

    hr = GetConfigValue(isolate, options, "sessionTimeout", timeout);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return Runtime::setError("SecureContext: sessionTimeout must be a valid number.");

    if (hr != CALL_E_PARAMNOTOPTIONAL)
        if (SSL_CTX_set_timeout(m_ctx, timeout) <= 0)
            return openssl_error();

    return 0;
}

result_t SecureContext::get_sessionTimeout(int32_t& retVal)
{
    retVal = SSL_CTX_get_timeout(m_ctx);
    return 0;
}

}
