/*
 * Event.h
 *
 *  Created on: Apr 30, 2012
 *      Author: lion
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "ifs/Event.h"

namespace fibjs
{

class Event: public Event_base
{

	FIBER_FREE();

public:
	// Lock_base
	virtual result_t acquire(bool blocking, bool& retVal);
	virtual result_t release();

public:
	// Event_base
	virtual result_t isSet(bool& retVal);
	virtual result_t set();
	virtual result_t pulse();
	virtual result_t clear();
	virtual result_t wait();

private:
	exlib::Event m_event;
};

}

#endif /* EVENT_H_ */
