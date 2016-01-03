#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

static exlib::Queue<AsyncEvent> s_acPool;
extern int32_t stack_size;

class _acThread: public exlib::Service
{
public:
    _acThread() : m_idles(0)
    {
        start();
    }

    void worker()
    {
        Runtime rt;
        rt.m_pDateCache = &m_dc;
        Runtime::reg(&rt);

        while (m_idles <= 2)
        {
            m_lock.lock();
            AsyncEvent *p = s_acPool.get();
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

    for (int32_t i = 0; i < cpus * 2; i ++)
        new _acThread();
}

}
