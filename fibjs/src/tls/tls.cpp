/*
 * tls.cpp
 *
 *  Created on: Mar 1, 2024
 *      Author: lion
 */

#include "crypto_util.h"
#include "ifs/tls.h"
#include "ifs/crypto.h"
#include "ifs/Socket.h"
#include "ifs/TLSServer.h"
#include "Socket.h"
#include "TLSSocket.h"
#include "Url.h"
#include "options.h"
#include "openssl/provider.h"

namespace fibjs {

DECLARE_MODULE(tls);
DECLARE_MODULE_EX(ssl, tls);

void init_blst_eng();

void init_tls()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    OSSL_PROVIDER_load(NULL, "default");
    if (g_openssl_legacy_provider)
        OSSL_PROVIDER_load(nullptr, "legacy");

    init_blst_eng();
}

result_t tls_base::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->m_ctx;
    return 0;
}

class asyncConnect : public AsyncState {
public:
    asyncConnect(const exlib::string host, int32_t port, bool ipv6, TLSSocket_base* ssl_sock,
        int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
        : AsyncState(ac)
        , m_host(host)
        , m_port(port)
        , m_ipv6(ipv6)
        , m_ssl_sock(ssl_sock)
        , m_timeout(timeout)
        , m_retVal(retVal)
    {
        next(connect);
    }

    ON_STATE(asyncConnect, connect)
    {
        Socket_base::_new(m_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET, m_sock);
        return m_sock->connect(m_port, m_host, m_timeout,
            reinterpret_cast<obj_ptr<Stream_base>&>(m_sock), next(handshake));
    }

    ON_STATE(asyncConnect, handshake)
    {
        return m_ssl_sock->connect(m_sock, m_host, next(ok));
    }

    ON_STATE(asyncConnect, ok)
    {
        m_retVal = m_ssl_sock;
        return next();
    }

private:
    const exlib::string m_host;
    int32_t m_port;
    bool m_ipv6;
    obj_ptr<TLSSocket_base> m_ssl_sock;
    int32_t m_timeout;
    obj_ptr<Stream_base>& m_retVal;
    obj_ptr<Socket_base> m_sock;
};

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(url, secureContext, timeout, v8::Local<v8::Function>(), retVal, ac);
}

result_t tls_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    Isolate* isolate = ac->isolate();
    return connect(url, isolate->m_ctx, timeout, v8::Local<v8::Function>(), retVal, ac);
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(url, options, v8::Local<v8::Function>(), retVal, ac);
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, host, options, v8::Local<v8::Function>(), retVal, ac);
}

result_t tls_base::connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(options, v8::Local<v8::Function>(), retVal, ac);
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    Isolate* isolate = ac->isolate();
    return connect(url, isolate->m_ctx, 0, connectListener, retVal, ac);
}

result_t tls_base::connect(exlib::string url, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    Isolate* isolate = ac->isolate();
    return connect(url, isolate->m_ctx, timeout, connectListener, retVal, ac);
}

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(url, secureContext, 0, connectListener, retVal, ac);
}

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (qstrcmp(url.c_str(), "ssl:", 4))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "tls.connect: url must start with 'ssl:', got '%s'.", url.c_str()));

    if (ac->isSync()) {
        ac->m_ctx.resize(1);

        obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
        ssl_sock->init(secureContext);

        if (!connectListener.IsEmpty()) {
            v8::Local<v8::Object> _retVal;
            ssl_sock->once(ssl_sock->holder()->NewString("connect"), connectListener, _retVal);
        }

        ac->m_ctx[0] = ssl_sock;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    exlib::string port = u->port();
    if (port.length() == 0)
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "tls.connect: missing port in url '%s'.", url.c_str()));

    int32_t nPort = atoi(port.c_str());
    obj_ptr<TLSSocket> ssl_sock;
    if (ac->m_ctx.size() == 0) {
        ssl_sock = new TLSSocket();
        ssl_sock->init(secureContext);
    } else
        ssl_sock = (TLSSocket*)ac->m_ctx[0].object();

    return (new asyncConnect(u->hostname(), nPort, u->isIPv6(), ssl_sock, timeout, retVal, ac))
        ->post(0);
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        obj_ptr<SecureContext_base> ctx;
        result_t hr = createSecureContext(options, false, ctx);
        if (hr < 0)
            return hr;

        obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
        ssl_sock->init(ctx);

        if (!connectListener.IsEmpty()) {
            v8::Local<v8::Object> _retVal;
            ssl_sock->once(ssl_sock->holder()->NewString("connect"), connectListener, _retVal);
        }

        ac->m_ctx[0] = ssl_sock;

        int32_t timeout = 0;
        hr = GetConfigValue(options, "timeout", timeout);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        ac->m_ctx[1] = timeout;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    int32_t timeout = ac->m_ctx[1].intVal();
    return connect(url, nullptr, timeout, connectListener, retVal, ac);
}

result_t tls_base::connect(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, "localhost", connectListener, retVal, ac);
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    v8::Local<v8::Object> options;
    if (ac->isSync())
        options = v8::Object::New(ac->isolate()->m_isolate);

    return connect(port, host, options, connectListener, retVal, ac);
}

result_t tls_base::connect(int32_t port, v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    return connect(port, "localhost", options, connectListener, retVal, ac);
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        ac->m_ctx.resize(2);

        obj_ptr<SecureContext_base> ctx;
        result_t hr = createSecureContext(options, false, ctx);
        if (hr < 0)
            return hr;

        obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
        ssl_sock->init(ctx);

        if (!connectListener.IsEmpty()) {
            v8::Local<v8::Object> _retVal;
            ssl_sock->once(ssl_sock->holder()->NewString("connect"), connectListener, _retVal);
        }

        ac->m_ctx[0] = ssl_sock;

        int32_t timeout = 0;
        hr = GetConfigValue(options, "timeout", timeout);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        ac->m_ctx[1] = timeout;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    TLSSocket_base* ssl_sock = (TLSSocket_base*)ac->m_ctx[0].object();
    int32_t timeout = ac->m_ctx[1].intVal();
    return (new asyncConnect(host, port, Url::isIPv6(host), ssl_sock, timeout, retVal, ac))
        ->post(0);
}

result_t tls_base::connect(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = Isolate::current(options);
        ac->m_ctx.resize(2);

        obj_ptr<SecureContext_base> ctx;
        result_t hr = createSecureContext(options, false, ctx);
        if (hr < 0)
            return hr;

        obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
        ssl_sock->init(ctx);

        if (!connectListener.IsEmpty()) {
            v8::Local<v8::Object> _retVal;
            ssl_sock->once(ssl_sock->holder()->NewString("connect"), connectListener, _retVal);
        }

        ac->m_ctx[0] = ssl_sock;

        obj_ptr<ConnectOptions> opts;
        hr = ConnectOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac->m_ctx[1] = opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    TLSSocket_base* ssl_sock = (TLSSocket_base*)ac->m_ctx[0].object();
    ConnectOptions* opts = (ConnectOptions*)ac->m_ctx[1].object();
    exlib::string host = opts->host.value();
    int32_t port = opts->port.value();
    int32_t timeout = opts->timeout.value();
    return (new asyncConnect(host, port, Url::isIPv6(host), ssl_sock, timeout, retVal, ac))
        ->post(0);
}

result_t tls_base::createServer(SecureContext_base* context, Handler_base* listener,
    obj_ptr<TLSServer_base>& retVal)
{
    return TLSServer_base::_new(context, listener, retVal);
}

result_t tls_base::createServer(v8::Local<v8::Object> options, Handler_base* listener,
    obj_ptr<TLSServer_base>& retVal)
{
    obj_ptr<SecureContext_base> ctx;
    result_t hr = tls_base::createSecureContext(options, true, ctx);
    if (hr < 0)
        return hr;
    return TLSServer_base::_new(ctx, listener, retVal);
}
}
