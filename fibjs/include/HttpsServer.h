/*
 * HttpsServer.h
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#ifndef _fj_HTTPSSERVER_H_
#define _fj_HTTPSSERVER_H_

#include "ifs/HttpsServer.h"
#include "SslServer.h"
#include "HttpHandler.h"

namespace fibjs
{

class HttpsServer: public HttpsServer_base
{
    FIBER_FREE();

public:
    // object_base
    virtual result_t dispose()
    {
        m_server.dispose();
        m_handler.dispose();

        return 0;
    }

public:
    // TcpServer_base
    virtual result_t run(AsyncEvent *ac);
    virtual result_t asyncRun();
    virtual result_t stop(AsyncEvent *ac);
    virtual result_t get_socket(obj_ptr<Socket_base> &retVal);
    virtual result_t get_handler(obj_ptr<Handler_base> &retVal);
    virtual result_t set_handler(Handler_base *newVal);
    virtual result_t get_stats(obj_ptr<Stats_base> &retVal);

public:
    // HttpServer_base
    virtual result_t onerror(v8::Local<v8::Object> hdlrs);
    virtual result_t get_crossDomain(bool &retVal);
    virtual result_t set_crossDomain(bool newVal);
    virtual result_t get_forceGZIP(bool &retVal);
    virtual result_t set_forceGZIP(bool newVal);
    virtual result_t get_maxHeadersCount(int32_t &retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxUploadSize(int32_t &retVal);
    virtual result_t set_maxUploadSize(int32_t newVal);
    virtual result_t get_httpStats(obj_ptr<Stats_base> &retVal);

public:
    // HttpsServer_base
    virtual result_t get_verification(int32_t &retVal);
    virtual result_t set_verification(int32_t newVal);
    virtual result_t get_ca(obj_ptr<X509Cert_base> &retVal);

public:
    result_t create(v8::Local<v8::Array> certs, const char *addr, int32_t port, v8::Local<v8::Value> hdlr);
    result_t create(X509Cert_base *crt, PKey_base *key, const char *addr, int32_t port, v8::Local<v8::Value> hdlr);

private:
    naked_ptr<SslServer_base> m_server;
    naked_ptr<HttpHandler_base> m_handler;
};

} /* namespace fibjs */

#endif /* _fj_HTTPSSERVER_H_ */
