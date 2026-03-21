/*
 * Http2Server.cpp
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#include "object.h"
#include "Http2Server.h"
#include "Http2Session.h"
#include "ifs/mq.h"
#include "ifs/tls.h"
#include "TLSServer.h"

namespace fibjs {

// Http2Handler: accepts a TLS stream and creates an Http2Session
class Http2Handler : public Handler_base {
    FIBER_FREE();

public:
    Http2Handler(Handler_base* hdlr)
        : m_hdlr(hdlr)
    {
    }

public:
    virtual result_t isRouting(bool& retVal)
    {
        retVal = false;
        return 0;
    }

    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac)
    {
        class asyncInvoke : public AsyncState {
        public:
            asyncInvoke(Http2Handler* pThis, Stream_base* stm, AsyncEvent* ac)
                : AsyncState(ac)
                , m_pThis(pThis)
                , m_stm(stm)
            {
                m_session = new Http2Session(true);
                m_session->wrap();

                next(init_session);
            }

        public:
            ON_STATE(asyncInvoke, init_session)
            {
                result_t hr = m_session->init(m_stm);
                if (hr < 0)
                    return hr;

                // The session's readLoop runs asynchronously.
                // Wait for session to close before returning.
                m_session->m_close_event.wait();
                return next(CALL_RETURN_NULL);
            }

        private:
            obj_ptr<Http2Handler> m_pThis;
            obj_ptr<Stream_base> m_stm;
            obj_ptr<Http2Session> m_session;
        };

        if (ac->isSync())
            return CHECK_ERROR(CALL_E_NOSYNC);

        obj_ptr<Stream_base> stm = Stream_base::getInstance(v);
        if (stm == NULL)
            return CHECK_ERROR(CALL_E_BADVARTYPE);

        return (new asyncInvoke(this, stm, ac))->post(0);
    }

private:
    obj_ptr<Handler_base> m_hdlr;
};

// -- Http2Server_base constructors --

result_t Http2Server_base::_new(SecureContext_base* context, Handler_base* hdlr,
    obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<Http2Server> svr = new Http2Server();
    svr->wrap(This);

    result_t hr = svr->create(context, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;
    return 0;
}

result_t Http2Server_base::_new(SecureContext_base* context, int32_t port,
    Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(context, "", port, hdlr, retVal, This);
}

result_t Http2Server_base::_new(SecureContext_base* context, exlib::string addr,
    int32_t port, Handler_base* hdlr, obj_ptr<Http2Server_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Http2Server> svr = new Http2Server();
    svr->wrap(This);

    result_t hr = svr->create(context, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;
    return 0;
}

result_t Http2Server_base::_new(v8::Local<v8::Object> options, Handler_base* hdlr,
    obj_ptr<Http2Server_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<SecureContext_base> ctx;
    result_t hr = tls_base::createSecureContext(options, true, ctx);
    if (hr < 0)
        return hr;

    return _new(ctx, hdlr, retVal, This);
}

// -- Http2Server implementation --

result_t Http2Server::create(SecureContext_base* context, Handler_base* hdlr)
{
    result_t hr;

    m_ctx = context;
    m_handler = hdlr;

    obj_ptr<Http2Handler> h2handler = new Http2Handler(hdlr);

    obj_ptr<TLSServer_base> _server;
    hr = TLSServer_base::_new(context, h2handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", h2handler->wrap());
    SetPrivate("server", _server->wrap());
    m_server = _server;
    static_cast<TLSServer*>(_server.get())->set_event_delegate(this);

    return 0;
}

result_t Http2Server::create(SecureContext_base* context, exlib::string addr,
    int32_t port, Handler_base* hdlr)
{
    result_t hr;

    m_ctx = context;
    m_handler = hdlr;

    obj_ptr<Http2Handler> h2handler = new Http2Handler(hdlr);

    obj_ptr<TLSServer_base> _server;
    hr = TLSServer_base::_new(context, addr, port, h2handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", h2handler->wrap());
    SetPrivate("server", _server->wrap());
    m_server = _server;
    static_cast<TLSServer*>(_server.get())->set_event_delegate(this);

    return 0;
}

result_t Http2Server::start()
{
    return m_server->start();
}

result_t Http2Server::stop(AsyncEvent* ac)
{
    return m_server->stop(ac);
}

result_t Http2Server::close(AsyncEvent* ac)
{
    return stop(ac);
}

result_t Http2Server::listen(int32_t port, exlib::string addr, int32_t backlog, AsyncEvent* ac)
{
    return static_cast<TLSServer*>(m_server.get())->listen(port, addr, backlog, ac);
}

result_t Http2Server::get_timeout(int32_t& retVal)
{
    return m_server->get_timeout(retVal);
}

result_t Http2Server::set_timeout(int32_t newVal)
{
    return m_server->set_timeout(newVal);
}

result_t Http2Server::address(obj_ptr<AddressType>& retVal)
{
    return m_server->address(retVal);
}

result_t Http2Server::get_socket(obj_ptr<Socket_base>& retVal)
{
    return m_server->get_socket(retVal);
}

result_t Http2Server::get_handler(obj_ptr<Handler_base>& retVal)
{
    retVal = m_handler;
    return 0;
}

result_t Http2Server::set_handler(Handler_base* newVal)
{
    m_handler = newVal;
    return 0;
}

result_t Http2Server::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    retVal = m_ctx;
    return 0;
}

result_t Http2Server::setSecureContext(SecureContext_base* context)
{
    m_ctx = context;
    return m_server->setSecureContext(context);
}

result_t Http2Server::setSecureContext(v8::Local<v8::Object> options)
{
    return m_server->setSecureContext(options);
}

} /* namespace fibjs */
