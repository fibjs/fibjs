/*
 * Locker.cpp
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "Lock.h"

namespace fibjs
{

result_t Lock_base::_new(obj_ptr<Lock_base>& retVal)
{
	retVal = new Lock();

	return 0;
}

result_t Lock::acquire(bool blocking, bool& retVal)
{
	if (!blocking)
		return m_lock.trylock();

	if(!m_lock.trylock())
	{
		v8::Unlocker unlocker(isolate);
		m_lock.lock();
	}

	return true;
}

result_t Lock::release()
{
	if (!m_lock.owned())
		return CALL_E_INVALID_CALL;

	m_lock.unlock();

	return 0;
}

}
