/*
 * Locker.h
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#pragma once

#include "ifs/Lock.h"

namespace fibjs {

class Lock : public Lock_base {
    FIBER_FREE();

public:
    virtual result_t acquire(bool blocking, bool& retVal, AsyncEvent* ac);
    virtual result_t release();
    virtual result_t count(int32_t& retVal);
};
}
