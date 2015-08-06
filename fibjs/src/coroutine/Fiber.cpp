/*
 * Fiber.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "Fiber.h"
#include "ifs/os.h"

namespace fibjs
{

extern int32_t stack_size;
extern bool g_preemptive;

#define MAX_FIBER   10000
#define MAX_IDLE   256

static exlib::Queue<AsyncEvent> g_jobs;
static exlib::IDLE_PROC s_oldIdle;
static int32_t s_fibers;
static int32_t s_idleFibers;
int32_t g_spareFibers;

static int32_t g_tlsCurrent;
DateCache FiberBase::g_dc;

static class null_fiber_data: public Fiber_base
{
public:
    null_fiber_data()
    {
        Ref();
    }

    virtual result_t join()
    {
        return 0;
    }

    virtual result_t get_traceInfo(std::string& retVal)
    {
        return 0;
    }

    virtual result_t get_caller(obj_ptr<Fiber_base> &retVal)
    {
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

} *s_null;

static void onIdle()
{
    if (!g_jobs.empty() && (s_idleFibers == 0) &&  (s_fibers < MAX_FIBER))
    {
        s_fibers++;
        s_idleFibers ++;
        exlib::Fiber::Create(FiberBase::fiber_proc, NULL,
                             stack_size * 1024);
    }

    if (s_oldIdle)
        s_oldIdle();
}

extern exlib::LockedList<Isolate> s_isolates;
class _preemptThread: public exlib::OSThread
{
public:
    virtual void Run()
    {
        Isolate& isolate = *s_isolates.head();
        exlib::atomic_t lastTimes = isolate.service->m_switchTimes;
        int32_t cnt = 0;

        while (true)
        {
            sleep(100);

            if (isolate.service->m_resume.empty())
            {
                cnt = 0;
                continue;
            }

            if (lastTimes != isolate.service->m_switchTimes)
            {
                cnt = 0;
                lastTimes = isolate.service->m_switchTimes;
                continue;
            }

            cnt ++;
            if (cnt == 2)
            {
                cnt = 0;
                isolate.isolate->RequestInterrupt(InterruptCallback, NULL);
            }
        }
    }

private:
    static void InterruptCallback(v8::Isolate *isolate, void *data)
    {
        coroutine_base::sleep(0);
    }

} s_preemptThread;

void init_fiber()
{
    g_spareFibers = MAX_IDLE;
    s_null = new null_fiber_data();

    s_fibers = 0;
    s_idleFibers = 0;

    g_tlsCurrent = exlib::Fiber::tlsAlloc();
    s_oldIdle = exlib::Service::current()->onIdle(onIdle);

    if (g_preemptive)
        s_preemptThread.start();
}

void *FiberBase::fiber_proc(void *p)
{
    Isolate &isolate = Isolate::now();
    v8::Locker locker(isolate.isolate);
    v8::Isolate::Scope isolate_scope(isolate.isolate);

    v8::HandleScope handle_scope(isolate.isolate);
    v8::Context::Scope context_scope(
        v8::Local<v8::Context>::New(isolate.isolate, isolate.s_context));

    s_idleFibers --;
    while (1)
    {
        AsyncEvent *ae;

        if ((ae = g_jobs.tryget()) == NULL)
        {
            s_idleFibers ++;
            if (s_idleFibers > g_spareFibers) {
                s_idleFibers --;
                break;
            }

            v8::Unlocker unlocker(isolate.isolate);
            ae = g_jobs.get();

            s_idleFibers --;
        }

        {
            v8::HandleScope handle_scope(isolate.isolate);
            ae->js_invoke();
        }
    }

    s_fibers --;

    return NULL;
}

void FiberBase::start()
{
    m_caller = JSFiber::current();

    if (m_caller)
    {
        v8::Local<v8::Object> co = m_caller->wrap();
        v8::Local<v8::Object> o = wrap();

        v8::Local<v8::Array> ks = co->GetOwnPropertyNames();
        int32_t len = ks->Length();

        int32_t i;

        for (i = 0; i < len; i++)
        {
            v8::Local<v8::Value> k = ks->Get(i);
            o->Set(k, co->Get(k));
        }
    }

    g_jobs.put(this);
    Ref();
}

result_t FiberBase::join()
{
    if (!m_quit.isSet())
    {
        Isolate::rt _rt;
        m_quit.wait();
    }

    return 0;
}

result_t FiberBase::get_traceInfo(std::string& retVal)
{
    if (JSFiber::current() == this)
        retVal = traceInfo(300);
    else
        retVal = m_traceInfo;

    return 0;
}

result_t FiberBase::get_caller(obj_ptr<Fiber_base> &retVal)
{
    if (m_caller == NULL)
        return CALL_RETURN_NULL;

    retVal = m_caller;
    return 0;
}

void JSFiber::callFunction1(v8::Local<v8::Function> func,
                            v8::Local<v8::Value> *args, int32_t argCount,
                            v8::Local<v8::Value> &retVal)
{
    TryCatch try_catch;
    retVal = func->Call(wrap(), argCount, args);
    if (try_catch.HasCaught())
    {
        m_error = true;
        ReportException(try_catch, 0);
    }
}

void JSFiber::callFunction(v8::Local<v8::Value> &retVal)
{
    size_t i;
    Isolate &isolate = Isolate::now();
    std::vector<v8::Local<v8::Value> > argv;

    argv.resize(m_argv.size());
    for (i = 0; i < m_argv.size(); i++)
        argv[i] = v8::Local<v8::Value>::New(isolate.isolate, m_argv[i]);

    callFunction1(v8::Local<v8::Function>::New(isolate.isolate, m_func),
                  argv.data(), (int32_t) argv.size(), retVal);

    if (!IsEmpty(retVal))
        m_result.Reset(isolate.isolate, retVal);
}

JSFiber *JSFiber::current()
{
    return (JSFiber *)exlib::Fiber::tlsGet(g_tlsCurrent);
}

void JSFiber::js_invoke()
{
    scope s(this);
    v8::Local<v8::Value> retVal;
    callFunction(retVal);
    Unref();
}

JSFiber::scope::scope(JSFiber *fb) :
    m_hr(0), m_pFiber(fb)
{
    if (fb == NULL)
        m_pFiber = new JSFiber();

    exlib::Fiber::tlsPut(g_tlsCurrent, m_pFiber);
    Isolate::now().m_fibers.putTail(m_pFiber);
}

JSFiber::scope::~scope()
{
    v8::Local<v8::Object> o = m_pFiber->wrap();

    m_pFiber->m_quit.set();
    m_pFiber->dispose();

    s_null->Ref();
    o->SetAlignedPointerInInternalField(0, s_null);

    ReportException(try_catch, m_hr);
    Isolate::now().m_fibers.remove(m_pFiber);
    exlib::Fiber::tlsPut(g_tlsCurrent, 0);
}

void AsyncEvent::sync()
{
    g_jobs.put(this);
}

} /* namespace fibjs */
