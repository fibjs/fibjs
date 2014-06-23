/*
 * SslServer.cpp
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#include "SslServer.h"
#include "JSHandler.h"

namespace fibjs
{

result_t SslServer_base::_new(v8::Local<v8::Array> certs, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal)
{
    return _new(certs, "", port, listener, retVal);
}

result_t SslServer_base::_new(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal)
{
    obj_ptr<SslServer> svr = new SslServer();
    result_t hr = svr->create(certs, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t SslServer_base::_new(X509Cert_base *crt, PKey_base *key, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal)
{
    return _new(crt, key, "", port, listener, retVal);
}

result_t SslServer_base::_new(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                              v8::Local<v8::Value> listener, obj_ptr<SslServer_base> &retVal)
{
    obj_ptr<SslServer> svr = new SslServer();
    result_t hr = svr->create(crt, key, addr, port, listener);
    if (hr < 0)
        return hr;

    retVal = svr;

    return 0;
}

result_t SslServer::create(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port,
                           v8::Local<v8::Value> listener)
{
    result_t hr;

    hr = SslHandler_base::_new(crt, key, listener, m_handler);
    if (hr < 0)
        return hr;


    hr = m_handler->ValueOf(listener);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, listener, m_server);
    if (hr < 0)
        return hr;

    return 0;
}

result_t SslServer::create(v8::Local<v8::Array> certs, const char *addr, int32_t port,
                           v8::Local<v8::Value> listener)
{
    result_t hr;

    hr = SslHandler_base::_new(certs, listener, m_handler);
    if (hr < 0)
        return hr;

    hr = m_handler->ValueOf(listener);
    if (hr < 0)
        return hr;

    hr = TcpServer_base::_new(addr, port, listener, m_server);
    if (hr < 0)
        return hr;

    return 0;
}

result_t SslServer::run(exlib::AsyncEvent *ac)
{
    return m_server->run(ac);
}

result_t SslServer::asyncRun()
{
    return m_server->asyncRun();
}

result_t SslServer::get_socket(obj_ptr<Socket_base> &retVal)
{
    return m_server->get_socket(retVal);
}

result_t SslServer::get_handler(obj_ptr<Handler_base> &retVal)
{
    return m_handler->get_handler(retVal);
}

result_t SslServer::set_handler(Handler_base *newVal)
{
    return m_handler->set_handler(newVal);
}

result_t SslServer::get_verification(int32_t &retVal)
{
    return m_handler->get_verification(retVal);
}

result_t SslServer::set_verification(int32_t newVal)
{
    return m_handler->set_verification(newVal);
}

result_t SslServer::get_ca(obj_ptr<X509Cert_base> &retVal)
{
    return m_handler->get_ca(retVal);
}

result_t SslServer::get_stats(obj_ptr<Stats_base> &retVal)
{
    return m_server->get_stats(retVal);
}

}
