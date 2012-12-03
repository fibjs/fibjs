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

namespace fibjs
{

class AsyncWaitHandler: public AsyncWait_base
{

	FIBER_FREE();

public:
	class asyncWaiter: public AsyncCall
	{
	public:
		asyncWaiter(exlib::AsyncEvent* ac) :
			AsyncCall(NULL), m_ac(ac)
		{}

	public:
		virtual void invoke()
		{
			m_ac->post(CALL_RETURN_NULL);
		}

	private:
		exlib::AsyncEvent* m_ac;
	};

public:
	AsyncWaitHandler() : m_ac(NULL), m_as(NULL)
	{}

public:
	// Handler_base
	virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
			exlib::AsyncEvent* ac);

public:
	// AsyncWait_base
	virtual result_t end();

private:
	AsyncCall m_ac;
	asyncWaiter* m_as;
};

} /* namespace fibjs */
#endif /* ASYNCWAITHANDLER_H_ */
