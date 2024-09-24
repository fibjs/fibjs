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
#include <unordered_map>
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
            (new _thread(FiberProcWorker, this))->start();
        else
            exlib::Service::CreateFiber(FiberProcWorker, this, WORKER_STACK_SIZE * 1024, "WorkerFiber");
    }

    void FiberProcWorker()
    {
        Runtime rtForThread(NULL);
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

    static void FiberProcWorker(void* ptr)
    {
        ((acPool*)ptr)->FiberProcWorker();
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

AsyncEvent::AsyncEvent(Isolate* isolate)
    : m_isolate(isolate)
    , m_state(kStateSync)
{
}

AsyncEvent::~AsyncEvent()
{
}

void AsyncEvent::async(int32_t type)
{
    if (type == CALL_E_NOSYNC)
        s_acPool->put(this);
    else if (type == CALL_E_LONGSYNC)
        s_lsPool->put(this);
    else if (type == CALL_E_GUICALL)
        putGuiPool(this);
}

AsyncCallBack::AsyncCallBack(v8::Local<v8::Object> cb, object_base* pThis)
{
    if (pThis) {
        m_pThis = pThis;
        m_isolate = pThis->holder();
    } else
        m_isolate = Isolate::current(cb);

    m_isolate->Ref();
    m_cb.Reset(m_isolate->m_isolate, cb);
    m_is_promise = !cb->IsFunction();
}

AsyncCallBack::~AsyncCallBack()
{
    m_isolate->Unref();
    m_cb.Reset();
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
    assert(m_isolate);
    return callback(v);
}

void AsyncCallBack::fillRetVal(std::vector<v8::Local<v8::Value>>& args, object_base* obj)
{
    v8::Local<v8::Value> v;
    obj->valueOf(v);
    args.push_back(v);
}

void AsyncCallBack::fillRetVal(std::vector<v8::Local<v8::Value>>& args, NType* v)
{
    m_result = v;
    if (!m_is_promise)
        v->to_args(m_isolate, args);
}

int AsyncCallBack::syncFunc()
{
    JSFiber::EnterJsScope s;
    std::vector<v8::Local<v8::Value>> args;

    if (m_is_promise) {
        v8::Local<v8::Promise::Resolver> resolver = m_cb.Get(m_isolate->m_isolate).As<v8::Promise::Resolver>();

        if (m_v == CALL_RETURN_NULL)
            resolver->Resolve(m_isolate->context(), v8::Null(m_isolate->m_isolate)).IsJust();
        else if (m_v == CALL_RETURN_UNDEFINED)
            resolver->Resolve(m_isolate->context(), v8::Undefined(m_isolate->m_isolate)).IsJust();
        else if (m_v >= 0) {
            to_args(args);

            v8::Local<v8::Value> result;
            if (m_result)
                m_result->valueOf(result);
            else if (args.size() == 0)
                result = v8::Undefined(m_isolate->m_isolate);
            else {
                result = args[0];

                if (result->IsObject()) {
                    v8::Local<v8::Object> o = result.As<v8::Object>();
                    obj_ptr<object_base> obj = object_base::getInstance(o);
                    if (obj) {
                        ClassInfo& ci = obj->Classinfo();
                        if (ci.hasAsync())
                            o->SetPrototype(m_isolate->context(), ci.GetAsyncPrototype(m_isolate)).IsJust();
                    }
                }
            }

            resolver->Resolve(m_isolate->context(), result).IsJust();
        } else
            resolver->Reject(m_isolate->context(), FillError(m_v)).IsJust();
    } else {
        v8::Local<v8::Function> func = m_cb.Get(m_isolate->m_isolate).As<v8::Function>();

        if (m_v == CALL_RETURN_NULL) {
            args.resize(2);
            args[0] = v8::Undefined(m_isolate->m_isolate);
            args[1] = v8::Null(m_isolate->m_isolate);
        } else if (m_v >= 0) {
            args.resize(1);
            args[0] = v8::Undefined(m_isolate->m_isolate);
            to_args(args);
        } else {
            if (m_v == CALL_E_EXCEPTION)
                Runtime::setError(m_error);

            args.resize(1);
            args[0] = FillError(m_v);
        }

        v8::Local<v8::Value> oThis;
        if (m_result != nullptr)
            oThis = m_result->wrap(m_isolate);
        else
            oThis = v8::Undefined(m_isolate->m_isolate);

        func->Call(func->GetCreationContextChecked(), oThis, (int32_t)args.size(), args.data())
            .IsEmpty();
    }

    delete this;

    return 0;
}

int32_t AsyncCallBack::check_result(int32_t hr, const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (hr != CALL_E_NOSYNC && hr != CALL_E_LONGSYNC && hr != CALL_E_GUICALL)
        callback(hr);
    else
        async_call(hr);

    if (m_is_promise) {
        v8::Local<v8::Promise::Resolver> resolver = m_cb.Get(m_isolate->m_isolate).As<v8::Promise::Resolver>();
        args.GetReturnValue().Set(resolver->GetPromise());
    } else if (m_ctxo)
        args.GetReturnValue().Set(GetReturnValue(m_isolate, m_ctxo));

    return CALL_RETURN_UNDEFINED;
}

void InitializeAcPool()
{
    s_lsPool = new acPool(2, true);
    s_acPool = new acPool(2);
}
}
