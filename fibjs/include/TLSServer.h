/*
 * TLSServer.h
 *
 *  Created on: Mar 14, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/TLSServer.h"
#include "TcpServer.h"
#include "TLSHandler.h"

namespace fibjs {

class TLSServer : public TLSServer_base {
    FIBER_FREE();

public:
    // TcpServer_base
    virtual result_t start();
    virtual result_t stop(AsyncEvent* ac);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

public:
    // TLSServer_base
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    virtual result_t setSecureContext(SecureContext_base* context);
    virtual result_t setSecureContext(v8::Local<v8::Object> options);

public:
    result_t create(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* listener);

private:
    obj_ptr<TcpServer_base> m_server;
    obj_ptr<TLSHandler_base> m_handler;
};

} /* namespace fibjs */
