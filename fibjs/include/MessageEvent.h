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
        return result_t();
    }

    // The type is stored at construction for exactly this getter; the other
    // MessageEvent fields have no fibjs counterpart, so they report the Web
    // defaults instead of inventing state.
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = m_type;
        return result_t();
    }

    virtual result_t get_origin(exlib::string& retVal)
    {
        retVal.clear();
        return result_t();
    }

    virtual result_t get_lastEventId(exlib::string& retVal)
    {
        retVal.clear();
        return result_t();
    }

    virtual result_t get_source(v8::Local<v8::Value>& retVal)
    {
        retVal = v8::Null(holder()->m_isolate);
        return result_t();
    }

    virtual result_t get_ports(v8::Local<v8::Array>& retVal)
    {
        retVal = v8::Array::New(holder()->m_isolate);
        return result_t();
    }

private:
    exlib::string m_type;
    v8::Global<v8::Value> m_data;
};

} /* namespace fibjs */
