/*
 *  fbService.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <string.h>
#include <stdlib.h>

#include "osconfig.h"
#include "service.h"
#include "thread.h"

namespace exlib
{

#define FB_STK_ALIGN 256

#ifdef _WIN32
extern "C" int win_switch(context *from, context *to);
#define fb_switch win_switch
#else
extern "C" int nix_switch(context *from, context *to);
#define fb_switch nix_switch
#endif

Service *Service::root;

#ifdef _WIN32

static DWORD s_main;

Service *Service::getFiberService()
{
    if (!s_main)
    {
        root = new Service();
        s_main = GetCurrentThreadId();
    }
    else if (s_main != GetCurrentThreadId())
        return NULL;

    return root;
}

bool Service::hasService()
{
    return s_main == GetCurrentThreadId();
}

#else

static pthread_t s_main;

Service *Service::getFiberService()
{
    if (!s_main)
    {
        root = new Service();
        s_main = pthread_self();
    }
    else if (s_main != pthread_self())
        return NULL;

    return root;
}

bool Service::hasService()
{
    return s_main == pthread_self();
}

#endif

static class _service_init
{
public:
    _service_init()
    {
        exlib::Service::getFiberService();
    }
} s_service_init;

Service::Service()
{
    m_recycle = NULL;
    m_running = &m_main;
    m_Idle = NULL;
    memset(&m_main, 0, sizeof(m_main));
    memset(&m_tls, 0, sizeof(m_tls));
}

static void fiber_proc(void *(*func)(void *), void *data)
{
    func(data);

    Service::root->m_recycle = Service::root->m_running;
    Service::root->switchConext();
}

Fiber *Fiber::Create(void *(*func)(void *), void *data, int stacksize)
{
    Fiber *fb;
    void **stack;

    stacksize = (stacksize + FB_STK_ALIGN - 1) & ~(FB_STK_ALIGN - 1);
#ifdef WIN32
    fb = (Fiber *) VirtualAlloc(NULL, stacksize, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
#else
    fb = (Fiber *) malloc(stacksize);
#endif
    if (fb == NULL)
        return NULL;
    stack = (void **) fb + stacksize / sizeof(void *) - 5;

    memset(fb, 0, sizeof(Fiber));

#if defined(x64)
    fb->m_cntxt.Rip = (unsigned long long) fiber_proc;
    fb->m_cntxt.Rsp = (unsigned long long) stack;

#ifdef _WIN32
    fb->m_cntxt.Rcx = (unsigned long long) func;
    fb->m_cntxt.Rdx = (unsigned long long) data;
#else
    fb->m_cntxt.Rdi = (unsigned long long) func;
    fb->m_cntxt.Rsi = (unsigned long long) data;
#endif

#elif defined(I386)
    fb->m_cntxt.Eip = (unsigned long) fiber_proc;
    fb->m_cntxt.Esp = (unsigned long) stack;

    stack[1] = (void *)func;
    stack[2] = data;

#elif defined(arm)
    fb->m_cntxt.r14 = (unsigned long) fiber_proc;
    fb->m_cntxt.r13 = (unsigned long) stack;

    fb->m_cntxt.r0 = (unsigned long) func;
    fb->m_cntxt.r1 = (unsigned long) data;
#endif

    Service::root->m_resume.put(fb);

    fb->Ref();
    fb->Ref();

    return fb;
}

void Service::switchConext()
{
    while (1)
    {
        // First switch if we have work to do.
        if (!m_resume.empty())
        {
            Fiber *old = m_running;

            m_running = m_resume.get();

            fb_switch(&old->m_cntxt, &m_running->m_cntxt);

            if (m_recycle)
            {
                m_recycle->m_joins.set();
                m_recycle->Unref();
                m_recycle = NULL;
            }
            break;
        }

        // Then wakeup async event.
        while (1)
        {
            AsyncEvent *p = m_aEvents.get();
            if (p == NULL)
                break;

            p->callback();
        }

        if (!m_resume.empty())
            continue;

        // doing smoething when we have time.
        if (m_Idle)
            m_Idle();

        if (!m_resume.empty())
            continue;

        // if we still have time, wakeup yield fiber.
        while (1)
        {
            AsyncEvent *p = m_yieldList.get();
            if (p == NULL)
                break;

            p->callback();
        }

        if (!m_resume.empty())
            continue;

        // still no work, we wait, and wait, and wait.....
        m_aEvents.wait()->callback();
    }
}

void Service::yield()
{
    AsyncEvent ae;
    m_yieldList.put(&ae);
    ae.wait();
}

}
