/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "Fiber.h"
#include "SandBox.h"

namespace fibjs
{

OSTls th_rt;

void Runtime::reg(void *rt)
{
    th_rt = rt;
}

Runtime &Runtime::now()
{
    if (exlib::Service::hasService())
        return JSFiber::current()->runtime();

    return *(Runtime *)th_rt;
}

} /* namespace fibjs */

