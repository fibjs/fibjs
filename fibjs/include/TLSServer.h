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
    virtual result_t close(AsyncEvent* ac);
    virtual result_t listen(int32_t port, exlib::string addr, int32_t backlog, AsyncEvent* ac);
    virtual result_t address(obj_ptr<AddressType>& retVal);
    virtual result_t get_timeout(int32_t& retVal);
    virtual result_t set_timeout(int32_t newVal);
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

public:
    // TLSServer_base
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    virtual result_t setSecureContext(SecureContext_base* context);
    virtual result_t setSecureContext(v8::Local<v8::Object> options);

public:
    result_t setup(SecureContext_base* context, Handler_base* listener);
    result_t create(SecureContext_base* context, exlib::string addr, int32_t port, Handler_base* listener);

    void set_event_delegate(object_base* delegate)
    {
        static_cast<TcpServer*>(m_server.get())->m_eventDelegate = delegate;
    }

private:
    obj_ptr<TcpServer_base> m_server;
    obj_ptr<TLSHandler_base> m_handler;
};

} /* namespace fibjs */
