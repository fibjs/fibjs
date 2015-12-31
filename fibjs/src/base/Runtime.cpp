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

Isolate::Isolate(const char *fname) : m_test_setup_bbd(false), m_test_setup_tdd(false),
    m_oldIdle(NULL), m_currentFibers(0), m_idleFibers(0)
{
    if (fname)
        m_fname = fname;
}

Isolate *Isolate::current()
{
    OSThread* thread_ = OSThread::current();

    assert(thread_ != 0);
    assert(thread_->is(Isolate::type));

    return (Isolate*)thread_;
}

bool Isolate::check()
{
    assert(OSThread::current() != 0);
    return OSThread::current()->is(Isolate::type);
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
    unlocker(Isolate::current()->m_isolate)
{
}

Isolate::rt::~rt()
{
    if (m_fiber)
        m_fiber->m_traceInfo.resize(0);
}

} /* namespace fibjs */

