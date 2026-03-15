/*
 * DOMEvent.h
 *
 *  Created on: Mar 16, 2026
 *      Author: lion
 */

#pragma once

#include "ifs/DOMEvent.h"

namespace fibjs {

class DOMEvent : public DOMEvent_base {
public:
    DOMEvent(exlib::string type, bool bubbles, bool cancelable, bool composed)
        : m_type(type)
        , m_bubbles(bubbles)
        , m_cancelable(cancelable)
        , m_composed(composed)
        , m_defaultPrevented(false)
        , m_timeStamp(0)
    {
        date_t d;
        d.now();
        m_timeStamp = d.date();
    }

public:
    // DOMEvent_base
    virtual result_t get_type(exlib::string& retVal)
    {
        retVal = m_type;
        return 0;
    }

    virtual result_t get_bubbles(bool& retVal)
    {
        retVal = m_bubbles;
        return 0;
    }

    virtual result_t get_cancelable(bool& retVal)
    {
        retVal = m_cancelable;
        return 0;
    }

    virtual result_t get_composed(bool& retVal)
    {
        retVal = m_composed;
        return 0;
    }

    virtual result_t get_defaultPrevented(bool& retVal)
    {
        retVal = m_defaultPrevented;
        return 0;
    }

    virtual result_t get_target(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = holder();
        retVal = v8::Null(isolate->m_isolate);
        return 0;
    }

    virtual result_t get_currentTarget(v8::Local<v8::Value>& retVal)
    {
        Isolate* isolate = holder();
        retVal = v8::Null(isolate->m_isolate);
        return 0;
    }

    virtual result_t get_timeStamp(double& retVal)
    {
        retVal = m_timeStamp;
        return 0;
    }

    virtual result_t stopPropagation()
    {
        return 0;
    }

    virtual result_t stopImmediatePropagation()
    {
        return 0;
    }

    virtual result_t preventDefault()
    {
        if (m_cancelable)
            m_defaultPrevented = true;
        return 0;
    }

private:
    exlib::string m_type;
    bool m_bubbles;
    bool m_cancelable;
    bool m_composed;
    bool m_defaultPrevented;
    double m_timeStamp;
};

} /* namespace fibjs */
