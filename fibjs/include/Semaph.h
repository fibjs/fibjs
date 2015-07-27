/*
 * Semaphore.h
 *
 *  Created on: Apr 25, 2012
 *      Author: lion
 */

#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "ifs/Semaphore.h"

namespace fibjs
{

class Semaphore: public Semaphore_base
{
public:
    Semaphore(int32_t count) :
        m_sem(count)
    {
    }

    FIBER_FREE();

public:
    // Lock_base
    virtual result_t acquire(bool blocking, bool &retVal);
    virtual result_t release();

public:
    // Semaphore_base
    virtual result_t wait();
    virtual result_t post();
    virtual result_t trywait(bool &retVal);

private:
    exlib::Semaphore m_sem;
};

}

#endif /* SEMAPHORE_H_ */
