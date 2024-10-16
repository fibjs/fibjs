/*
 * Condition.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "Condition.h"

namespace fibjs {

result_t Condition_base::_new(obj_ptr<Condition_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new Condition();

    return 0;
}

result_t Condition_base::_new(Lock_base* lock, obj_ptr<Condition_base>& retVal,
    v8::Local<v8::Object> This)
{
    retVal = new Condition(lock);

    return 0;
}

result_t Condition::acquire(bool blocking, bool& retVal, AsyncEvent* ac)
{
    return m_lockCond->acquire(blocking, retVal, ac);
}

result_t Condition::release()
{
    return m_lockCond->release();
}

result_t Condition::count(int32_t& retVal)
{
    retVal = m_cond.count();
    return 0;
}

result_t Condition::wait(int32_t timeout, bool& retVal, AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    retVal = m_cond.wait(m_lockCond->m_lock, timeout);
    return 0;
}

result_t Condition::notify()
{
    result_t hr = m_lockCond->release();
    if (hr < 0)
        return hr;

    m_cond.notify_one();

    return 0;
}

result_t Condition::notifyAll()
{
    result_t hr = m_lockCond->release();
    if (hr < 0)
        return hr;

    m_cond.notify_all();

    return 0;
}
}
