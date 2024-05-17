/*
 * TLSHandler.h
 *
 *  Created on: Mar 14, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/TLSHandler.h"

namespace fibjs {

class TLSHandler : public TLSHandler_base {
    FIBER_FREE();

public:
    TLSHandler(SecureContext_base* context)
        : m_ctx(context)
    {
    }

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    // TLSHandler_base
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal);
    virtual result_t setSecureContext(SecureContext_base* context);
    virtual result_t setSecureContext(v8::Local<v8::Object> options);
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal);
    virtual result_t set_handler(Handler_base* newVal);

private:
    obj_ptr<Handler_base> m_handler;
    obj_ptr<SecureContext_base> m_ctx;
};

} /* namespace fibjs */
