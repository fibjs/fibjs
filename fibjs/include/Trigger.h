/*
 * Event.h
 *
 *  Created on: Apr 8, 2012
 *      Author: lion
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "ifs/Trigger.h"

namespace fibjs
{

class Trigger: public Trigger_base
{
    EVENT_SUPPORT();
};

}

#endif /* EVENT_H_ */
