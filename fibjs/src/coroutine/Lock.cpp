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

result_t Lock::acquire(bool blocking, bool& retVal)
{
    if (!blocking) {
        retVal = m_lock.trylock();
        return 0;
    }

    if (!m_lock.trylock()) {
        Isolate::LeaveJsScope _rt(holder());
        m_lock.lock();
    }

    retVal = true;

    return 0;
}

result_t Lock::release()
{
    if (!m_lock.owned())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    m_lock.unlock();

    return 0;
}

result_t Lock::count(int32_t& retVal)
{
    retVal = m_lock.count();
    return 0;
}
}
