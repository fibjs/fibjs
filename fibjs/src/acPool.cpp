#include "ifs/os.h"
#include <log4cpp/Category.hh>
#include <exlib/thread.h>

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
static bool s_logEmpty;

void asyncLog(int priority, std::string msg)
{
	log4cpp::Category::getRoot().log(priority, msg);
//	s_acLog.put(new AsyncLog(priority, msg));
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

			Sleep(10);
		}
	}
} s_logger;

}
