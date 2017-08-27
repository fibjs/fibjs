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

namespace fibjs {

#define WORKER_STACK_SIZE 128

class acPool {
public:
    acPool(int32_t max_idle, bool bThread = false)
        : m_bThread(bThread)
        , m_max_idle(max_idle)
        , m_idleWorkers(1)
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
        class _thread : public exlib::OSThread {
        public:
            typedef void (*thread_func)(void*);

        public:
            _thread(thread_func proc, void* arg)
                : m_proc(proc)
                , m_arg(arg)
            {
            }

        public:
            virtual void Run()
            {
#ifdef _WIN32
                CoInitializeEx(NULL, COINIT_MULTITHREADED);
                m_proc(m_arg);
                CoUninitialize();
#else
                m_proc(m_arg);
#endif
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
        AsyncEvent* p;

        m_idleWorkers.dec();

        while (true) {
            if (m_idleWorkers.inc() > m_max_idle) {
                if (m_idleWorkers.dec() > 0)
                    break;

                m_idleWorkers.inc();
            }

            p = m_pool.get();
            if (m_idleWorkers.dec() == 0)
                if (m_idleWorkers.CompareAndSwap(0, 1) == 0)
                    new_worker();

            p->invoke();
        }
    }

    static void worker_proc(void* ptr)
    {
        ((acPool*)ptr)->worker_proc();
    }

private:
    bool m_bThread;
    int32_t m_max_idle;
    exlib::Queue<AsyncEvent> m_pool;
    exlib::atomic m_idleWorkers;
};

static acPool* s_acPool;
static acPool* s_lsPool;

void putGuiPool(AsyncEvent* ac);

void AsyncEvent::async(int32_t type)
{
    if (type == CALL_E_NOSYNC)
        s_acPool->put(this);
    else if (type == CALL_E_LONGSYNC)
        s_lsPool->put(this);
    else if (type == CALL_E_GUICALL)
        putGuiPool(this);
}

AsyncCallBack::AsyncCallBack(object_base* pThis, v8::Local<v8::Function> cb)
{
    m_pThis = pThis;
    m_isolate = pThis->holder();
    m_isolate->m_pendding.inc();
    m_cb.Reset(m_isolate->m_isolate, cb);
}

void AsyncCallBack::async_call(int32_t v)
{
    m_v = v;
    if (m_pThis == NULL || m_pThis->enterTask(this))
        async(v);
}

int32_t AsyncCallBack::post(int32_t v)
{
    if (m_pThis)
        m_pThis->leave(this);
    return callback(v);
}

result_t AsyncCallBack::syncFunc(AsyncCallBack* pThis)
{
    JSFiber::scope s;

    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(pThis->m_isolate->m_isolate, pThis->m_cb);

    v8::Local<v8::Value> args[2];

    if (pThis->m_v == CALL_RETURN_NULL) {
        args[0] = v8::Undefined(pThis->m_isolate->m_isolate);
        args[1] = v8::Null(pThis->m_isolate->m_isolate);
    } else if (pThis->m_v >= 0) {
        args[0] = v8::Undefined(pThis->m_isolate->m_isolate);
        args[1] = pThis->getValue();
    } else {
        if (pThis->m_v == CALL_E_EXCEPTION)
            Runtime::setError(pThis->m_error);

        v8::Local<v8::Value> e = v8::Exception::Error(
            pThis->m_isolate->NewString(getResultMessage(pThis->m_v)));
        e->ToObject()->Set(pThis->m_isolate->NewString("number"),
            v8::Int32::New(pThis->m_isolate->m_isolate, -pThis->m_v));

        args[0] = e;
        args[1] = v8::Undefined(pThis->m_isolate->m_isolate);
    }

    func->Call(v8::Undefined(pThis->m_isolate->m_isolate), 2, args);

    delete pThis;

    return 0;
}

void init_acThread()
{
#ifdef _WIN32
    s_acPool = s_lsPool = new acPool(10, true);
#else
    s_lsPool = new acPool(2, true);
    s_acPool = new acPool(2);
#endif
}
}
