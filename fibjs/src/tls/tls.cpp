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
#include "union_helpers.h"

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
        int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
        : AsyncState(ac)
        , m_host(host)
        , m_port(port)
        , m_ipv6(ipv6)
        , m_ssl_sock(ssl_sock)
        , m_timeout(timeout)
        , m_retVal(retVal)
    {
        init(connect);
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

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(url, secureContext, timeout, v8::Local<v8::Function>(), retVal, std::move(ac));
}

result_t tls_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    Isolate* isolate = ac.isolate();
    return connect(url, isolate->m_ctx, timeout, v8::Local<v8::Function>(), retVal, std::move(ac));
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(url, options, v8::Local<v8::Function>(), retVal, std::move(ac));
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(port, host, options, v8::Local<v8::Function>(), retVal, std::move(ac));
}

result_t tls_base::connect(v8::Local<v8::Object> options, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(options, v8::Local<v8::Function>(), retVal, std::move(ac));
}

// the three forms of the merged arity-2 entry: the url, the port and the
// options object, each with a once connect listener
static result_t connect_by_options(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener,
    obj_ptr<Stream_base>& retVal, AsyncHandle ac);

static result_t connect_by_url(exlib::string url, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    Isolate* isolate = ac.isolate();
    return tls_base::connect(url, isolate->m_ctx, 0, connectListener, retVal, std::move(ac));
}

result_t tls_base::connect(exlib::string url, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    Isolate* isolate = ac.isolate();
    return connect(url, isolate->m_ctx, timeout, connectListener, retVal, std::move(ac));
}

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(url, secureContext, 0, connectListener, retVal, std::move(ac));
}

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, int32_t timeout, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (qstrcmp(url.c_str(), "ssl:", 4))
        return CHECK_ERROR(Runtime::setError(CALL_E_INVALIDARG, "tls.connect: url must start with 'ssl:', got '%s'.", url.c_str()));

    if (ac.isSync()) {
        // only the listener registration needs the sync phase; without one the
        // socket is built in the async phase (no `_new` off the JS thread,
        // plans/async-phase-discipline-audit-2026-10-05.md §3-F16)
        if (!connectListener.IsEmpty()) {
            ac.ctxv().resize(1);

            obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
            ssl_sock->init(secureContext);

            v8::Local<v8::Object> _retVal;
            ssl_sock->once(ssl_sock->holder()->NewString("connect"), connectListener, _retVal);

            ac.ctxv()[0] = ssl_sock;
        }

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
    if (ac.ctxv().size() == 0) {
        ssl_sock = new TLSSocket();
        ssl_sock->init(secureContext);
    } else
        ssl_sock = (TLSSocket*)ac.ctxv()[0].object();

    return (new asyncConnect(u->hostname(), nPort, u->isIPv6(), ssl_sock, timeout, retVal, std::move(ac)))
        ->post(0);
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

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

        ac.ctxv()[0] = ssl_sock;

        int32_t timeout = 0;
        hr = GetConfigValue(options, "timeout", timeout);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        ac.ctxv()[1] = timeout;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    if (ac.ctxv().size() < 2) {
        // no sync phase (cc_ from the port form): the default context, no timeout
        // hoisted: the call below moves the handle, and the argument expressions
        // are only indeterminately sequenced with that move
        obj_ptr<SecureContext_base> ctxo = ac.isolate()->m_ctx;

        return connect(url, ctxo, 0, connectListener, retVal, std::move(ac));
    }

    int32_t timeout = ac.ctxv()[1].intVal();
    return connect(url, nullptr, timeout, connectListener, retVal, std::move(ac));
}

static result_t connect_by_port(int32_t port, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return tls_base::connect(port, "localhost", connectListener, retVal, std::move(ac));
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    v8::Local<v8::Object> options;
    if (ac.isSync())
        options = v8::Object::New(ac.isolate()->m_isolate);

    return connect(port, host, options, connectListener, retVal, std::move(ac));
}

result_t tls_base::connect(int32_t port, v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    return connect(port, "localhost", options, connectListener, retVal, std::move(ac));
}

result_t tls_base::connect(int32_t port, exlib::string host, v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        ac.ctxv().resize(2);

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

        ac.ctxv()[0] = ssl_sock;

        int32_t timeout = 0;
        hr = GetConfigValue(options, "timeout", timeout);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        ac.ctxv()[1] = timeout;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<TLSSocket> ssl_sock;
    int32_t timeout = 0;

    if (ac.ctxv().size() > 1) {
        ssl_sock = (TLSSocket*)ac.ctxv()[0].object();
        timeout = ac.ctxv()[1].intVal();
    } else {
        // no sync phase (cc_ from connect_by_port): the default context
        ssl_sock = new TLSSocket();
        ssl_sock->init(ac.isolate()->m_ctx);
    }

    return (new asyncConnect(host, port, Url::isIPv6(host), ssl_sock, timeout, retVal, std::move(ac)))
        ->post(0);
}

static result_t connect_by_options(v8::Local<v8::Object> options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (ac.isSync()) {
        Isolate* isolate = Isolate::current(options);
        ac.ctxv().resize(2);

        obj_ptr<SecureContext_base> ctx;
        result_t hr = tls_base::createSecureContext(options, false, ctx);
        if (hr < 0)
            return hr;

        obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
        ssl_sock->init(ctx);

        if (!connectListener.IsEmpty()) {
            v8::Local<v8::Object> _retVal;
            ssl_sock->once(ssl_sock->holder()->NewString("connect"), connectListener, _retVal);
        }

        ac.ctxv()[0] = ssl_sock;

        obj_ptr<ConnectOptions> opts;
        hr = ConnectOptions::load(options, opts);
        if (hr < 0)
            return hr;

        ac.ctxv()[1] = opts;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    result_t ctx_hr = ac.ctx(0);
    if (ctx_hr < 0)
        return ctx_hr;
    ctx_hr = ac.ctx(1);
    if (ctx_hr < 0)
        return ctx_hr;

    TLSSocket_base* ssl_sock = (TLSSocket_base*)ac.ctxv()[0].object();
    ConnectOptions* opts = (ConnectOptions*)ac.ctxv()[1].object();
    exlib::string host = opts->host.value();
    int32_t port = opts->port.value();
    int32_t timeout = opts->timeout.value();
    return (new asyncConnect(host, port, Url::isIPv6(host), ssl_sock, timeout, retVal, std::move(ac)))
        ->post(0);
}

result_t tls_base::connect(Union_connect_options options, v8::Local<v8::Function> connectListener, obj_ptr<Stream_base>& retVal, AsyncHandle ac)
{
    if (std::holds_alternative<v8::Local<v8::Object>>(options))
        return connect_by_options(std::get<v8::Local<v8::Object>>(options), connectListener, retVal, std::move(ac));

    if (std::holds_alternative<exlib::string>(options))
        return connect_by_url(std::get<exlib::string>(options), connectListener, retVal, std::move(ac));

    return connect_by_port(std::get<int32_t>(options), connectListener, retVal, std::move(ac));
}

result_t tls_base::createServer(Union_createServer_options options, Union_createServer_listener listener,
    obj_ptr<TLSServer_base>& retVal)
{
    obj_ptr<Handler_base> handler;
    result_t hr = handler_from_union(listener, handler);
    if (hr < 0)
        return hr;

    if (std::holds_alternative<obj_ptr<SecureContext_base>>(options))
        return TLSServer_base::_new(std::get<obj_ptr<SecureContext_base>>(options).get(), handler, retVal);

    obj_ptr<SecureContext_base> ctx;
    hr = tls_base::createSecureContext(std::get<v8::Local<v8::Object>>(options), true, ctx);
    if (hr < 0)
        return hr;
    return TLSServer_base::_new(ctx, handler, retVal);
}
}
