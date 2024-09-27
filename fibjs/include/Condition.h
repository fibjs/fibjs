/*
 * Condition.h
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Condition.h"
#include "Lock.h"

namespace fibjs {

class Condition : public Condition_base {
public:
    Condition(Lock_base* lock)
        : m_lockCond(lock)
    {
    }

    Condition()
    {
        m_lockCond = new Lock();
    }

    FIBER_FREE();

public:
    // Lock_base
    virtual result_t acquire(bool blocking, bool& retVal, AsyncEvent* ac);
    virtual result_t release();
    virtual result_t count(int32_t& retVal);

public:
    // Condition_base
    virtual result_t wait(int32_t timeout, bool& retVal, AsyncEvent* ac);
    virtual result_t notify();
    virtual result_t notifyAll();

private:
    obj_ptr<Lock_base> m_lockCond;
    exlib::CondVar m_cond;
};
}
