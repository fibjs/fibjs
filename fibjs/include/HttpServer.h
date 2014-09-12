/*
 * HttpServer.h
 *
 *  Created on: Dec 8, 2013
 *      Author: lion
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "ifs/HttpServer.h"
#include "TcpServer.h"
#include "HttpHandler.h"

namespace fibjs
{

class HttpServer: public HttpServer_base
{
    FIBER_FREE();

public:
    // object_base
    virtual result_t dispose()
    {
        return 0;
    }

public:
    // TcpServer_base
    virtual result_t run(exlib::AsyncEvent *ac);
    virtual result_t asyncRun();
    virtual result_t stop(exlib::AsyncEvent *ac);
    virtual result_t get_socket(obj_ptr<Socket_base> &retVal);
    virtual result_t get_handler(obj_ptr<Handler_base> &retVal);
    virtual result_t set_handler(Handler_base *newVal);
    virtual result_t get_stats(obj_ptr<Stats_base> &retVal);

public:
    // HttpServer_base
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
    result_t create(const char *addr, int32_t port, v8::Local<v8::Value> hdlr);

private:
    naked_ptr<TcpServer> m_server;
    naked_ptr<HttpHandler_base> m_handler;
};

} /* namespace fibjs */

#endif /* HTTPSERVER_H_ */
