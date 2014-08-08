/*
 * Condition.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "Condition.h"

namespace fibjs
{

result_t Condition_base::_new(obj_ptr<Condition_base> &retVal, v8::Local<v8::Object> This)
{
    retVal = new Condition();

    return 0;
}

result_t Condition_base::_new(Lock_base *lock, obj_ptr<Condition_base> &retVal,
                              v8::Local<v8::Object> This)
{
    retVal = new Condition(lock);

    return 0;
}

result_t Condition::acquire(bool blocking, bool &retVal)
{
    return m_lockCond->acquire(blocking, retVal);
}

result_t Condition::release()
{
    return m_lockCond->release();
}

result_t Condition::wait()
{
    if (!m_lockCond->m_lock.owned())
        return CHECK_ERROR(CALL_E_INVALID_CALL);

    v8::Unlocker unlocker(isolate);
    m_cond.wait(m_lockCond->m_lock);

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
