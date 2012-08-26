#include "ifs/os.h"
#include <log4cpp/Category.hh>
#include <exlib/thread.h>

#ifdef _WIN32

static LARGE_INTEGER systemFrequency;

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

namespace fibjs
{

AsyncQueue s_acPool;

static int32_t s_threads;
static int32_t s_idleThreads;
static int32_t s_idleCount;

static class _acThread: public exlib::OSThread
{
public:
	_acThread()
	{
		int32_t cpus;
		if (os_base::CPUs(cpus) < 3)
			cpus = 3;

		s_threads = cpus * 2;

		for (int i = 0; i < s_threads; i++)
		{
			start();
			detach();
		}
	}

	virtual void Run()
	{
		asyncEvent *p;

		while (1)
		{
			if (exlib::atom_inc(&s_idleThreads) > s_threads * 2)
			{
				exlib::atom_dec(&s_idleThreads);
				break;
			}

			p = s_acPool.wait();
			exlib::atom_dec(&s_idleThreads);

			p->invoke();
		}
	}
} s_ac;

static AsyncLogQueue s_acLog;
AsyncQueue s_acSleep;
std::multimap<int64_t, AsyncCall*> s_tms;
static int64_t s_time;
static bool s_logEmpty;

void asyncLog(int priority, std::string msg)
{
//	log4cpp::Category::getRoot().log(priority, msg);
	s_acLog.put(new AsyncLog(priority, msg));
}

void flushLog()
{
	while (!s_acLog.empty() || !s_logEmpty)
		exlib::OSThread::Sleep(1);
}

static class _loggerThread: public exlib::OSThread
{
public:
	_loggerThread()
	{
		start();
	}

	virtual void Run()
	{
		std::multimap<int64_t, AsyncCall*>::iterator e;
		AsyncLog *p1, *p2, *pn;

		while (1)
		{
			s_logEmpty = false;

			p1 = s_acLog.getList();
			while (p1)
			{
				log4cpp::Category& root = log4cpp::Category::getRoot();
				pn = NULL;

				while (p1)
				{
					p2 = p1;
					p1 = (AsyncLog*) p1->m_next;
					p2->m_next = pn;
					pn = p2;
				}

				while (pn)
				{
					p1 = pn;
					pn = (AsyncLog*) pn->m_next;
					root.log(p1->m_priority, p1->m_msg);
					delete p1;
				}

				p1 = s_acLog.getList();
			}

			s_logEmpty = true;
			Sleep(100);
		}
	}
} s_logger;

#ifndef _WIN32
#define PASCAL
#define DWORD_PTR unsigned long*
#endif

#ifdef _WIN32
static int s_nTimer;
void clearTimer()
{
	timeKillEvent(s_nTimer);
}
#else
void clearTimer()
{
}
#endif

static class _timerThread: public exlib::OSThread
{
public:
	_timerThread()
	{
		s_time = Ticks();
		start();
	}

	static void PASCAL Timer(unsigned int uTimerID, unsigned int uMsg,
			DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
	{
		AsyncCall *p;
		int64_t tm;
		std::multimap<int64_t, AsyncCall*>::iterator e;

		while (1)
		{
			p = (AsyncCall*) s_acSleep.get();
			if (p == NULL)
				break;

			tm = s_time + *(int*) p->args[0] * 1000;
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

		if (s_idleThreads < s_threads)
		{
			if (++s_idleCount > 50)
			{
				s_idleCount = 0;

				for (int i = s_idleThreads; i < s_threads; i++)
				{
					s_ac.start();
					s_ac.detach();
				}
			}
		}
		else
			s_idleCount = 0;
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

}
