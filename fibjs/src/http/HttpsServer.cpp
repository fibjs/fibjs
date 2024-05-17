/*
 * HttpsServer.cpp
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#include "object.h"
#include "HttpsServer.h"
#include "ifs/http.h"
#include "ifs/tls.h"
#include "ifs/TLSServer.h"

namespace fibjs {

result_t HttpsServer_base::_new(SecureContext_base* context, int32_t port, Handler_base* hdlr,
    obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(context, "", port, hdlr, retVal, This);
}

result_t HttpsServer_base::_new(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* hdlr,
    obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<HttpsServer> svr = new HttpsServer();
    svr->wrap(This);

    result_t hr = svr->create(context, addr, port, hdlr);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t HttpsServer_base::_new(v8::Local<v8::Object> options, Handler_base* hdlr,
    obj_ptr<HttpsServer_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<SecureContext_base> ctx;
    result_t hr = tls_base::createSecureContext(options, true, ctx);
    if (hr < 0)
        return hr;

    Isolate* isolate = Isolate::current(This);
    exlib::string address;
    hr = GetConfigValue(isolate, options, "address", address, true);
    if (hr < 0 && hr != CALL_E_PARAMNOTOPTIONAL)
        return hr;

    int32_t port;
    hr = GetConfigValue(isolate, options, "port", port, true);
    if (hr < 0)
        return hr;

    return _new(ctx, address, port, hdlr, retVal, This);
}

result_t HttpsServer::create(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* hdlr)
{
    result_t hr;
    obj_ptr<TLSServer_base> _server;
    obj_ptr<HttpHandler_base> _handler;

    hr = HttpHandler_base::_new(hdlr, _handler);
    if (hr < 0)
        return hr;

    hr = TLSServer_base::_new(context, addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_handler = _handler;

    SetPrivate("server", _server->wrap());
    m_server = _server;

    return 0;
}

result_t HttpsServer::start()
{
    return m_server->start();
}

result_t HttpsServer::stop(AsyncEvent* ac)
{
    return m_server->stop(ac);
}

result_t HttpsServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    return m_server->get_socket(retVal);
}

result_t HttpsServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    return m_handler->get_handler(retVal);
}

result_t HttpsServer::set_handler(Handler_base* newVal)
{
    return m_handler->set_handler(newVal);
}

result_t HttpsServer::enableCrossOrigin(exlib::string allowHeaders)
{
    return m_handler->enableCrossOrigin(allowHeaders);
}

result_t HttpsServer::get_maxHeadersCount(int32_t& retVal)
{
    return m_handler->get_maxHeadersCount(retVal);
}

result_t HttpsServer::set_maxHeadersCount(int32_t newVal)
{
    return m_handler->set_maxHeadersCount(newVal);
}

result_t HttpsServer::get_maxHeaderSize(int32_t& retVal)
{
    return m_handler->get_maxHeaderSize(retVal);
}

result_t HttpsServer::set_maxHeaderSize(int32_t newVal)
{
    return m_handler->set_maxHeaderSize(newVal);
}

result_t HttpsServer::get_maxBodySize(int32_t& retVal)
{
    return m_handler->get_maxBodySize(retVal);
}

result_t HttpsServer::set_maxBodySize(int32_t newVal)
{
    return m_handler->set_maxBodySize(newVal);
}

result_t HttpsServer::get_enableEncoding(bool& retVal)
{
    return m_handler->get_enableEncoding(retVal);
}

result_t HttpsServer::set_enableEncoding(bool newVal)
{
    return m_handler->set_enableEncoding(newVal);
}

result_t HttpsServer::get_serverName(exlib::string& retVal)
{
    return m_handler->get_serverName(retVal);
}

result_t HttpsServer::set_serverName(exlib::string newVal)
{
    return m_handler->set_serverName(newVal);
}

result_t HttpsServer::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    return m_server->get_secureContext(retVal);
}

result_t HttpsServer::setSecureContext(SecureContext_base* context)
{
    return m_server->setSecureContext(context);
}

result_t HttpsServer::setSecureContext(v8::Local<v8::Object> options)
{
    return m_server->setSecureContext(options);
}

} /* namespace fibjs */
