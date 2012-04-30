/*
 * Event.cpp
 *
 *  Created on: Apr 30, 2012
 *      Author: lion
 */

#include "Event.h"

namespace fibjs
{

result_t Event_base::_new(bool value, obj_ptr<Event_base>& retVal)
{
	retVal = new Event();
	if (value)
		retVal->set();

	return 0;
}

result_t Event::acquire(bool blocking, bool& retVal)
{
	if(!blocking)
	{
		retVal = m_event.isset();
		return 0;
	}

	retVal = true;
	return wait();
}

result_t Event::release()
{
	m_event.set();
	return 0;
}

result_t Event::isSet(bool& retVal)
{
	retVal = m_event.isset();
	return 0;
}

result_t Event::set()
{
	m_event.set();
	return 0;
}

result_t Event::pulse()
{
	m_event.pulse();
	return 0;
}

result_t Event::clear()
{
	m_event.reset();
	return 0;
}

result_t Event::wait()
{
	if (!m_event.isset())
		return 0;

	v8::Unlocker unlocker(isolate);
	m_event.wait();
	return 0;
}

}
