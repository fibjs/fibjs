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

#ifdef _WIN32
#include <windows.h>

static LARGE_INTEGER systemFrequency;

#ifndef int64_t
typedef __int64 int64_t;
#endif

inline int64_t Ticks()
{
	LARGE_INTEGER t;

	if(systemFrequency.QuadPart == 0)
	QueryPerformanceFrequency(&systemFrequency);

	QueryPerformanceCounter(&t);

	return t.QuadPart * 1000000 / systemFrequency.QuadPart;
}

#else
#include <sys/time.h>

inline int64_t Ticks()
{
	struct timeval tv;
	if (gettimeofday(&tv, NULL) < 0)
		return 0;
	return (tv.tv_sec * 1000000ll) + tv.tv_usec;
}

#endif

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

class AsyncSleep: public AsyncEvent
{
public:
	AsyncSleep(Service* pService, int tm) :
			AsyncEvent(pService), m_tm(tm)
	{
	}

	int wait()
	{
		return AsyncEvent::wait();
	}

public:
	int m_tm;
};

exlib::lockfree<AsyncSleep> s_acSleep;
std::multimap<int64_t, AsyncSleep*> s_tms;
static int64_t s_time;

#ifndef _WIN32
#define PASCAL
#define DWORD_PTR unsigned long*
#else
static int s_nTimer;
#endif

static class _timerThread: public OSThread
{
public:
	_timerThread()
	{
		s_time = Ticks();
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
		AsyncSleep *p;
		int64_t tm;
		std::multimap<int64_t, AsyncSleep*>::iterator e;

		while (1)
		{
			p = s_acSleep.get();
			if (p == NULL)
				break;

			tm = s_time + p->m_tm * 1000;
			s_tms.insert(std::make_pair(tm, p));
		}

		s_time = Ticks();

		while (1)
		{
			e = s_tms.begin();
			if (e == s_tms.end())
				break;
			if (e->first > s_time)
				break;

			e->second->post(0);
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

void Fiber::sleep(int ms)
{
	Service* pService = Service::getFiberService();

	if (pService)
	{
		if (ms <= 0)
			yield();
		else
		{
			AsyncSleep as(pService, ms);
			s_acSleep.put(&as);
			as.wait();

		}
	}
	else
		OSThread::Sleep(ms);
}

}
