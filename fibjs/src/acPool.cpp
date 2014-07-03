#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

BlockedAsyncQueue s_acPool;

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

static class _acThreadDog: public exlib::OSThread
{
public:
    _acThreadDog()
    {
        start();
    }

    virtual void Run()
    {
        while (1)
        {
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

            Sleep(100);
        }
    }
} s_dog;

}
