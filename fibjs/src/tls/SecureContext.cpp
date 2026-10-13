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
#include "Fiber.h"

namespace fibjs {

static result_t load_cert_option(v8::Local<v8::Object> options, const char* name, obj_ptr<X509Certificate_base>& retVal)
{
    Isolate* isolate = Isolate::current(options);
    v8::Local<v8::Context> context = isolate->context();
    result_t hr = GetConfigValue(options, name, retVal);

    if (hr.ok())
        return result_t();

    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL) && !hr.is_type_mismatch())
        return result_t::make(CALL_E_EXCEPTION).with_message(exlib::string("SecureContext: ") + name + " must be a valid X509Certificate, Buffer, or Buffer array.");

    if (!js_obj_has_value(options, context, name))
        return result_t(CALL_E_PARAMNOTOPTIONAL);

    v8::Local<v8::Value> value = options->Get(context, isolate->NewString(name)).FromMaybe(v8::Local<v8::Value>());
    hr = X509Certificate_base::load(value, retVal);
    if (hr.failed())
        return result_t::make(CALL_E_EXCEPTION).with_message(exlib::string("SecureContext: ") + name + " must be a valid X509Certificate, Buffer, or Buffer array.");

    return result_t();
}

result_t tls_base::createSecureContext(v8::Local<v8::Object> options, bool isServer, obj_ptr<SecureContext_base>& retVal)
{
    result_t hr;

    hr = GetConfigValue(options, "secureContext", retVal, true);
    if (!hr.is_param_missing())
        return hr;

    obj_ptr<SecureContext> ctx = new SecureContext();
    hr = ctx->init(options, isServer);
    if (hr.failed())
        return hr;

    retVal = ctx;

    return result_t();
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
    if (hr.failed())
        return hr;

    hr = set_ca(options, isServer);
    if (hr.failed())
        return hr;

    hr = set_cert(options);
    if (hr.failed())
        return hr;

    hr = set_key(options);
    if (hr.failed())
        return hr;

    hr = set_verify(options, isServer);
    if (hr.failed())
        return hr;

    hr = set_sessionTimeout(options);
    if (hr.failed())
        return hr;

    hr = set_alpnProtocols(options, isServer);
    if (hr.failed())
        return hr;

    if (isServer) {
        hr = set_sn_callback(options);
        if (hr.failed())
            return hr;
    } else
        m_sniContexts.resize(0);

    return result_t();
}

void SecureContext::init_ctx(const SSL_METHOD* method)
{
    m_ctx = SSL_CTX_new(method);

    SSL_CTX_set_options(m_ctx, SSL_OP_NO_SSLv2);
    SSL_CTX_set_options(m_ctx, SSL_OP_NO_SSLv3);
    SSL_CTX_set_options(m_ctx, SSL_OP_ALLOW_CLIENT_RENEGOTIATION);
    SSL_CTX_clear_mode(m_ctx, SSL_MODE_NO_AUTO_CHAIN);

    static std::atomic<uint32_t> s_sid_ctx = 1;
    uint32_t sid_ctx = s_sid_ctx++;

    SSL_CTX_set_session_id_context(m_ctx, (const unsigned char*)&sid_ctx, sizeof(sid_ctx));

    SSL_CTX_set_session_cache_mode(m_ctx,
        SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_SERVER | SSL_SESS_CACHE_NO_INTERNAL | SSL_SESS_CACHE_NO_AUTO_CLEAR);
}

result_t SecureContext::set_ca(v8::Local<v8::Object> options, bool isServer)
{
    Isolate* isolate = holder();
    obj_ptr<X509Certificate_base> ca;
    result_t hr = load_cert_option(options, "ca", ca);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    if (!hr.is_param_missing())
        return set_ca(ca);
    else if (!isServer)
        return set_ca(isolate->m_ctx.As<SecureContext>()->m_ca);

    return result_t();
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

    return result_t();
}

result_t SecureContext::SetRootCerts()
{
    init_ctx(TLS_client_method());

    obj_ptr<X509Certificate> root = new X509Certificate();

    root->LoadRootCerts();
    set_ca(root);

    SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);

    return result_t();
}

result_t SecureContext::get_ca(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_ca)
        return result_t::return_undefined();

    retVal = m_ca;

    return result_t();
}

result_t SecureContext::set_key(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;
    obj_ptr<KeyObject_base> key;

    {
        // Use js_obj_has_value instead of Has to skip undefined/null values
        if (js_obj_has_value(options, context, "key")) {
            hr = crypto_base::createPrivateKey(options, key);
            if (hr.failed())
                return hr;
        }
    }

    if (key) {
        if (!m_cert)
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: cert must be set before key.");

        bool matched;
        hr = m_cert->checkPrivateKey(key, matched);
        if (hr.failed())
            return hr;

        if (!matched)
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: key and cert do not match.");

        SSL_CTX_use_PrivateKey(m_ctx, key.As<KeyObject>()->pkey());

        m_key = key;
    } else if (m_cert)
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: key must be set before cert.");

    return result_t();
}

result_t SecureContext::get_key(obj_ptr<KeyObject_base>& retVal)
{
    if (!m_key)
        return result_t::return_undefined();

    retVal = m_key;

    return result_t();
}

result_t SecureContext::set_cert(v8::Local<v8::Object> options)
{
    obj_ptr<X509Certificate_base> certs;

    result_t hr = load_cert_option(options, "cert", certs);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return hr;

    if (!hr.is_param_missing()) {
        X509Certificate* now = certs.As<X509Certificate>();

        SSL_CTX_use_certificate(m_ctx, now->cert());
        now = now->next();

        while (now) {
            SSL_CTX_add1_chain_cert(m_ctx, now->cert());
            now = now->next();
        }

        m_cert = certs;
    }

    return result_t();
}

result_t SecureContext::get_cert(obj_ptr<X509Certificate_base>& retVal)
{
    if (!m_cert)
        return result_t::return_undefined();

    retVal = m_cert;

    return result_t();
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
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: TLS version \"" + ver + "\" is not supported.");

    return result_t();
}

inline result_t ver_string(int32_t ver, exlib::string& retVal)
{
    switch (ver) {
    case SSL2_VERSION:
        retVal = "SSLv2";
        return result_t();
    case SSL3_VERSION:
        retVal = "SSLv3";
        return result_t();
    case TLS1_VERSION:
        retVal = "TLSv1";
        return result_t();
    case TLS1_1_VERSION:
        retVal = "TLSv1.1";
        return result_t();
    case TLS1_2_VERSION:
        retVal = "TLSv1.2";
        return result_t();
    case TLS1_3_VERSION:
        retVal = "TLSv1.3";
        return result_t();
    }

    return result_t::return_undefined();
}

result_t SecureContext::set_secureProtocol(v8::Local<v8::Object> options, bool isServer)
{
    result_t hr;
    exlib::string ver;
    int32_t maxVersion = 0;
    int32_t minVersion = 0;

    hr = GetConfigValue(options, "maxVersion", ver, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: maxVersion must be a valid string.");

    if (!hr.is_param_missing()) {
        hr = ver_id(ver, maxVersion);
        if (hr.failed())
            return hr;
    }

    hr = GetConfigValue(options, "minVersion", ver, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: minVersion must be a valid string.");

    if (!hr.is_param_missing()) {
        hr = ver_id(ver, minVersion);
        if (hr.failed())
            return hr;
    }

    const SSL_METHOD* method = nullptr;
    exlib::string secureProtocol;

    hr = GetConfigValue(options, "secureProtocol", secureProtocol, true);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: secureProtocol must be a valid string.");

    if (hr.is_param_missing() || secureProtocol == "TLS_method") {
        method = TLS_method();
    } else if (secureProtocol == "TLS_client_method") {
        method = TLS_client_method();
    } else if (secureProtocol == "TLS_server_method") {
        method = TLS_server_method();
    } else {
        if (minVersion != 0 || maxVersion != 0)
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: secureProtocol \" + secureProtocol + \" does not support minVersion or maxVersion.");

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
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: secureProtocol \"" + secureProtocol + "\" is not supported.");
    }

    if (isServer && method == TLS_client_method())
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: secureProtocol \"" + secureProtocol + "\" is not supported for server.");

    init_ctx(method);

    if (minVersion)
        SSL_CTX_set_min_proto_version(m_ctx, minVersion);
    if (maxVersion)
        SSL_CTX_set_max_proto_version(m_ctx, maxVersion);

    return result_t();
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
        return result_t::return_undefined();

    return result_t();
}

result_t SecureContext::set_verify(v8::Local<v8::Object> options, bool isServer)
{
    result_t hr;

    int32_t verify_mode;
    bool requestCert = true;
    bool rejectUnverified = true;
    bool rejectUnauthorized = !isServer;

    hr = GetConfigValue(options, "requestCert", requestCert);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: requestCert must be a valid boolean.");

    hr = GetConfigValue(options, "rejectUnverified", rejectUnverified);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: rejectUnverified must be a valid boolean.");

    hr = GetConfigValue(options, "rejectUnauthorized", rejectUnauthorized);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: rejectUnauthorized must be a valid boolean.");

    if (!requestCert)
        verify_mode = SSL_VERIFY_NONE;
    else {
        verify_mode = SSL_VERIFY_PEER | SSL_VERIFY_CLIENT_ONCE;
        if (rejectUnauthorized)
            verify_mode |= SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
    }

    if (rejectUnverified)
        SSL_CTX_set_verify(m_ctx, verify_mode, nullptr);
    else
        SSL_CTX_set_verify(m_ctx, verify_mode, [](int ok, X509_STORE_CTX* ctx) -> int {
            return 1;
        });

    return result_t();
}

int SecureContext::sn_callback(SSL* ssl, int* ad)
{
    const char* servername = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);
    if (servername) {
        obj_ptr<SecureContext> ctx;

        if (!m_sniContexts.lookup(servername, ctx))
            return SSL_TLSEXT_ERR_NOACK;

        SSL_set_SSL_CTX(ssl, ctx->m_ctx);
        return SSL_TLSEXT_ERR_OK;
    }

    return SSL_TLSEXT_ERR_NOACK;
}

int SecureContext::s_sn_callback(SSL* ssl, int* ad, void* arg)
{
    return ((SecureContext*)arg)->sn_callback(ssl, ad);
}

result_t SecureContext::set_sn_callback(v8::Local<v8::Object> options)
{
    Isolate* isolate = holder();
    result_t hr;

    int32_t SNICacheSize;
    hr = GetConfigValue(options, "SNICacheSize", SNICacheSize);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: SNICacheSize must be a valid number.");
    if (!hr.is_param_missing())
        m_sniContexts.resize(SNICacheSize);

    int64_t SNICacheIdleTimeout = 300;
    hr = GetConfigValue(options, "SNICacheIdleTimeout", SNICacheIdleTimeout);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: SNICacheIdleTimeout must be a valid number.");

    int64_t SNICacheTimeout = 300;
    hr = GetConfigValue(options, "SNICacheTimeout", SNICacheTimeout);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: SNICacheTimeout must be a valid number.");

    m_sniContexts.set_timeout(SNICacheIdleTimeout, SNICacheTimeout);

    v8::Local<v8::Function> js_sn_resolver;
    hr = GetConfigValue(options, "SNIResolver", js_sn_resolver);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: SNIResolver must be a valid function.");

    if (!hr.is_param_missing()) {
        m_sn_callback.Reset(isolate->m_isolate, js_sn_resolver);
        m_sniContexts.set_resolver([this](exlib::string key, obj_ptr<SecureContext>& ctx) -> bool {
            struct Param {
                SecureContext* self;
                exlib::string key;
                obj_ptr<SecureContext_base> retVal;
                exlib::Event ready;
            } param = { this, key };

            holder()->sync([&param]() {
                JSFiber::EnterJsScope s;
                Isolate* isolate = param.self->holder();
                v8::Local<v8::Context> context = isolate->context();
                v8::Local<v8::Function> js_sn_resolver = param.self->m_sn_callback.Get(isolate->m_isolate);
                v8::Local<v8::Value> argv[] = { isolate->NewString(param.key) };

                v8::Local<v8::Value> retVal = js_sn_resolver->Call(context, param.self->wrap(), 1, argv).FromMaybe(v8::Local<v8::Value>());
                param.retVal = SecureContext_base::getInstance(retVal);

                param.ready.set();

                return result_t();
            });

            param.ready.wait();

            if (!param.retVal)
                return false;

            ctx = param.retVal.As<SecureContext>();
            return true;
        });
    }

    SSL_CTX_set_tlsext_servername_arg(m_ctx, this);
    SSL_CTX_set_tlsext_servername_callback(m_ctx, s_sn_callback);

    return result_t();
}

result_t SecureContext::get_requestCert(bool& retVal)
{
    retVal = !!(SSL_CTX_get_verify_mode(m_ctx) & SSL_VERIFY_PEER);
    return result_t();
}

result_t SecureContext::get_rejectUnverified(bool& retVal)
{
    retVal = SSL_CTX_get_verify_callback(m_ctx) == nullptr;
    return result_t();
}

result_t SecureContext::get_rejectUnauthorized(bool& retVal)
{
    retVal = !!(SSL_CTX_get_verify_mode(m_ctx) & SSL_VERIFY_FAIL_IF_NO_PEER_CERT);
    return result_t();
}

result_t SecureContext::set_sessionTimeout(v8::Local<v8::Object> options)
{
    result_t hr;
    int32_t timeout;

    hr = GetConfigValue(options, "sessionTimeout", timeout);
    if (hr.failed_other_than(CALL_E_PARAMNOTOPTIONAL))
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: sessionTimeout must be a valid number.");

    if (!hr.is_param_missing())
        if (SSL_CTX_set_timeout(m_ctx, timeout) <= 0)
            return openssl_error();

    return result_t();
}

result_t SecureContext::get_sessionTimeout(int32_t& retVal)
{
    retVal = SSL_CTX_get_timeout(m_ctx);
    return result_t();
}

result_t SecureContext::setSNIContext(exlib::string servername, SecureContext_base* context)
{
    m_sniContexts.set(servername, (SecureContext*)context);
    return result_t();
}

result_t SecureContext::setSNIContext(exlib::string servername, v8::Local<v8::Object> options)
{
    obj_ptr<SecureContext_base> ctx;
    result_t hr = tls_base::createSecureContext(options, true, ctx);
    if (hr.failed())
        return hr;

    return setSNIContext(servername, ctx);
}

result_t SecureContext::getSNIContext(exlib::string servername, bool auto_resolve,
    obj_ptr<SecureContext_base>& retVal, AsyncHandle ac)
{
    if (auto_resolve && ac.isSync())
        return result_t::nosync();

    obj_ptr<SecureContext> ctx;
    if (!m_sniContexts.lookup(servername, ctx, auto_resolve))
        return result_t::return_undefined();

    retVal = ctx;

    return result_t();
}

result_t SecureContext::removeSNIContext(exlib::string servername)
{
    m_sniContexts.erase(servername);
    return result_t();
}

result_t SecureContext::clearSNIContexts()
{
    m_sniContexts.clear();
    return result_t();
}

int SecureContext::alpn_select_callback(SSL* ssl, const unsigned char** out, unsigned char* outlen,
    const unsigned char* in, unsigned int inlen, void* arg)
{
    SecureContext* ctx = static_cast<SecureContext*>(arg);
    const std::vector<unsigned char>& alpn = ctx->m_alpnProtos;

    if (alpn.empty())
        return SSL_TLSEXT_ERR_NOACK;

    if (SSL_select_next_proto(const_cast<unsigned char**>(out), outlen,
            alpn.data(), alpn.size(), in, inlen)
        != OPENSSL_NPN_NEGOTIATED)
        return SSL_TLSEXT_ERR_NOACK;

    return SSL_TLSEXT_ERR_OK;
}

result_t SecureContext::set_alpnProtocols(v8::Local<v8::Object> options, bool isServer)
{
    Isolate* isolate = holder();
    v8::Local<v8::Context> context = isolate->context();
    result_t hr;

    v8::Local<v8::Value> val;
    hr = GetConfigValue(options, "alpnProtocols", val);
    if (hr.is_param_missing())
        return result_t();
    if (hr.failed())
        return hr;

    if (!val->IsArray())
        return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: alpnProtocols must be an array of strings.");

    v8::Local<v8::Array> arr = val.As<v8::Array>();
    uint32_t len = arr->Length();
    if (len == 0)
        return result_t();

    // Build wire-format ALPN protocol list: each protocol prefixed by its length byte
    m_alpnProtos.clear();
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> item = arr->Get(context, i).FromMaybe(v8::Local<v8::Value>());
        if (item.IsEmpty() || !item->IsString())
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: alpnProtocols must be an array of strings.");

        v8::String::Utf8Value proto(isolate->m_isolate, item);
        int proto_len = proto.length();
        if (proto_len == 0 || proto_len > 255)
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: ALPN protocol length must be 1-255.");

        m_alpnProtos.push_back(static_cast<unsigned char>(proto_len));
        m_alpnProtos.insert(m_alpnProtos.end(), *proto, *proto + proto_len);
    }

    if (isServer) {
        SSL_CTX_set_alpn_select_cb(m_ctx, alpn_select_callback, this);
    } else {
        if (SSL_CTX_set_alpn_protos(m_ctx, m_alpnProtos.data(), m_alpnProtos.size()) != 0)
            return result_t::make(CALL_E_EXCEPTION).with_message("SecureContext: failed to set ALPN protocols.");
    }

    return result_t();
}

}
