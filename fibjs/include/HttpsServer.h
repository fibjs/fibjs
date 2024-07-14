/*
 * HttpsServer.h
 *
 *  Created on: Apr 22, 2014
 *      Author: lion
 */

#pragma once

#include "ifs/HttpsServer.h"
#include "ifs/TLSServer.h"
#include "HttpHandler.h"

namespace fibjs {

class HttpsServer : public HttpsServer_base {
    FIBER_FREE();

public:
    // TcpServer_base
    virtual result_t start();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

public:
    // HttpServer_base
    virtual result_t enableCrossOrigin(exlib::string allowHeaders);
    virtual result_t get_maxHeadersCount(int32_t& retVal);
    virtual result_t set_maxHeadersCount(int32_t newVal);
    virtual result_t get_maxHeaderSize(int32_t& retVal);
    virtual result_t set_maxHeaderSize(int32_t newVal);
    virtual result_t get_maxBodySize(int32_t& retVal);
    virtual result_t set_maxBodySize(int32_t newVal);
    virtual result_t get_enableEncoding(bool& retVal);
    virtual result_t set_enableEncoding(bool newVal);
    virtual result_t get_serverName(exlib::string& retVal);
    virtual result_t set_serverName(exlib::string newVal);

public:
    // HttpsServer_base
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    virtual result_t setSecureContext(SecureContext_base* context);
    virtual result_t setSecureContext(v8::Local<v8::Object> options);

public:
    result_t create(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* hdlr);

private:
    obj_ptr<TLSServer_base> m_server;
    obj_ptr<HttpHandler_base> m_handler;
};

} /* namespace fibjs */
