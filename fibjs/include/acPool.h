/*
 * acPool.h
 *
 *  Created on: Apr 6, 2012
 *      Author: lion
 */

#ifndef ACPOOL_H_
#define ACPOOL_H_

#include "AsyncCall.h"

namespace fibjs
{

extern AsyncQueue s_acPool;
extern AsyncQueue s_acSleep;
void asyncLog(int priority, std::string msg);

}

#endif /* ACPOOL_H_ */
