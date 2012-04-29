/*
 * Event.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "ifs/EventObject.h"

namespace fibjs
{

class Event: public EventObject_base
{
	EVENT_SUPPORT();
};

}

#endif /* EVENT_H_ */
