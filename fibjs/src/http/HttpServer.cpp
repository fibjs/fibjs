/*
 * HttpServer.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#include "object.h"
#include "HttpServer.h"
#include "ifs/http.h"
#include "JSHandler.h"

namespace fibjs {

result_t HttpServer_base::_new(int32_t port, v8::Local<v8::Value> hdlr,
    obj_ptr<HttpServer_base>& retVal,
    v8::Local<v8::Object> This)
{
    return _new("", port, hdlr, retVal, This);
}

result_t HttpServer_base::_new(exlib::string addr, int32_t port, v8::Local<v8::Value> hdlr,
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

result_t HttpServer::create(exlib::string addr, int32_t port, v8::Local<v8::Value> hdlr)
{
    result_t hr;
    obj_ptr<TcpServer> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    _server = new TcpServer();

    m_server = _server;

    SetPrivate("handler", _handler->wrap());
    SetPrivate("server", _server->wrap());

    return _server->create(addr, port, _handler);
}

result_t HttpServer::run(AsyncEvent* ac)
{
    return m_server->run(ac);
}

result_t HttpServer::asyncRun()
{
    return m_server->asyncRun();
}

result_t HttpServer::stop(AsyncEvent* ac)
{
    return m_server->stop(ac);
}

result_t HttpServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    return m_server->get_socket(retVal);
}

result_t HttpServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    return handler()->get_handler(retVal);
}

result_t HttpServer::set_handler(Handler_base* newVal)
{
    return handler()->set_handler(newVal);
}

result_t HttpServer::onerror(v8::Local<v8::Object> hdlrs)
{
    return handler()->onerror(hdlrs);
}

result_t HttpServer::get_crossDomain(bool& retVal)
{
    return handler()->get_crossDomain(retVal);
}

result_t HttpServer::set_crossDomain(bool newVal)
{
    return handler()->set_crossDomain(newVal);
}

result_t HttpServer::get_forceGZIP(bool& retVal)
{
    return handler()->get_forceGZIP(retVal);
}

result_t HttpServer::set_forceGZIP(bool newVal)
{
    return handler()->set_forceGZIP(newVal);
}

result_t HttpServer::get_maxHeadersCount(int32_t& retVal)
{
    return handler()->get_maxHeadersCount(retVal);
}

result_t HttpServer::set_maxHeadersCount(int32_t newVal)
{
    return handler()->set_maxHeadersCount(newVal);
}

result_t HttpServer::get_maxUploadSize(int32_t& retVal)
{
    return handler()->get_maxUploadSize(retVal);
}

result_t HttpServer::set_maxUploadSize(int32_t newVal)
{
    return handler()->set_maxUploadSize(newVal);
    ;
}

result_t HttpServer::get_httpStats(obj_ptr<Stats_base>& retVal)
{
    return handler()->get_stats(retVal);
}

result_t HttpServer::get_stats(obj_ptr<Stats_base>& retVal)
{
    return m_server->get_stats(retVal);
}

} /* namespace fibjs */
