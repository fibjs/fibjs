#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/thread.h>
#include "ifs/coroutine.h"
#include "console.h"
#include "map"

namespace fibjs
{

exlib::AsyncQueue s_acPool;

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

        Runtime rt;
        DateCache dc;
        rt.m_pDateCache = &dc;

        Runtime::reg(&rt);

        while (1)
        {
            if (exlib::atom_inc(&s_idleThreads) > s_threads * 2)
            {
                exlib::atom_dec(&s_idleThreads);
                break;
            }

            p = (asyncEvent *)s_acPool.wait();
            exlib::atom_dec(&s_idleThreads);

            p->invoke();
        }
    }
} s_ac;

static exlib::AsyncQueue s_acLog;
static bool s_logEmpty;

int32_t g_loglevel = console_base::_NOTSET;

void asyncLog(int priority, std::string msg)
{
    if (priority <= g_loglevel)
    {
        //  log4cpp::Category::getRoot().log(priority, msg);
        s_acLog.put(new logger::item(priority, msg));
    }
}

void flushLog()
{
    while (!s_acLog.empty() || !s_logEmpty)
        coroutine_base::ac_sleep(1);
}

inline void _append(int32_t priority, std::string &msg)
{
    std::string txt;
    if (priority == console_base::_NOTICE)
        txt = logger::notice() + msg + COLOR_RESET + "\n";
    else if (priority == console_base::_WARN)
        txt = logger::warn() + msg + COLOR_RESET + "\n";
    else if (priority <= console_base::_ERROR)
        txt = logger::error() + msg + COLOR_RESET + "\n";
    else
        txt = msg + "\n";

    logger::std_out(txt.c_str());
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
        std::multimap<int64_t, AsyncCall *>::iterator e;
        logger::item *p1, *p2, *pn;

        while (1)
        {
            s_logEmpty = false;

            p1 = (logger::item *)s_acLog.getList();
            while (p1)
            {
                pn = NULL;

                while (p1)
                {
                    p2 = p1;
                    p1 = (logger::item *) p1->m_next;
                    p2->m_next = pn;
                    pn = p2;
                }

                while (pn)
                {
                    p1 = pn;
                    pn = (logger::item *) pn->m_next;
                    _append(p1->m_priority, p1->m_msg);
                    delete p1;
                }

                p1 = (logger::item *)s_acLog.getList();
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
