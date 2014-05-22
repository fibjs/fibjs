/*
 * HttpServer.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#include "HttpServer.h"
#include "ifs/http.h"
#include "JSHandler.h"

namespace fibjs
{

result_t HttpServer_base::_new(int32_t port, v8::Local<v8::Value> hdlr,
                               obj_ptr<HttpServer_base> &retVal)
{
    return _new("", port, hdlr, retVal);
}

result_t HttpServer_base::_new(const char *addr, int32_t port, v8::Local<v8::Value> hdlr,
                               obj_ptr<HttpServer_base> &retVal)
{
    result_t hr;

    obj_ptr<HttpServer> svr = new HttpServer();
    hr = svr->create(addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;
    return 0;
}

result_t HttpServer::create(const char *addr, int32_t port, v8::Local<v8::Value> hdlr)
{
    obj_ptr<Handler_base> hdlr1;
    result_t hr;

    hr = JSHandler::New(hdlr, hdlr1);
    if (hr < 0)
        return hr;

    m_server = new TcpServer();
    m_handler = new HttpHandler(hdlr1);

    return m_server->create(addr, port, m_handler);
}

result_t HttpServer::run(exlib::AsyncEvent *ac)
{
    return m_server->run(ac);
}

result_t HttpServer::asyncRun()
{
    return m_server->asyncRun();
}

result_t HttpServer::get_socket(obj_ptr<Socket_base> &retVal)
{
    return m_server->get_socket(retVal);
}

result_t HttpServer::get_crossDomain(bool &retVal)
{
    return m_handler->get_crossDomain(retVal);
}

result_t HttpServer::set_crossDomain(bool newVal)
{
    return m_handler->set_crossDomain(newVal);
}

result_t HttpServer::get_forceGZIP(bool &retVal)
{
    return m_handler->get_forceGZIP(retVal);
}

result_t HttpServer::set_forceGZIP(bool newVal)
{
    return m_handler->set_forceGZIP(newVal);
}

result_t HttpServer::get_maxHeadersCount(int32_t &retVal)
{
    return m_handler->get_maxHeadersCount(retVal);
}

result_t HttpServer::set_maxHeadersCount(int32_t newVal)
{
    return m_handler->set_maxHeadersCount(newVal);
}

result_t HttpServer::get_maxUploadSize(int32_t &retVal)
{
    return m_handler->get_maxUploadSize(retVal);
}

result_t HttpServer::set_maxUploadSize(int32_t newVal)
{
    return m_handler->set_maxUploadSize(newVal);;
}

result_t HttpServer::get_httpStats(obj_ptr<Stats_base> &retVal)
{
    return m_handler->get_stats(retVal);
}

result_t HttpServer::get_tcpStats(obj_ptr<Stats_base> &retVal)
{
    return m_server->get_stats(retVal);
}

} /* namespace fibjs */
