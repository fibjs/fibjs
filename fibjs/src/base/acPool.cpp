#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

static exlib::Queue<AsyncEvent> s_acPool;
extern int32_t stack_size;
static exlib::atomic s_threads;
static exlib::atomic s_idleThreads;

class _acThread: public exlib::Service
{
public:
    _acThread() : m_idles(0)
    {
        s_idleThreads.inc();
        s_threads.dec();
        start();
    }

    void worker()
    {
        Runtime rt;
        rt.m_pDateCache = &m_dc;
        Runtime::reg(&rt);

        AsyncEvent *p;

        s_idleThreads.dec();
        while (m_idles <= 2)
        {
            m_lock.lock();

            s_idleThreads.inc();
            p = s_acPool.get();
            if (s_idleThreads.dec() == 0 && s_threads > 0)
                new _acThread();

            m_lock.unlock();

            m_idles --;
            if (m_idles == 0) {
                m_idles ++;
                exlib::Fiber::Create(fiber_proc, this, stack_size * 1024);
            }

            p->invoke();

            m_idles ++;
        }

        m_idles --;
    }

    static void *fiber_proc(void *p)
    {
        ((_acThread*)p)->worker();
        return 0;
    }

    virtual void Run()
    {
        m_idles ++;
        exlib::Fiber::Create(fiber_proc, this, stack_size * 1024);
        m_wait.wait();
    }

private:
    exlib::Event m_wait;
    DateCache m_dc;
    exlib::Locker m_lock;
    int32_t m_idles;
};

void AsyncEvent::async()
{
    s_acPool.put(this);
}

void init_acThread()
{
    int32_t cpus = 0;

    os_base::CPUs(cpus);
    if (cpus < 1)
        cpus = 1;

    s_threads = cpus * 2;
    new _acThread();
}

}
