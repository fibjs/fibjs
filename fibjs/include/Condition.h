/*
 * Condition.h
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#include "ifs/Condition.h"
#include "Lock.h"

#ifndef CONDITION_H_
#define CONDITION_H_

namespace fibjs
{

class Condition: public Condition_base
{
public:
	Condition(Lock_base* lock) :
		m_lockCond(lock)
	{
	}

	Condition()
	{
		m_lockCond = new Lock();
	}

	FIBER_FREE();

public:
	// Lock_base
	virtual result_t acquire(bool blocking, bool& retVal);
	virtual result_t release();

public:
	// Condition_base
	virtual result_t wait();
	virtual result_t notify();
	virtual result_t notifyAll();

private:
	obj_ptr<Lock_base> m_lockCond;
	exlib::CondVar m_cond;
};

}

#endif /* CONDITION_H_ */
