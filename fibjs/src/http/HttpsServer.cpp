/*
 * HttpsServer.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#include "object.h"
#include "HttpsServer.h"
#include "ifs/http.h"

namespace fibjs {

result_t HttpsServer_base::_new(v8::Local<v8::Array> certs, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(certs, "", port, hdlr, retVal, This);
}

result_t HttpsServer_base::_new(v8::Local<v8::Array> certs, exlib::string addr, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    svr->wrap(This);

    result_t hr = svr->create(certs, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer_base::_new(X509Cert_base* crt, PKey_base* key, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new(crt, key, "", port, hdlr, retVal, This);
}

result_t HttpsServer_base::_new(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port,
    Handler_base* hdlr, obj_ptr<HttpsServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    svr->wrap(This);

    result_t hr = svr->create(crt, key, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer::create(X509Cert_base* crt, PKey_base* key, exlib::string addr, int32_t port,
    Handler_base* hdlr)
{
    result_t hr;
    obj_ptr<SslServer_base> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    hr = SslServer_base::_new(crt, key, addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_hdlr.set(_handler);

    SetPrivate("server", _server->wrap());
    m_server.set(_server);

    return 0;
}

result_t HttpsServer::create(v8::Local<v8::Array> certs, exlib::string addr, int32_t port,
    Handler_base* hdlr)
{
    result_t hr;
    obj_ptr<SslServer_base> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    hr = SslServer_base::_new(certs, addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_hdlr.set(_handler);

    SetPrivate("server", _server->wrap());
    m_server.set(_server);

    return 0;
}

result_t HttpsServer::run(AsyncEvent* ac)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->run(ac);
}

result_t HttpsServer::asyncRun()
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->asyncRun();
}

result_t HttpsServer::stop(AsyncEvent* ac)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->stop(ac);
}

result_t HttpsServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->get_socket(retVal);
}

result_t HttpsServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_handler(retVal);
}

result_t HttpsServer::set_handler(Handler_base* newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_handler(newVal);
}

result_t HttpsServer::onerror(v8::Local<v8::Object> hdlrs)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->onerror(hdlrs);
}

result_t HttpsServer::get_crossDomain(bool& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_crossDomain(retVal);
}

result_t HttpsServer::set_crossDomain(bool newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_crossDomain(newVal);
}

result_t HttpsServer::get_forceGZIP(bool& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_forceGZIP(retVal);
}

result_t HttpsServer::set_forceGZIP(bool newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_forceGZIP(newVal);
}

result_t HttpsServer::get_maxHeadersCount(int32_t& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_maxHeadersCount(retVal);
}

result_t HttpsServer::set_maxHeadersCount(int32_t newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_maxHeadersCount(newVal);
}

result_t HttpsServer::get_maxBodySize(int32_t& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_maxBodySize(retVal);
}

result_t HttpsServer::set_maxBodySize(int32_t newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_maxBodySize(newVal);
}

result_t HttpsServer::get_serverName(exlib::string& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_serverName(retVal);
}

result_t HttpsServer::set_serverName(exlib::string newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_serverName(newVal);
}

result_t HttpsServer::get_verification(int32_t& retVal)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->get_verification(retVal);
}
result_t HttpsServer::set_verification(int32_t newVal)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->set_verification(newVal);
}
result_t HttpsServer::get_ca(obj_ptr<X509Cert_base>& retVal)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->get_ca(retVal);
}

result_t HttpsServer::get_httpStats(obj_ptr<Stats_base>& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_stats(retVal);
}

result_t HttpsServer::get_stats(obj_ptr<Stats_base>& retVal)
{
    obj_ptr<SslServer_base> svr;
    m_server.get(svr);
    return svr->get_stats(retVal);
}

} /* namespace fibjs */
