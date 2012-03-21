/*
 *  fbService.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <fiber.h>
#include <stdio.h>
#include <stdlib.h>

namespace fiber
{

#define FB_STK_ALIGN 256

#ifdef _WIN32

extern "C" int win_setjmp(context* _Buf);
extern "C" int win_longjmp(context* _Buf, int _Value);

#define fb_setjmp win_setjmp
#define fb_longjmp win_longjmp

static __declspec(thread) Service* th_Service;

Service* Service::getTLSService()
{
    Service* pService = th_Service;

    if(pService == NULL)
    {
        pService = new Service();
        th_Service = pService;
    }

    return pService;
}

#else

extern "C" int nix_setjmp(context* _Buf);
extern "C" int nix_longjmp(context* _Buf, int _Value);

#define fb_setjmp nix_setjmp
#define fb_longjmp nix_longjmp

#include <pthread.h>

static pthread_key_t keyService;
static int key_inited = 0;

Service* Service::getTLSService()
{
    if(!key_inited)
    {
        key_inited = 1;
        pthread_key_create(&keyService, NULL);
    }

    Service* pService = (Service*)pthread_getspecific(keyService);

    if(pService == NULL)
    {
        pService = new Service();
        pthread_setspecific(keyService, pService);
    }

    return pService;
}

#endif

Service::Service()
{
    m_running = &m_main;
    memset(&m_main, 0, sizeof(m_main));
    memset(&m_tls, 0, sizeof(m_tls));
}

int Service::switchto(Fiber* to, int state)
{
    Fiber* old = m_running;
    m_running = to;

    int r = fb_setjmp(&old->m_cntxt);
    if(r == 0)
        fb_longjmp(&to->m_cntxt, state);

    return r;
}

static void fiber_proc(void* (*func)(void *), void *data)
{
    func(data);

    Service* pService = Service::getTLSService();
    pService->switchto(&pService->m_main, FB_TERMINATE);
}

Fiber* Service::CreateFiber(void* (*func)(void *), void *data, int stacksize)
{
    Service* pService = Service::getTLSService();
    Fiber* fb;
    void** stack;

    stacksize = (stacksize+ sizeof(Fiber) + FB_STK_ALIGN - 1) & ~(FB_STK_ALIGN - 1);
    fb = (Fiber*)malloc(stacksize);
    if (fb == NULL)
        return NULL;
    stack = (void**)fb + stacksize / sizeof(void*) - 3;

    memset(fb, 0, sizeof(Fiber));

#if defined(__x86_64__)
    fb->m_cntxt.Rip = (unsigned long long) fiber_proc;
    fb->m_cntxt.Rsp = (unsigned long long) stack;

#ifdef _WIN32
    fb->m_cntxt.Rcx = (unsigned long long) func;
    fb->m_cntxt.Rdx = (unsigned long long) data;
#else
    fb->m_cntxt.Rdi = (unsigned long long) func;
    fb->m_cntxt.Rsi = (unsigned long long) data;
#endif

#else
    fb->m_cntxt.Eip = (unsigned long) fiber_proc;
    fb->m_cntxt.Esp = (unsigned long) stack;

    stack[1] = func;
    stack[2] = data;
#endif

    pService->m_resume.push_back(fb);

    return fb;
}

void Service::Run()
{
    Service* pService = Service::getTLSService();
    Fiber *now, *cntxt_join;
    int state;

    while(!pService->m_resume.empty())
    {
        now = pService->m_resume.front();
        pService->m_resume.pop_front();

        state = pService->switchto(now, FB_RESUME);

        pService->m_running = &pService->m_main;

        if(state == FB_TERMINATE)
        {
            cntxt_join = now->m_join;
            free(now);

            if(cntxt_join)
                pService->m_resume.push_back(cntxt_join);
        }
    }
}

Fiber* Service::Current()
{
    return Service::getTLSService()->m_running;
}

void Service::Join(Fiber* fb)
{
    Service* pService = Service::getTLSService();

    fb->m_join = pService->m_running;

    pService->switchto(&pService->m_main, FB_SUSPEND);
}

void Service::Suspend()
{
    Service* pService = Service::getTLSService();

    pService->m_resume.push_back(pService->m_running);
    pService->switchto(&pService->m_main, FB_SUSPEND);
}

}
