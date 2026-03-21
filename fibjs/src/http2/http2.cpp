/*
 * http2.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#include "object.h"
#include "ifs/http2.h"
#include "Http2Server.h"
#include "Http2Session.h"
#include "ifs/tls.h"
#include "ifs/net.h"
#include "ifs/url.h"
#include "TLSSocket.h"
#include "Url.h"
#include "Socket.h"

namespace fibjs {

DECLARE_MODULE(http2);

result_t http2_base::createServer(v8::Local<v8::Object> options, Handler_base* hdlr,
    obj_ptr<Http2Server_base>& retVal)
{
    obj_ptr<SecureContext_base> ctx;
    result_t hr = tls_base::createSecureContext(options, true, ctx);
    if (hr < 0)
        return hr;

    return createServer(ctx, hdlr, retVal);
}

result_t http2_base::createServer(SecureContext_base* context, Handler_base* hdlr,
    obj_ptr<Http2Server_base>& retVal)
{
    obj_ptr<Http2Server> svr = new Http2Server();
    svr->wrap();

    result_t hr = svr->create(context, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;
    return 0;
}

result_t http2_base::connect(exlib::string authority, v8::Local<v8::Object> options,
    obj_ptr<Http2Session_base>& retVal, AsyncEvent* ac)
{
    if (ac->isSync()) {
        Isolate* isolate = ac->isolate();
        v8::Local<v8::Context> context = isolate->context();

        // Parse URL
        obj_ptr<Url> u = new Url();
        result_t hr = u->parse(authority);
        if (hr < 0)
            return hr;

        exlib::string proto = u->protocol();
        if (proto != "https:")
            return CHECK_ERROR(Runtime::setError("http2.connect: authority must use https:// scheme."));

        exlib::string hostname = u->hostname();
        exlib::string port_str = u->port();
        int32_t port = port_str.empty() ? 443 : atoi(port_str.c_str());

        // Ensure alpnProtocols includes 'h2'
        v8::Local<v8::Value> alpnVal;
        hr = GetConfigValue(options, "alpnProtocols", alpnVal);
        if (hr == CALL_E_PARAMNOTOPTIONAL || alpnVal.IsEmpty()) {
            v8::Local<v8::Array> alpn = v8::Array::New(isolate->m_isolate, 1);
            alpn->Set(context, 0, isolate->NewString("h2")).IsJust();
            options->Set(context, isolate->NewString("alpnProtocols"), alpn).IsJust();
        }

        // Create SecureContext from options
        obj_ptr<SecureContext_base> ctx;
        hr = tls_base::createSecureContext(options, false, ctx);
        if (hr < 0)
            return hr;

        // Create TLS socket
        obj_ptr<TLSSocket> ssl_sock = new TLSSocket();
        ssl_sock->init(ctx);

        // Create session on V8 thread where wrap() is safe
        obj_ptr<Http2Session> session = new Http2Session(false);
        session->wrap();
        session->m_scheme = "https";
        session->m_authority = hostname;
        if (port != 443)
            session->m_authority.append(1, ':').append(port_str);

        ac->m_ctx.resize(5);
        ac->m_ctx[0] = ssl_sock;
        ac->m_ctx[1] = hostname;
        ac->m_ctx[2] = port;
        ac->m_ctx[3] = u->isIPv6();
        ac->m_ctx[4] = session;

        return CHECK_ERROR(CALL_E_NOSYNC);
    }

    obj_ptr<TLSSocket> ssl_sock = (TLSSocket*)(object_base*)ac->m_ctx[0].object();
    exlib::string hostname = ac->m_ctx[1].string();
    int32_t port = ac->m_ctx[2].intVal();
    bool ipv6 = ac->m_ctx[3].boolVal();
    obj_ptr<Http2Session> session = (Http2Session*)(object_base*)ac->m_ctx[4].object();

    class asyncConnect : public AsyncState {
    public:
        asyncConnect(TLSSocket* ssl_sock, const exlib::string& host, int32_t port,
            bool ipv6, Http2Session* session, obj_ptr<Http2Session_base>& retVal, AsyncEvent* ac)
            : AsyncState(ac)
            , m_ssl_sock(ssl_sock)
            , m_host(host)
            , m_port(port)
            , m_ipv6(ipv6)
            , m_session(session)
            , m_retVal(retVal)
        {
            next(tcp_connect);
        }

    public:
        ON_STATE(asyncConnect, tcp_connect)
        {
            Socket_base::_new(m_ipv6 ? net_base::C_AF_INET6 : net_base::C_AF_INET, m_sock);
            return m_sock->connect(m_port, m_host, 0,
                reinterpret_cast<obj_ptr<Stream_base>&>(m_sock), next(tls_handshake));
        }

        ON_STATE(asyncConnect, tls_handshake)
        {
            return m_ssl_sock->connect(m_sock, m_host, next(init_session));
        }

        ON_STATE(asyncConnect, init_session)
        {
            result_t hr = m_session->init(m_ssl_sock);
            if (hr < 0)
                return hr;

            // Collect pending SETTINGS frame for async write
            hr = m_session->collectPendingData(m_pending_buf);
            if (hr < 0)
                return hr;

            if (m_pending_buf)
                return m_ssl_sock->writeBuffer(m_pending_buf, next(send_settings));

            m_session->startLoops();
            m_retVal = m_session;
            return next(0);
        }

        ON_STATE(asyncConnect, send_settings)
        {
            m_pending_buf.Release();
            m_session->startLoops();
            m_retVal = m_session;
            return next(0);
        }

    private:
        obj_ptr<TLSSocket> m_ssl_sock;
        exlib::string m_host;
        int32_t m_port;
        bool m_ipv6;
        obj_ptr<Http2Session> m_session;
        obj_ptr<Http2Session_base>& m_retVal;
        obj_ptr<Socket_base> m_sock;
        obj_ptr<Buffer_base> m_pending_buf;
    };

    return (new asyncConnect(ssl_sock, hostname, port, ipv6, session, retVal, ac))->post(0);
}

result_t http2_base::getDefaultSettings(v8::Local<v8::Object>& retVal)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Context> context = isolate->context();
    v8::Local<v8::Object> obj = v8::Object::New(isolate->m_isolate);

    obj->Set(context, isolate->NewString("headerTableSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate, 4096))
        .IsJust();
    obj->Set(context, isolate->NewString("enablePush"),
        v8::Boolean::New(isolate->m_isolate, true))
        .IsJust();
    obj->Set(context, isolate->NewString("maxConcurrentStreams"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate, 100))
        .IsJust();
    obj->Set(context, isolate->NewString("initialWindowSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate, 65535))
        .IsJust();
    obj->Set(context, isolate->NewString("maxFrameSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate, 16384))
        .IsJust();
    obj->Set(context, isolate->NewString("maxHeaderListSize"),
        v8::Integer::NewFromUnsigned(isolate->m_isolate, 65535))
        .IsJust();

    retVal = obj;
    return 0;
}

} /* namespace fibjs */
