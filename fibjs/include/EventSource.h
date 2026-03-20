/*
 * EventSource.h
 *
 *  Created on: Dec 31, 2024
 *      Author: lion
 */

#pragma once

#include <boost/preprocessor.hpp>

#include "ifs/EventSource.h"
#include "ifs/sse.h"

namespace fibjs {

class EventSource : public EventSource_base {
public:
    class SendOptions : public obj_base {
    public:
        LOAD_OPTIONS(SendOptions, (id)(event)(retry));

    public:
        std::optional<exlib::string> id;
        std::optional<exlib::string> event;
        std::optional<int32_t> retry;
    };

public:
    // EventSource_base
    virtual result_t close(AsyncEvent* ac);
    virtual result_t send(exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac);
    virtual result_t get_readyState(int32_t& retVal);
    virtual result_t get_url(exlib::string& retVal);
    virtual result_t get_withCredentials(bool& retVal);
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal);

public:
    exlib::string m_url;
    int32_t m_readyState = sse_base::C_CONNECTING;
    obj_ptr<HttpMessage_base> m_response;
    obj_ptr<Stream_base> m_stream;
    AsyncEvent* m_ac = nullptr;
};

}
