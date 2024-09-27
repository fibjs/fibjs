/*
 * Locker.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "Lock.h"

namespace fibjs {

result_t Lock_base::_new(obj_ptr<Lock_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Lock();

    return 0;
}

result_t Lock::acquire(bool blocking, bool& retVal, AsyncEvent* ac)
{
    if (m_lock.trylock()) {
        retVal = true;
        return 0;
    }

    if (!blocking) {
        retVal = false;
        return 0;
    }

    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = true;
    m_lock.lock();
    return 0;
}

result_t Lock::release()
{
    m_lock.unlock();
    return 0;
}

result_t Lock::count(int32_t& retVal)
{
    retVal = m_lock.count();
    return 0;
}
}
