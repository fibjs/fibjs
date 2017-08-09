/*
 * HttpServer.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#include "object.h"
#include "HttpServer.h"
#include "ifs/http.h"

namespace fibjs {

result_t HttpServer_base::_new(int32_t port, Handler_base* hdlr,
    obj_ptr<HttpServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new("", port, hdlr, retVal, This);
}

result_t HttpServer_base::_new(exlib::string addr, int32_t port, Handler_base* hdlr,
    obj_ptr<HttpServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    result_t hr;

    obj_ptr<HttpServer> svr = new HttpServer();
    svr->wrap(This);

    hr = svr->create(addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;
    return 0;
}

result_t HttpServer::create(exlib::string addr, int32_t port, Handler_base* hdlr)
{
    result_t hr;
    obj_ptr<TcpServer> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    _server = new TcpServer();

    SetPrivate("handler", _handler->wrap());
    m_hdlr.set(_handler);

    SetPrivate("server", _server->wrap());
    m_server.set(_server);

    return _server->create(addr, port, _handler);
}

result_t HttpServer::run(AsyncEvent* ac)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->run(ac);
}

result_t HttpServer::asyncRun()
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->asyncRun();
}

result_t HttpServer::stop(AsyncEvent* ac)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->stop(ac);
}

result_t HttpServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->get_socket(retVal);
}

result_t HttpServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_handler(retVal);
}

result_t HttpServer::set_handler(Handler_base* newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_handler(newVal);
}

result_t HttpServer::onerror(v8::Local<v8::Object> hdlrs)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->onerror(hdlrs);
}

result_t HttpServer::get_crossDomain(bool& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_crossDomain(retVal);
}

result_t HttpServer::set_crossDomain(bool newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_crossDomain(newVal);
}

result_t HttpServer::get_forceGZIP(bool& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_forceGZIP(retVal);
}

result_t HttpServer::set_forceGZIP(bool newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_forceGZIP(newVal);
}

result_t HttpServer::get_maxHeadersCount(int32_t& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_maxHeadersCount(retVal);
}

result_t HttpServer::set_maxHeadersCount(int32_t newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_maxHeadersCount(newVal);
}

result_t HttpServer::get_maxBodySize(int32_t& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_maxBodySize(retVal);
}

result_t HttpServer::set_maxBodySize(int32_t newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_maxBodySize(newVal);
}

result_t HttpServer::get_serverName(exlib::string& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_serverName(retVal);
}

result_t HttpServer::set_serverName(exlib::string newVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->set_serverName(newVal);
}

result_t HttpServer::get_httpStats(obj_ptr<Stats_base>& retVal)
{
    obj_ptr<HttpHandler_base> hdlr;
    m_hdlr.get(hdlr);
    return hdlr->get_stats(retVal);
}

result_t HttpServer::get_stats(obj_ptr<Stats_base>& retVal)
{
    obj_ptr<TcpServer_base> svr;
    m_server.get(svr);
    return svr->get_stats(retVal);
}

} /* namespace fibjs */
