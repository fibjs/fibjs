/*
 * MessageEvent.h
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/MessageEvent.h"

namespace fibjs {

class MessageEvent : public MessageEvent_base {
public:
    MessageEvent(exlib::string type, v8::Local<v8::Value> data)
        : m_type(type)
    {
        Isolate* isolate = Isolate::current();
        m_data.Reset(isolate->m_isolate, data);
    }

public:
    // MessageEvent_base
    virtual result_t get_data(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = holder();
        if (m_data.IsEmpty())
            retVal = v8::Undefined(isolate->m_isolate);
        else
            retVal = m_data.Get(isolate->m_isolate);
        return 0;
    }

private:
    exlib::string m_type;
    v8::Global<v8::Value> m_data;
};

} /* namespace fibjs */
