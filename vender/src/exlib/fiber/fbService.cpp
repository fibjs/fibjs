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
#include <string.h>
#include <stdlib.h>

#ifdef MacOS
#include <sys/sysctl.h>
#endif

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

static bool s_bRootThread = true;

#ifdef MacOS

static pthread_key_t keyService;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static intptr_t kMacTlsBaseOffset = -1;

intptr_t MacTlsBaseOffset()
{
	if (kMacTlsBaseOffset == -1)
	{
		const size_t kBufferSize = 128;
		char buffer[kBufferSize];
		size_t buffer_size = kBufferSize;
		int ctl_name[] =
		{ CTL_KERN, KERN_OSRELEASE };
		sysctl(ctl_name, 2, buffer, &buffer_size, NULL, 0);
		buffer[kBufferSize - 1] = '\0';
		char* period_pos = strchr(buffer, '.');
		*period_pos = '\0';
		int kernel_version_major = static_cast<int>(strtol(buffer, NULL, 10));

		if (kernel_version_major < 11)
		{
#if defined(I386)
			kMacTlsBaseOffset = 0x48;
#else
			kMacTlsBaseOffset = 0x60;
#endif
		}
		else
		{
			kMacTlsBaseOffset = 0;
		}
	}

	return kMacTlsBaseOffset;
}

inline intptr_t FastThreadLocal(intptr_t index)
{
	intptr_t result;
#if defined(I386)
	asm("movl %%gs:(%1,%2,4), %0;"
			:"=r"(result)
			:"r"(kMacTlsBaseOffset), "r"(index));
#else
	asm("movq %%gs:(%1,%2,8), %0;"
			:"=r"(result)
			:"r"(kMacTlsBaseOffset), "r"(index));
#endif
	return result;
}

static void once_run(void)
{
	MacTlsBaseOffset();
	pthread_key_create(&keyService, NULL);
}

Service* Service::getFiberService()
{
	pthread_once(&once, once_run);

	Service* pService = (Service*) FastThreadLocal(keyService);
//	Service* pService = (Service*) pthread_getspecific(keyService);

	if (pService == NULL && s_bRootThread)
	{
		s_bRootThread = false;
		pService = new Service();
		pthread_setspecific(keyService, pService);
	}

	return pService;
}

void Service::init()
{
	Service* pService = (Service*) FastThreadLocal(keyService);
//	Service* pService = (Service*) pthread_getspecific(keyService);

	if (pService == NULL)
	{
		pService = new Service();
		pthread_setspecific(keyService, pService);
	}
}

bool Service::hasService()
{
	return !!FastThreadLocal(keyService);
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

	if(pService == NULL && s_bRootThread)
	{
		s_bRootThread = false;
		pService = new Service();
		th_Service = pService;
	}

	return pService;
}

void Service::init()
{
	Service* pService = th_Service;

	if(pService == NULL)
	{
		pService = new Service();
		th_Service = pService;
	}
}

bool Service::hasService()
{
	return !!th_Service;
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

	if (pService)
	{
		Fiber* now = pService->m_running;

		if (now->m_join)
			pService->m_resume.put(now->m_join);

		pService->m_recycle = now;
		pService->switchtonext();
	}
}

Fiber* Service::CreateFiber(void* (*func)(void *), void *data, int stacksize)
{
	Service* pService = Service::getFiberService();

	if (pService)
	{
		Fiber* fb;
		void** stack;

		stacksize = (stacksize + FB_STK_ALIGN - 1) & ~(FB_STK_ALIGN - 1);
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

	return NULL;
}

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

			p->callback();
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
			AsyncEvent* p = m_yieldList.get();
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
	Service* pService = Service::getFiberService();

	if (pService)
	{
		AsyncEvent ae(pService);

		pService->m_yieldList.put(&ae);
		ae.wait();
	}
}

}
