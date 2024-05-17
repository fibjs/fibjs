/*
 * TLSServer.cpp
 *
 *  Created on: Mar 14, 2024
 *      Author: lion
 */

#include "object.h"
#include "ifs/tls.h"
#include "TLSServer.h"

namespace fibjs {

result_t TLSServer_base::_new(SecureContext_base* context, int32_t port, Handler_base* listener,
    obj_ptr<TLSServer_base>& retVal, v8::Local<v8::Object> This)
{
    return _new(context, "", port, listener, retVal, This);
}

result_t TLSServer_base::_new(SecureContext_base* context, exlib::string addr, int32_t port,
    Handler_base* listener, obj_ptr<TLSServer_base>& retVal, v8::Local<v8::Object> This)
{
    obj_ptr<TLSServer> svr = new TLSServer();
    svr->wrap(This);

    result_t hr = svr->create(context, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t TLSServer_base::_new(v8::Local<v8::Object> options, Handler_base* listener,
    obj_ptr<TLSServer_base>& retVal, v8::Local<v8::Object> This)
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

    return _new(ctx, address, port, listener, retVal, This);
}

result_t TLSServer::create(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* listener)
{
    result_t hr;
    obj_ptr<TcpServer_base> _server;
    obj_ptr<TLSHandler_base> _handler;

    hr = TLSHandler_base::_new(context, listener, _handler);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, _handler, _server);
    if (hr < 0)
        return hr;

    SetPrivate("handler", _handler->wrap());
    m_handler = _handler;

    SetPrivate("server", _server->wrap());
    m_server = _server;

    return 0;
}

result_t TLSServer::start()
{
    return m_server->start();
}

result_t TLSServer::stop(AsyncEvent* ac)
{
    return m_server->stop(ac);
}

result_t TLSServer::get_socket(obj_ptr<Socket_base>& retVal)
{
    return m_server->get_socket(retVal);
}

result_t TLSServer::get_handler(obj_ptr<Handler_base>& retVal)
{
    return m_handler->get_handler(retVal);
}

result_t TLSServer::set_handler(Handler_base* newVal)
{
    return m_handler->set_handler(newVal);
}

result_t TLSServer::get_secureContext(obj_ptr<SecureContext_base>& retVal)
{
    return m_handler->get_secureContext(retVal);
}

result_t TLSServer::setSecureContext(SecureContext_base* context)
{
    return m_handler->setSecureContext(context);
}

result_t TLSServer::setSecureContext(v8::Local<v8::Object> options)
{
    return m_handler->setSecureContext(options);
}

}
