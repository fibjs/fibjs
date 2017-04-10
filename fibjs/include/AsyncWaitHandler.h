/*
 * AsyncWaitHandler.h
 *
 *  Created on: Dec 2, 2012
 *      Author: lion
 */

#ifndef ASYNCWAITHANDLER_H_
#define ASYNCWAITHANDLER_H_

#include "ifs/AsyncWait.h"
#include "Event.h"

namespace fibjs {

class AsyncWaitHandler : public AsyncWait_base {

    FIBER_FREE();

public:
    class asyncWaiter : public AsyncEvent {
    public:
        asyncWaiter(AsyncEvent* ac)
            : m_ac(ac)
        {
        }

    public:
        virtual void invoke()
        {
            m_ac->post(CALL_RETURN_NULL);
            delete this;
        }

    private:
        AsyncEvent* m_ac;
    };

public:
    AsyncWaitHandler()
        : m_as(NULL)
        , m_stat(0)
        , m_invoked(0)
    {
    }

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    // AsyncWait_base
    virtual result_t end();

private:
    asyncWaiter* m_as;
    exlib::atomic m_stat;
    exlib::atomic m_invoked;
};

} /* namespace fibjs */
#endif /* ASYNCWAITHANDLER_H_ */
