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
                                  AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_invoked.xchg(1) != 0)
        return 0;

    m_as = new asyncWaiter(ac);
    if (m_stat.CompareAndSwap(AC_INIT, AC_WAIT) == AC_INIT)
        return CHECK_ERROR(CALL_E_PENDDING);

    delete m_as;
    m_as = NULL;

    return CHECK_ERROR(CALL_RETURN_NULL);
}

result_t AsyncWaitHandler::end()
{
    if (m_stat.xchg(AC_END) == AC_WAIT)
    {
        m_as->async(false);
        m_as = NULL;
    }

    return 0;
}

} /* namespace fibjs */
