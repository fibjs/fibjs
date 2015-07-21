#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

BlockedAsyncQueue s_acPool;

static int32_t s_threads;
static exlib::atomic s_idleThreads;
static int32_t s_idleCount;

class _acThread: public exlib::OSThread
{
public:
    _acThread()
    {
        start();
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
            if (s_idleThreads.inc() > s_threads * 3)
            {
                s_idleThreads.dec();
                break;
            }

            p = (asyncEvent *)s_acPool.wait();
            s_idleThreads.dec();

            p->invoke();
        }
    }
};

void asyncEvent::async()
{
    s_acPool.put(this);
}

static class _acThreadDog: public exlib::OSThread
{
public:
    _acThreadDog()
    {
        start();
    }

    virtual void Run()
    {
        int32_t cpus = 0;

        os_base::CPUs(cpus);
        if (cpus < 3)
            cpus = 3;

        s_threads = cpus;
        s_idleCount = 100;

        while (1)
        {
            if (s_idleThreads < s_threads)
            {
                if (++s_idleCount > 5)
                {
                    s_idleCount = 0;

                    for (int32_t i = (int32_t)s_idleThreads; i < s_threads; i++)
                        new _acThread();
                }
            }
            else
                s_idleCount = 0;

            Sleep(100);
        }
    }
} s_dog;

}
