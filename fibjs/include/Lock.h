/*
 * Locker.h
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#ifndef LOCKER_H_
#define LOCKER_H_

#include "ifs/Lock.h"

namespace fibjs
{

class Lock: public Lock_base
{
    FIBER_FREE();

public:
    virtual result_t acquire(bool blocking, bool &retVal);
    virtual result_t release();
};

}

#endif /* LOCKER_H_ */
