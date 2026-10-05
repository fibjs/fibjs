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
    // 受控同步快路径（C 类例外，见审计报告 §3-C）：锁能立即到手（或调用方
    // 不要求阻塞）时没有异步工作，直接在 sync 相位返回；只有真正需要阻塞
    // 等待才进 async 相位。
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
