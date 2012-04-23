
#include "ifs/os.h"
#include <exlib/thread.h>
#include <acPool.h>
#include <map>
#include <log4cpp/Category.hh>

#ifdef _WIN32
#include <windows.h>

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

static class _acThread: public exlib::Thread
{
public:
	_acThread()
	{
		int32_t cpus;
		if (os_base::CPUs(cpus) < 3)
			cpus = 3;

		for (int i = 0; i < cpus * 2 - 2; i++)
		{
			start();
			detach();
		}
	}

	virtual void Run()
	{
		AsyncCall *p;

		while (1)
		{
			p = s_acPool.wait();
			p->func(p);
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
		exlib::Thread::Sleep(1);
}

static class _timerThread: public exlib::Thread
{
public:
	_timerThread()
	{
		s_time = Ticks();
		start();
	}

	virtual void Run()
	{
		AsyncCall *p;
		int64_t tm;
		std::multimap<int64_t, AsyncCall*>::iterator e;
		AsyncLog *p1, *p2, *pn;

		while (1)
		{
			s_logEmpty = false;
			p1 = s_acLog.getList();
			if (p1)
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
			}
			else
			{
				s_logEmpty = true;
				Sleep(1);
			}

			while (1)
			{
				p = s_acSleep.get();
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

				e->second->post();
				s_tms.erase(e);
			}
		}
	}
} s_timer;

}
