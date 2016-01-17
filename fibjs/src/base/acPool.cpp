#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"

namespace fibjs
{

#define WORKER_STACK_SIZE   128
#define MAX_IDLE_WORKERS    2

static exlib::Queue<AsyncEvent> s_acPool;
static exlib::atomic s_idleWorkers;
static exlib::Service* s_service;

static void *worker_proc(void *ptr)
{
    Runtime rt;
    Runtime::reg(&rt);

    AsyncEvent *p;

    s_idleWorkers.dec();

    while (true)
    {
        if (s_idleWorkers.inc() > MAX_IDLE_WORKERS)
            break;

        p = s_acPool.get();
        if (s_idleWorkers.dec() == 0)
        {
            if (s_idleWorkers.inc() > MAX_IDLE_WORKERS)
                s_idleWorkers.dec();
            else
                s_service->Create(worker_proc, NULL, WORKER_STACK_SIZE * 1024);
        }

        p->invoke();
    }

    s_idleWorkers.dec();

    return 0;
}

void AsyncEvent::async()
{
    s_acPool.put(this);
}

void init_acThread()
{
    int32_t cpus = 0;

    os_base::CPUs(cpus);
    if (cpus < 2)
        cpus = 2;

    s_service = new exlib::Service(cpus - 1);

    s_idleWorkers.inc();
    s_service->Create(worker_proc, NULL, WORKER_STACK_SIZE * 1024);
    s_service->start();
}

}
