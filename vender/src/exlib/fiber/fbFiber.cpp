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
#include <stdio.h>
#include <stdlib.h>
#include <map>

namespace v8
{
namespace internal
{

class OS
{
public:
	static double TimeCurrentMillis();
};

}
}

namespace exlib
{

Fiber* Fiber::Current()
{
	Service* pService = Service::getFiberService();

	if (pService)
		return pService->m_running;

	return NULL;
}

void Fiber::join()
{
	Service* pService = Service::getFiberService();

	if (pService)
	{
		m_join = pService->m_running;
		pService->switchtonext();
	}
}

void Fiber::yield()
{
	Service* pService = Service::getFiberService();

	if (pService)
		pService->yield();
}

void Fiber::destroy()
{
	free(this);
}

exlib::lockfree<AsyncEvent> s_acSleep;
std::multimap<double, AsyncEvent*> s_tms;
static double s_time;

#ifndef _WIN32
#define PASCAL
#define DWORD_PTR unsigned long*
#else
static int s_nTimer;
#endif

double FastCurrentMillis()
{
	return s_time;
}

static class _timerThread: public OSThread
{
public:
	_timerThread()
	{
		s_time = v8::internal::OS::TimeCurrentMillis();
		start();
	}

#ifdef _WIN32
	~_timerThread()
	{
		timeKillEvent(s_nTimer);
	}
#endif

	static void PASCAL Timer(unsigned int uTimerID, unsigned int uMsg,
			DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
	{
		AsyncEvent *p;
		double tm;
		std::multimap<double, AsyncEvent*>::iterator e;

		while (1)
		{
			p = s_acSleep.get();
			if (p == NULL)
				break;

			tm = s_time + p->result();
			s_tms.insert(std::make_pair(tm, p));
		}

		s_time = v8::internal::OS::TimeCurrentMillis();

		while (1)
		{
			e = s_tms.begin();
			if (e == s_tms.end())
				break;
			if (e->first > s_time)
				break;

			e->second->apost(0);
			s_tms.erase(e);
		}
	}

	virtual void Run()
	{
#ifdef _WIN32
		TIMECAPS tc;

		timeGetDevCaps(&tc, sizeof(TIMECAPS));

		if (tc.wPeriodMin < 1)
		tc.wPeriodMin = 1;

		timeBeginPeriod(tc.wPeriodMin);
		s_nTimer = timeSetEvent(tc.wPeriodMin, tc.wPeriodMin, Timer, 0, TIME_PERIODIC);

		MSG msg;
		while (GetMessage(&msg, 0, 0, 0));
#else
		while (1)
		{
			Sleep(1);
			Timer(0, 0, NULL, NULL, NULL);
		}
#endif
	}
} s_timer;

void AsyncEvent::sleep(int ms)
{
	m_v = ms;
	s_acSleep.put(this);
}

void Fiber::sleep(int ms)
{
	Service* pService = Service::getFiberService();

	if (pService)
	{
		if (ms <= 0)
			yield();
		else
		{
			AsyncEvent as(pService);

			as.sleep(ms);
			as.wait();
		}
	}
	else
		OSThread::Sleep(ms);
}

}
