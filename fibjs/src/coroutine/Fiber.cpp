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

extern int stack_size;

#define MAX_FIBER   10000
#define MAX_IDLE   10

static exlib::Queue<AsyncEvent> g_jobs;
static exlib::IDLE_PROC s_oldIdle;
static int32_t s_fibers;
static int32_t s_idleFibers;

static int g_tlsCurrent;
DateCache FiberBase::g_dc;

static class null_fiber_data: public Fiber_base
{
public:
    null_fiber_data()
    {
        Ref();
    }

    result_t join()
    {
        return 0;
    }

    result_t get_caller(obj_ptr<Fiber_base> &retVal)
    {
        return CHECK_ERROR(CALL_E_INVALID_CALL);
    }

} *s_null;

static void onIdle()
{
    if (!g_jobs.empty() && (s_fibers < MAX_FIBER))
    {
        s_fibers++;
        exlib::Fiber::Create(FiberBase::fiber_proc, NULL,
                             stack_size * 1024);
    }

    if (s_oldIdle)
        s_oldIdle();
}

inline void fiber_init()
{
    static bool bInit = false;

    if (!bInit)
    {
        bInit = true;

        s_null = new null_fiber_data();

        s_fibers = 0;
        s_idleFibers = 0;

        g_tlsCurrent = exlib::Fiber::tlsAlloc();
        s_oldIdle = exlib::Service::current()->onIdle(onIdle);
    }
}

void *FiberBase::fiber_proc(void *p)
{
    Isolate &isolate = Isolate::now();
    v8::Locker locker(isolate.isolate);
    v8::Isolate::Scope isolate_scope(isolate.isolate);

    v8::HandleScope handle_scope(isolate.isolate);
    v8::Context::Scope context_scope(
        v8::Local<v8::Context>::New(isolate.isolate, isolate.s_context));

    while (1)
    {
        AsyncEvent *ae;

        if ((ae = g_jobs.tryget()) == NULL)
        {
            s_idleFibers ++;
            if (s_idleFibers > MAX_IDLE) {
                s_idleFibers --;
                break;
            }

            v8::Unlocker unlocker(isolate.isolate);
            ae = g_jobs.get();

            s_idleFibers --;
        }

        {
            v8::HandleScope handle_scope(isolate.isolate);
            ae->js_callback();
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
        int len = ks->Length();

        int i;

        for (i = 0; i < len; i++)
        {
            v8::Local<v8::Value> k = ks->Get(i);
            o->Set(k, co->Get(k));
        }
    }

    g_jobs.put(this);
    Ref();
}

void JSFiber::callFunction1(v8::Local<v8::Function> func,
                            v8::Local<v8::Value> *args, int argCount,
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
                  argv.data(), (int) argv.size(), retVal);

    if (!IsEmpty(retVal))
        m_result.Reset(isolate.isolate, retVal);
}

JSFiber *JSFiber::current()
{
    fiber_init();
    return (JSFiber *)exlib::Fiber::tlsGet(g_tlsCurrent);
}

void JSFiber::js_callback()
{
    scope s(this);
    v8::Local<v8::Value> retVal;
    callFunction(retVal);
    Unref();
}

JSFiber::scope::scope(JSFiber *fb) :
    m_hr(0), m_pFiber(fb)
{
    fiber_init();

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

void asyncCallBack::callback()
{
    g_jobs.put(this);
}

} /* namespace fibjs */
