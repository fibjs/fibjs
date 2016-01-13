#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

static exlib::Queue<AsyncEvent> s_acPool;
#define WORKER_STACK_SIZE   128
static exlib::atomic s_threads;
static exlib::atomic s_idleThreads;

class _acThread: public exlib::Service
{
public:
    _acThread() : exlib::Service(NULL), m_idles(0)
    {
        s_idleThreads.inc();
        start();
    }

    void worker()
    {
        Runtime rt;
        Runtime::reg(&rt);

        AsyncEvent *p;

        while (m_idles <= 2)
        {
            m_lock.lock();

            s_idleThreads.inc();
            p = s_acPool.get();
            if (s_idleThreads.dec() == 0 && s_threads > 0)
            {
                if (s_threads.dec() < 0)
                    s_threads.inc();
                else
                    new _acThread();
            }

            m_lock.unlock();

            m_idles --;
            if (m_idles == 0) {
                m_idles ++;
                Create(fiber_proc, this, WORKER_STACK_SIZE * 1024);
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
        s_idleThreads.dec();

        m_idles ++;
        Create(fiber_proc, this, WORKER_STACK_SIZE * 1024);

        m_wait.wait();
    }

private:
    exlib::Event m_wait;
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

    s_threads.dec();
    new _acThread();
}

}
