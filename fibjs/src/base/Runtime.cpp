/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "Fiber.h"

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

Isolate &Isolate::now()
{
    trace_assert(exlib::Service::hasService());

    if (exlib::Service::hasService())
    {
        return *(Isolate *)th_rt;
    }

    return *(Isolate *)NULL;
}

void Isolate::reg(void *rt)
{
    trace_assert(exlib::Service::hasService());

    Runtime::reg(rt);
}

bool Isolate::rt::g_trace = false;

inline JSFiber* saveTrace()
{
    JSFiber* fiber = JSFiber::current();
    trace_assert(fiber != 0);
    fiber->m_traceInfo = traceInfo(300);
    return fiber;
}

Isolate::rt::rt() :
    m_fiber(g_trace ? saveTrace() : NULL),
    unlocker(Isolate::now().isolate)
{
}

Isolate::rt::~rt()
{
    if (m_fiber)
        m_fiber->m_traceInfo.resize(0);
}

} /* namespace fibjs */

