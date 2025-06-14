/*
 * EventSource.h
 *
 *  Created on: Dec 31, 2024
 *      Author: lion
 */

#pragma once

#include "ifs/EventSource.h"

namespace fibjs {

class EventSource : public EventSource_base {
public:
    // EventSource_base
    virtual result_t close(AsyncEvent* ac);
    virtual result_t get_readyState(int32_t& retVal);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_withCredentials(bool& retVal);
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal);

public:
    exlib::string m_url;
    int32_t m_readyState = C_CONNECTING;
    obj_ptr<HttpResponse_base> m_response;
};

}
