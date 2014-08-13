/*
 * HttpsServer.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#include "HttpsServer.h"
#include "ifs/http.h"
#include "JSHandler.h"

namespace fibjs
{

result_t HttpsServer_base::_new(v8::Local<v8::Array> certs, int32_t port,
                                v8::Local<v8::Value> hdlr, obj_ptr<HttpsServer_base> &retVal,
                                v8::Local<v8::Object> This)
{
    return _new(certs, "", port, hdlr, retVal);
}

result_t HttpsServer_base::_new(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                                v8::Local<v8::Value> hdlr, obj_ptr<HttpsServer_base> &retVal,
                                v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    result_t hr = svr->create(certs, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer_base::_new(X509Cert_base *crt, PKey_base *key, int32_t port,
                                v8::Local<v8::Value> hdlr, obj_ptr<HttpsServer_base> &retVal,
                                v8::Local<v8::Object> This)
{
    return _new(crt, key, "", port, hdlr, retVal);
}

result_t HttpsServer_base::_new(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                                v8::Local<v8::Value> hdlr, obj_ptr<HttpsServer_base> &retVal,
                                v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    result_t hr = svr->create(crt, key, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer::create(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                             v8::Local<v8::Value> hdlr)
{
    result_t hr;
    hr = HttpHandler_base::_new(hdlr, m_handler);
    if (hr < 0)
        return hr;

    hr = m_handler->ValueOf(hdlr);
    if (hr < 0)
        return hr;

    hr = SslServer_base::_new(crt, key, addr, port, hdlr, m_server);
    if (hr < 0)
        return hr;

    return 0;
}

result_t HttpsServer::create(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                             v8::Local<v8::Value> hdlr)
{
    result_t hr;
    hr = HttpHandler_base::_new(hdlr, m_handler);
    if (hr < 0)
        return hr;

    hr = m_handler->ValueOf(hdlr);
    if (hr < 0)
        return hr;

    hr = SslServer_base::_new(certs, addr, port, hdlr, m_server);
    if (hr < 0)
        return hr;

    return 0;
}

result_t HttpsServer::run(exlib::AsyncEvent *ac)
{
    return m_server->run(ac);
}

result_t HttpsServer::asyncRun()
{
    return m_server->asyncRun();
}

result_t HttpsServer::get_socket(obj_ptr<Socket_base> &retVal)
{
    return m_server->get_socket(retVal);
}

result_t HttpsServer::get_handler(obj_ptr<Handler_base> &retVal)
{
    return m_handler->get_handler(retVal);
}

result_t HttpsServer::set_handler(Handler_base *newVal)
{
    return m_handler->set_handler(newVal);
}

result_t HttpsServer::get_crossDomain(bool &retVal)
{
    return m_handler->get_crossDomain(retVal);
}

result_t HttpsServer::set_crossDomain(bool newVal)
{
    return m_handler->set_crossDomain(newVal);
}

result_t HttpsServer::get_forceGZIP(bool &retVal)
{
    return m_handler->get_forceGZIP(retVal);
}

result_t HttpsServer::set_forceGZIP(bool newVal)
{
    return m_handler->set_forceGZIP(newVal);
}

result_t HttpsServer::get_maxHeadersCount(int32_t &retVal)
{
    return m_handler->get_maxHeadersCount(retVal);
}

result_t HttpsServer::set_maxHeadersCount(int32_t newVal)
{
    return m_handler->set_maxHeadersCount(newVal);
}

result_t HttpsServer::get_maxUploadSize(int32_t &retVal)
{
    return m_handler->get_maxUploadSize(retVal);
}

result_t HttpsServer::set_maxUploadSize(int32_t newVal)
{
    return m_handler->set_maxUploadSize(newVal);;
}

result_t HttpsServer::get_verification(int32_t &retVal)
{
    return m_server->get_verification(retVal);
}
result_t HttpsServer::set_verification(int32_t newVal)
{
    return m_server->set_verification(newVal);
}
result_t HttpsServer::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    return m_server->get_ca(retVal);
}

result_t HttpsServer::get_httpStats(obj_ptr<Stats_base> &retVal)
{
    return m_handler->get_stats(retVal);
}

result_t HttpsServer::get_tcpStats(obj_ptr<Stats_base> &retVal)
{
    return m_server->get_stats(retVal);
}

} /* namespace fibjs */
