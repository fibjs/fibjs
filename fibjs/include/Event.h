/*
 * Event.h
 *
 *  Created on: Apr 30, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Event.h"

namespace fibjs {

class Event : public Event_base {

    FIBER_FREE();

public:
    // Lock_base
    virtual result_t acquire(bool blocking, bool& retVal, AsyncEvent* ac);
    virtual result_t release();
    virtual result_t count(int32_t& retVal);

public:
    // Event_base
    virtual result_t isSet(bool& retVal);
    virtual result_t set();
    virtual result_t pulse();
    virtual result_t clear();
    virtual result_t wait(AsyncEvent* ac);

private:
    exlib::Event m_event;
};
}
