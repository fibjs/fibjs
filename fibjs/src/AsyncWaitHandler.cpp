/*
 * AsyncWaitHandler.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: lion
 */

#include "AsyncWaitHandler.h"

namespace fibjs
{

#define AC_INIT    0
#define AC_WAIT    1
#define AC_END     2

result_t AsyncWaitHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                                  exlib::AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (exlib::atom_xchg(&m_invoked, 1) != 0)
        return 0;

    m_as = new asyncWaiter(ac);
    if (exlib::CompareAndSwap(&m_stat, AC_INIT, AC_WAIT) == AC_INIT)
        return CHECK_ERROR(CALL_E_PENDDING);

    delete m_as;
    m_as = NULL;

    return CHECK_ERROR(CALL_RETURN_NULL);
}

result_t AsyncWaitHandler::end()
{
    if (exlib::atom_xchg(&m_stat, AC_END) == AC_WAIT)
    {
        s_acPool.put(m_as);
        m_as = NULL;
    }

    return 0;
}

} /* namespace fibjs */
