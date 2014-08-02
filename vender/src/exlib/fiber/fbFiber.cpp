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
#include <fcntl.h>
#include <time.h>

#ifdef _WIN32
#define WM_SLEEP    (WM_USER + 1)
#else
#include <map>
#endif

namespace v8
{
namespace base
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

#ifndef _WIN32
exlib::lockfree<AsyncEvent> s_acSleep;
std::multimap<double, AsyncEvent *> s_tms;
#endif


Fiber *Fiber::Current()
{
    Service *pService = Service::getFiberService();

    if (pService)
        return pService->m_running;

    return NULL;
}

void Fiber::yield()
{
    Service *pService = Service::getFiberService();

    if (pService)
        pService->yield();
}

void Fiber::destroy()
{
#ifdef WIN32
    VirtualFree(this, 0, MEM_RELEASE);
#else
    free(this);
#endif
}

static class _timerThread: public OSThread
{
public:
    _timerThread()
    {
        start();
    }

#ifdef _WIN32
    static void PASCAL Timer(unsigned int uTimerID, unsigned int uMsg,
                             DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
    {
        timeKillEvent(uTimerID);

        AsyncEvent *p = (AsyncEvent *)dwUser;

        p->apost(0);
    }

    virtual void Run()
    {
        TIMECAPS tc;

        timeGetDevCaps(&tc, sizeof(TIMECAPS));

        if (tc.wPeriodMin < 1)
            tc.wPeriodMin = 1;

        timeBeginPeriod(tc.wPeriodMin);

        MSG msg;
        while (GetMessage(&msg, 0, 0, 0))
            if (msg.message == WM_SLEEP)
            {
                UINT tm;
                AsyncEvent *p = (AsyncEvent *)msg.lParam;

                tm = (UINT)p->result();
                if (tm < tc.wPeriodMin)
                    tm = tc.wPeriodMin;

                timeSetEvent(tm, tc.wPeriodMin, Timer, (DWORD_PTR)p, TIME_ONESHOT);
            }
    }
#else
    OSSemaphore m_sem;
    double now;

#ifdef MacOS
    void wait()
    {
        std::multimap<double, AsyncEvent *>::iterator e;

        e = s_tms.begin();
        if (e != s_tms.end())
        {
            mach_timespec_t mts;
            int tm = e->first - now;

            mts.tv_sec = tm / 1000;
            mts.tv_nsec = (tm % 1000) * 1000000;

            semaphore_timedwait(m_sem.m_sem, mts);
        }
        else
            m_sem.Wait();
    }

#else
    void wait()
    {
        std::multimap<double, AsyncEvent *>::iterator e;

        e = s_tms.begin();
        if (e != s_tms.end())
        {
            struct timespec tm;

            tm.tv_sec = (time_t)(e->first / 1000);
            tm.tv_nsec = (e->first - (double)tm.tv_sec * 1000) * 1000000;

            sem_timedwait(&m_sem.m_sem, &tm);
        }
        else
            m_sem.Wait();
    }

#endif

    virtual void Run()
    {
        now = v8::base::OS::TimeCurrentMillis();

        while (1)
        {
            AsyncEvent *p;
            std::multimap<double, AsyncEvent *>::iterator e;

            wait();

            now = v8::base::OS::TimeCurrentMillis();

            while (1)
            {
                p = s_acSleep.get();
                if (p == NULL)
                    break;

                s_tms.insert(std::make_pair(now + p->result(), p));
            }

            while (1)
            {
                e = s_tms.begin();
                if (e == s_tms.end())
                    break;
                if (e->first > now)
                    break;

                e->second->apost(0);
                s_tms.erase(e);
            }
        }
    }

#endif

    static void post(AsyncEvent *p);

} s_timer;

void _timerThread::post(AsyncEvent *p)
{
#ifdef _WIN32
    PostThreadMessage(s_timer.threadid, WM_SLEEP, 0, (LPARAM)p);
#else
    s_acSleep.put(p);
    s_timer.m_sem.Post();
#endif
}

void AsyncEvent::sleep(int ms)
{
    m_v = ms;
    _timerThread::post(this);
}

void Fiber::sleep(int ms)
{
    if (Service::hasService())
    {
        if (ms <= 0)
            yield();
        else
        {
            AsyncEvent as;

            as.sleep(ms);
            as.wait();
        }
    }
    else
        OSThread::Sleep(ms);
}

}
