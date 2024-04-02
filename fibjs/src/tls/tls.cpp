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
#include "TLSSocket.h"
#include "Url.h"
#include "options.h"
#include "openssl/provider.h"

namespace fibjs {

DECLARE_MODULE(tls);
DECLARE_MODULE_EX(ssl, tls);

void init_tls()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    OSSL_PROVIDER_load(NULL, "default");
    if (g_openssl_legacy_provider)
        OSSL_PROVIDER_load(nullptr, "legacy");
}

result_t tls_base::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    retVal = isolate->m_ctx;
    return 0;
}

result_t tls_base::connect(exlib::string url, SecureContext_base* secureContext, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    class asyncConnect : public AsyncState {
    public:
        asyncConnect(const exlib::string host, int32_t port, bool ipv6, SecureContext_base* ctx,
            int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_host(host)
            , m_port(port)
            , m_ipv6(ipv6)
            , m_ctx(ctx)
            , m_timeout(timeout)
            , m_retVal(retVal)
        {
            next(connect);
        }

        ON_STATE(asyncConnect, connect)
        {
            Socket_base::_new(m_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET, m_sock);
            return m_sock->connect(m_host, m_port, m_timeout, next(handshake));
        }

        ON_STATE(asyncConnect, handshake)
        {
            m_ssl_sock = new TLSSocket();
            m_ssl_sock->init(m_ctx);
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
        obj_ptr<SecureContext_base> m_ctx;
        int32_t m_timeout;
        obj_ptr<Stream_base>& m_retVal;
        obj_ptr<Socket_base> m_sock;
        obj_ptr<TLSSocket> m_ssl_sock;
    };

    if (qstrcmp(url.c_str(), "ssl:", 4))
        return CHECK_ERROR(CALL_E_INVALIDARG);

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    obj_ptr<Url> u = new Url();

    result_t hr = u->parse(url);
    if (hr < 0)
        return hr;

    if (u->m_port.length() == 0)
        return CHECK_ERROR(CALL_E_INVALIDARG);

    int32_t nPort = atoi(u->m_port.c_str());

    return (new asyncConnect(u->m_hostname, nPort, u->m_ipv6, secureContext, timeout, retVal, ac))
        ->post(0);
}

result_t tls_base::connect(exlib::string url, int32_t timeout, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    Isolate* isolate = Isolate::current();
    return connect(url, isolate->m_ctx, timeout, retVal, ac);
}

result_t tls_base::connect(exlib::string url, v8::Local<v8::Object> optionns, obj_ptr<Stream_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = ac->isolate();

        ac->m_ctx.resize(2);

        obj_ptr<SecureContext_base> ctx;
        result_t hr = createSecureContext(optionns, false, ctx);
        if (hr < 0)
            return hr;

        ac->m_ctx[0] = ctx;

        int32_t timeout = 0;
        hr = GetConfigValue(isolate, optionns, "timeout", timeout);
        if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
            return hr;

        ac->m_ctx[1] = timeout;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<SecureContext_base> ctx = (SecureContext_base*)ac->m_ctx[0].object();
    int32_t timeout = ac->m_ctx[1].intVal();
    return connect(url, ctx, timeout, retVal, ac);
}

}
