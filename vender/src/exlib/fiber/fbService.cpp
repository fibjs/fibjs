/*
 *  fbService.cpp
 *  Created on: Mar 19, 2012
 *
 *  Copyright (c) 2012 by Leo Hoo
 *  lion@9465.net
 */

#include <osconfig.h>
#include <exlib/fiber.h>
#include <exlib/thread.h>

namespace exlib
{

#define FB_STK_ALIGN 256

#ifdef _WIN32
extern "C" int win_switch(context* from, context* to);
#define fb_switch win_switch
#else
extern "C" int nix_switch(context* from, context* to);
#define fb_switch nix_switch
#endif

#ifdef MacOS

#include <pthread.h>

static pthread_key_t keyService;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static void once_run(void)
{
	pthread_key_create(&keyService, NULL);
}

Service* Service::getFiberService()
{
	pthread_once(&once, once_run);

	Service* pService = (Service*) pthread_getspecific(keyService);

	if (pService == NULL)
	{
		pService = new Service();
		pthread_setspecific(keyService, pService);
	}

	return pService;
}

#else

#if defined(_MSC_VER)
__declspec(thread) Service* th_Service = NULL;
#else
__thread Service* th_Service = NULL;
#endif

Service* Service::getFiberService()
{
	Service* pService = th_Service;

	if(pService == NULL)
	{
		pService = new Service();
		th_Service = pService;
	}

	return pService;
}

#endif

Service::Service()
{
	m_recycle = NULL;
	m_running = &m_main;
	m_Idle = NULL;
	memset(&m_main, 0, sizeof(m_main));
	memset(&m_tls, 0, sizeof(m_tls));
}

static void fiber_proc(void* (*func)(void *), void *data)
{
	func(data);

	Service* pService = Service::getFiberService();

	Fiber* now = pService->m_running;

	if (now->m_join)
		pService->m_resume.put(now->m_join);

	pService->m_recycle = now;
	pService->switchtonext();
}

Fiber* Service::CreateFiber(void* (*func)(void *), void *data, int stacksize)
{
	Service* pService = Service::getFiberService();
	Fiber* fb;
	void** stack;

	stacksize = (stacksize + sizeof(Fiber) + FB_STK_ALIGN - 1)
			& ~(FB_STK_ALIGN - 1);
	fb = (Fiber*) malloc(stacksize);
	if (fb == NULL)
		return NULL;
	stack = (void**) fb + stacksize / sizeof(void*) - 5;

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

#else
	fb->m_cntxt.Eip = (unsigned long) fiber_proc;
	fb->m_cntxt.Esp = (unsigned long) stack;

	stack[1] = (void*)func;
	stack[2] = data;
#endif

	pService->m_resume.put(fb);

	fb->Ref();
	fb->Ref();

	return fb;
}

static List<AsyncEvent> s_yieldList;

void Service::switchtonext()
{
	while (1)
	{
		// First switch if we have work to do.
		if (!m_resume.empty())
		{
			Fiber* old = m_running;

			m_running = m_resume.get();

			fb_switch(&old->m_cntxt, &m_running->m_cntxt);

			if (m_recycle)
			{
				m_recycle->Unref();
				m_recycle = NULL;
			}
			break;
		}

		// Then weakup async event.
		while (1)
		{
			AsyncEvent* p = m_aEvents.get();
			if (p == NULL)
				break;

			p->set();
		}

		if (!m_resume.empty())
			continue;

		// doing smoething when we have time.
		if (m_Idle)
			m_Idle();

		if (!m_resume.empty())
			continue;

		// if we still have time, weakup yield fiber.
		while (1)
		{
			AsyncEvent* p = s_yieldList.get();
			if (p == NULL)
				break;

			p->set();
		}

		if (!m_resume.empty())
			continue;

		// still no work, we wait, and wait, and wait.....
		m_aEvents.wait()->set();
	}
}

void Service::yield()
{
	AsyncEvent ae;

	s_yieldList.put(&ae);
	ae.wait();
}

}
