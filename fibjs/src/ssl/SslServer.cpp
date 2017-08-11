/*
 * SslServer.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#include "object.h"
#include "SslServer.h"

namespace fibjs {

result_t SslServer_base::_new(v8::Local<v8::Array> certs, int32_t port,
    Handler_base* listener, obj_ptr<SslServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(certs, "", port, listener, retVal, This);
}

result_t SslServer_base::_new(v8::Local<v8::Array> certs, exlib::string addr, int32_t port,
    Handler_base* listener, obj_ptr<SslServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<SslServer> svr = new SslServer();
    svr->wrap(This);

    result_t hr = svr->create(certs, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t SslServer_base::_new(X509Cert_base* crt, PKey_base* key, int32_t port,
    Handler_base* listener, obj_ptr<SslServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(crt, key, "", port, listener, retVal, This);
}

result_t SslServer_base::_new(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port,
    Handler_base* listener, obj_ptr<SslServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<SslServer> svr = new SslServer();
    svr->wrap(This);

    result_t hr = svr->create(crt, key, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t SslServer::create(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port,
    Handler_base* listener)
{
    result_t hr;
    obj_ptr<TcpServer_base> _server;
    obj_ptr<SslHandler_base> _handler;

    hr = SslHandler_base::_new(crt, key, listener, _handler);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_hdlr.set(_handler);

    SetPrivate("server", _server->wrap());
    m_server.set(_server);

    return 0;
}

result_t SslServer::create(v8::Local<v8::Array> certs, exlib::string addr, int32_t port,
    Handler_base* listener)
{
    result_t hr;
    obj_ptr<TcpServer_base> _server;
    obj_ptr<SslHandler_base> _handler;

    hr = SslHandler_base::_new(certs, listener, _handler);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_hdlr.set(_handler);

    SetPrivate("server", _server->wrap());
    m_server.set(_server);

    return 0;
}

result_t SslServer::run(AsyncEvent* ac)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->run(ac);
}

result_t SslServer::asyncRun()
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->asyncRun();
}

result_t SslServer::stop(AsyncEvent* ac)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->stop(ac);
}

result_t SslServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->get_socket(retVal);
}

result_t SslServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    obj_ptr<SslHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_handler(retVal);
}

result_t SslServer::set_handler(Handler_base* newVal)
{
    obj_ptr<SslHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_handler(newVal);
}

result_t SslServer::get_verification(int32_t& retVal)
{
    obj_ptr<SslHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_verification(retVal);
}

result_t SslServer::set_verification(int32_t newVal)
{
    obj_ptr<SslHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_verification(newVal);
}

result_t SslServer::get_ca(obj_ptr<X509Cert_base>& retVal)
{
    obj_ptr<SslHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_ca(retVal);
}

result_t SslServer::get_stats(obj_ptr<Stats_base>& retVal)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->get_stats(retVal);
}
}
