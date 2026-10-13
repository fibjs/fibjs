/*
 * Semaphore.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "Semaph.h"

namespace fibjs {

result_t Semaphore_base::_new(int32_t value, obj_ptr<Semaphore_base>& retVal,
    v8::Local<v8::Object> This)
{
    if (value < 0)
        return result_t::make(CALL_E_EXCEPTION).with_format("Semaphore value must be non-negative, received %d.", value).with_result(result_t::out_of_range());

    retVal = new Semaphore(value);

    return result_t();
}

result_t Semaphore::acquire(bool blocking, bool& retVal, AsyncHandle ac)
{
    if (!blocking) {
        retVal = m_sem.trywait();
        return result_t();
    }

    return wait(-1, retVal, std::move(ac));
}

result_t Semaphore::release()
{
    m_sem.post();

    return result_t();
}

result_t Semaphore::count(int32_t& retVal)
{
    retVal = m_sem.count();
    return result_t();
}

result_t Semaphore::wait(int32_t timeout, bool& retVal, AsyncHandle ac)
{
    // 受控同步快路径（C 类例外，见审计报告 §3-C）：信号量有余量时立即返回，
    // 只有需要等待才进 async 相位。
    if (m_sem.trywait()) {
        retVal = true;
        return result_t();
    }

    if (ac.isSync())
        return result_t::nosync();

    retVal = m_sem.wait(timeout);
    return result_t();
}

result_t Semaphore::post()
{
    m_sem.post();

    return result_t();
}

result_t Semaphore::trywait(bool& retVal)
{
    retVal = m_sem.trywait();

    return result_t();
}
}
