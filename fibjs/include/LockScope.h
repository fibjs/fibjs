/*
 * Locker.h
 *
 *  Created on: Oct 24, 2023
 *      Author: lion
 */

#pragma once

#include "ifs/Lock.h"

namespace fibjs {

class LockScope {
public:
    LockScope(Lock_base* l)
        : m_lock(l)
    {
        bool r;
        m_lock->acquire(true, r);
    }

    ~LockScope()
    {
        m_lock->release();
    }

private:
    obj_ptr<Lock_base> m_lock;
};
}
