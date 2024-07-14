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
    obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This)
{
    return _new("", port, hdlr, retVal, This);
}

result_t HttpServer_base::_new(exlib::string addr, int32_t port, Handler_base* hdlr,
    obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This)
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

result_t HttpServer_base::_new(exlib::string addr, Handler_base* hdlr,
    obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This)
{
    int32_t n = get_port(addr.c_str());

    if (n >= 0)
        return _new("", n, hdlr, retVal, This);
    return _new(addr, 0, hdlr, retVal, This);
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
    m_hdlr = _handler;

    SetPrivate("server", _server->wrap());
    m_server = _server;

    return _server->create(addr, port, _handler);
}

result_t HttpServer::start()
{
    return m_server->start();
}

result_t HttpServer::stop(AsyncEvent* ac)
{
    return m_server->stop(ac);
}

result_t HttpServer::get_timeout(int32_t& retVal)
{
    return m_server->get_timeout(retVal);
}

result_t HttpServer::set_timeout(int32_t newVal)
{
    return m_server->set_timeout(newVal);
}

result_t HttpServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    return m_server->get_socket(retVal);
}

result_t HttpServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    return m_hdlr->get_handler(retVal);
}

result_t HttpServer::set_handler(Handler_base* newVal)
{
    return m_hdlr->set_handler(newVal);
}

result_t HttpServer::enableCrossOrigin(exlib::string allowHeaders)
{
    return m_hdlr->enableCrossOrigin(allowHeaders);
}

result_t HttpServer::get_maxHeadersCount(int32_t& retVal)
{
    return m_hdlr->get_maxHeadersCount(retVal);
}

result_t HttpServer::set_maxHeadersCount(int32_t newVal)
{
    return m_hdlr->set_maxHeadersCount(newVal);
}

result_t HttpServer::get_maxHeaderSize(int32_t& retVal)
{
    return m_hdlr->get_maxHeaderSize(retVal);
}

result_t HttpServer::set_maxHeaderSize(int32_t newVal)
{
    return m_hdlr->set_maxHeaderSize(newVal);
}

result_t HttpServer::get_maxBodySize(int32_t& retVal)
{
    return m_hdlr->get_maxBodySize(retVal);
}

result_t HttpServer::set_maxBodySize(int32_t newVal)
{
    return m_hdlr->set_maxBodySize(newVal);
}

result_t HttpServer::get_enableEncoding(bool& retVal)
{
    return m_hdlr->get_enableEncoding(retVal);
}

result_t HttpServer::set_enableEncoding(bool newVal)
{
    return m_hdlr->set_enableEncoding(newVal);
}

result_t HttpServer::get_serverName(exlib::string& retVal)
{
    return m_hdlr->get_serverName(retVal);
}

result_t HttpServer::set_serverName(exlib::string newVal)
{
    return m_hdlr->set_serverName(newVal);
}

} /* namespace fibjs */
