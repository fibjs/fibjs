/*
 * AsyncWaitHandler.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: lion
 */

#include "AsyncWaitHandler.h"

namespace fibjs
{

result_t AsyncWaitHandler::invoke(object_base* v, obj_ptr<Handler_base>& retVal,
		exlib::AsyncEvent* ac)
{
	if (!ac)
		return CALL_E_NOSYNC;

	m_as = new asyncWaiter(ac);
	m_ac.post(0);

	return CALL_E_PENDDING;
}

result_t AsyncWaitHandler::end()
{
	m_ac.wait();
	if(m_as)
	{
		s_acPool.put(m_as);
		m_as = NULL;
	}

	return 0;
}

} /* namespace fibjs */
