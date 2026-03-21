/*
 * Http2Server.h
 *
 *  Created on: Mar 21, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/Http2Server.h"
#include "ifs/TLSServer.h"
#include "ifs/SecureContext.h"
#include "ifs/Handler.h"

namespace fibjs {

class Http2Server : public Http2Server_base {
    FIBER_FREE();

public:
    // TcpServer_base
    virtual result_t start();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t close(AsyncEvent* ac);
    virtual result_t listen(int32_t port, exlib::string addr, int32_t backlog, AsyncEvent* ac);
    virtual result_t address(obj_ptr<AddressType>& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

public:
    // Http2Server_base
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    virtual result_t setSecureContext(SecureContext_base* context);
    virtual result_t setSecureContext(v8::Local<v8::Object> options);

public:
    result_t create(SecureContext_base* context, Handler_base* hdlr);
    result_t create(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* hdlr);

private:
    obj_ptr<TLSServer_base> m_server;
    obj_ptr<Handler_base> m_handler;
    obj_ptr<SecureContext_base> m_ctx;
};

} /* namespace fibjs */
