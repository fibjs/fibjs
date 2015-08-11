/*
 * Runtime.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Runtime.h"
#include "Fiber.h"
#include "SandBox.h"
#include <assert.h>

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

OSTls th_vm;

Isolate::Isolate() : m_test_setup_bbd(false), m_test_setup_tdd(false)
{
}

Isolate* Isolate::now()
{
    assert(exlib::Service::hasService());
    return (Isolate *)th_vm;
}

void Isolate::reg(void *rt)
{
    assert(exlib::Service::hasService());
    th_vm = rt;
}

bool Isolate::rt::g_trace = false;

inline JSFiber* saveTrace()
{
    JSFiber* fiber = JSFiber::current();
    assert(fiber != 0);
    fiber->m_traceInfo = traceInfo(300);
    return fiber;
}

Isolate::rt::rt() :
    m_fiber(g_trace ? saveTrace() : NULL),
    unlocker(Isolate::now()->m_isolate)
{
}

Isolate::rt::~rt()
{
    if (m_fiber)
        m_fiber->m_traceInfo.resize(0);
}

} /* namespace fibjs */

