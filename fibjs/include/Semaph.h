/*
 * Semaphore.h
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Semaphore.h"

namespace fibjs {

class Semaphore : public Semaphore_base {
public:
    Semaphore(int32_t count)
        : m_sem(count)
    {
    }

    FIBER_FREE();

public:
    // Lock_base
    virtual result_t acquire(bool blocking, bool& retVal, AsyncEvent* ac);
    virtual result_t release();
    virtual result_t count(int32_t& retVal);

public:
    // Semaphore_base
    virtual result_t wait(int32_t timeout, bool& retVal, AsyncEvent* ac);
    virtual result_t post();
    virtual result_t trywait(bool& retVal);

private:
    exlib::Semaphore m_sem;
};
}
