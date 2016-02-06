#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

#define WORKER_STACK_SIZE   128
#define MAX_IDLE_WORKERS    2

class acPool
{
public:
    acPool(bool bThread = false) : m_bThread(bThread)
    {
        m_idleWorkers.inc();
        new_worker();
    }

public:
    void put(AsyncEvent* ac)
    {
        m_pool.put(ac);
    }

private:
    void new_worker()
    {
        class _thread : public exlib::OSThread
        {
        public:
            typedef void* (*thread_func)(void *);

        public:
            _thread(thread_func proc, void* arg) : m_proc(proc), m_arg(arg)
            {}

        public:
            virtual void Run()
            {
                m_proc(m_arg);
            }

        private:
            thread_func m_proc;
            void* m_arg;
        };

        if (m_bThread)
            (new _thread(worker_proc, this))->start();
        else
            exlib::Service::Create(worker_proc, this, WORKER_STACK_SIZE * 1024, "WorkerFiber");
    }

    void worker_proc()
    {
        Runtime rt(NULL);
        AsyncEvent *p;

        m_idleWorkers.dec();

        while (true)
        {
            if (m_idleWorkers.inc() > MAX_IDLE_WORKERS)
                break;

            p = m_pool.get();
            if (m_idleWorkers.dec() == 0)
            {
                if (m_idleWorkers.inc() > MAX_IDLE_WORKERS)
                    m_idleWorkers.dec();
                else
                    new_worker();
            }

            p->invoke();
        }

        m_idleWorkers.dec();
    }

    static void *worker_proc(void *ptr)
    {
        ((acPool*)ptr)->worker_proc();
        return 0;
    }

private:
    bool m_bThread;
    exlib::Queue<AsyncEvent> m_pool;
    exlib::atomic m_idleWorkers;
};

static acPool* s_acPool;
static acPool* s_lsPool;

void AsyncEvent::async(bool bLongSync)
{
    if (bLongSync)
        s_lsPool->put(this);
    else
        s_acPool->put(this);
}

void init_acThread()
{
    s_acPool = new acPool();
    s_lsPool = new acPool(true);
}

}
