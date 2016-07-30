/*
 * date.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: lion
 */

#include "object.h"
#include "ifs/os.h"
#include "ifs/console.h"
#include <exlib/include/thread.h>
#include "console.h"
#include "map"
#include "Fiber.h"

namespace fibjs
{

#define WORKER_STACK_SIZE   128
#define MAX_IDLE_WORKERS    2

class acPool
{
public:
    acPool(bool bThread = false) : m_bThread(bThread), m_idleWorkers(1)
    {
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
            {
                if (m_idleWorkers.dec() > 0)
                    break;

                m_idleWorkers.inc();
            }

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

void AsyncCallBack::syncFunc(AsyncCallBack* pThis)
{
    JSFiber::scope s;

    v8::Local<v8::Function> func =
        v8::Local<v8::Function>::New(pThis->m_isolate->m_isolate, pThis->m_cb);

    v8::Local<v8::Value> args[2];

    if (pThis->m_v == CALL_RETURN_NULL)
    {
        args[0] = v8::Undefined(pThis->m_isolate->m_isolate);
        args[1] = v8::Null(pThis->m_isolate->m_isolate);
    } else if (pThis->m_v >= 0)
    {
        args[0] = v8::Undefined(pThis->m_isolate->m_isolate);
        args[1] = pThis->getValue();
    } else
    {
        v8::Local<v8::Value> e = v8::Exception::Error(
                                     pThis->m_isolate->NewFromUtf8(getResultMessage(pThis->m_v)));
        e->ToObject()->Set(pThis->m_isolate->NewFromUtf8("number"),
                           v8::Int32::New(pThis->m_isolate->m_isolate, -pThis->m_v));

        args[0] = e;
        args[1] = v8::Undefined(pThis->m_isolate->m_isolate);
    }

    func->Call(v8::Undefined(pThis->m_isolate->m_isolate), 2, args);

    delete pThis;
}

void init_acThread()
{
    s_acPool = new acPool();
    s_lsPool = new acPool(true);
}

}
