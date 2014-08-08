/*
 * Semaphore.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "Semaph.h"

namespace fibjs
{

result_t Semaphore_base::_new(int32_t value, obj_ptr<Semaphore_base> &retVal,
                              v8::Local<v8::Object> This)
{
    if (value < 0)
        return CHECK_ERROR(CALL_E_OUTRANGE);

    retVal = new Semaphore(value);

    return 0;
}

result_t Semaphore::acquire(bool blocking, bool &retVal)
{
    if (!blocking)
    {
        retVal = m_sem.trywait();
        return 0;
    }

    retVal = true;
    return wait();
}

result_t Semaphore::release()
{
    m_sem.post();

    return 0;
}

result_t Semaphore::wait()
{
    if (m_sem.trywait())
        return 0;

    v8::Unlocker unlocker(isolate);
    m_sem.wait();

    return 0;
}

result_t Semaphore::post()
{
    m_sem.post();

    return 0;
}

result_t Semaphore::trywait(bool &retVal)
{
    retVal = m_sem.trywait();

    return 0;
}

}
