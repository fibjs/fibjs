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

#define MAX_FIBER   10000
#define MAX_IDLE   256

int32_t g_spareFibers;
static int32_t g_tlsCurrent;

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
    Isolate* isolate = Isolate::current();

    if (!isolate->m_jobs.empty() && (isolate->m_idleFibers == 0) &&  (isolate->m_currentFibers < MAX_FIBER))
    {
        isolate->m_currentFibers++;
        isolate->m_idleFibers ++;
        exlib::Fiber::Create(FiberBase::fiber_proc, isolate,
                             stack_size * 1024);
    }

    if (isolate->m_oldIdle)
        isolate->m_oldIdle();
}

void init_fiber(Isolate* isolate)
{
    static bool s_init = false;

    isolate->m_currentFibers = 0;
    isolate->m_idleFibers = 0;
    isolate->m_oldIdle = exlib::Service::current()->onIdle(onIdle);

    if (!s_init)
    {
        s_init = true;

        g_spareFibers = MAX_IDLE;
        g_tlsCurrent = exlib::Fiber::tlsAlloc();
        s_null = new null_fiber_data();
    }
}

void *FiberBase::fiber_proc(void *p)
{
    Isolate* isolate = (Isolate*)p;
    v8::Locker locker(isolate->m_isolate);
    v8::Isolate::Scope isolate_scope(isolate->m_isolate);

    v8::HandleScope handle_scope(isolate->m_isolate);
    v8::Context::Scope context_scope(
        v8::Local<v8::Context>::New(isolate->m_isolate, isolate->m_context));

    isolate->m_idleFibers --;
    while (1)
    {
        AsyncEvent *ae;

        if ((ae = (AsyncEvent*)isolate->m_jobs.tryget()) == NULL)
        {
            isolate->m_idleFibers ++;
            if (isolate->m_idleFibers > g_spareFibers) {
                isolate->m_idleFibers --;
                break;
            }

            {
                v8::Unlocker unlocker(isolate->m_isolate);
                ae = (AsyncEvent*)isolate->m_jobs.get();
            }

            isolate->m_idleFibers --;
        }

        {
            v8::HandleScope handle_scope(isolate->m_isolate);
            ae->js_invoke();
        }
    }

    isolate->m_currentFibers --;

    return NULL;
}

void FiberBase::set_caller(Fiber_base* caller)
{
    m_caller = caller;

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
}

void FiberBase::start()
{
    Isolate* isolate = holder();

    set_caller(JSFiber::current());
    isolate->m_jobs.put(this);
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

JSFiber *JSFiber::current()
{
    return (JSFiber *)exlib::Fiber::tlsGet(g_tlsCurrent);
}

void JSFiber::js_invoke()
{
    scope s(this);
    v8::Local<v8::Value> retVal;

    size_t i;
    Isolate* isolate = holder();
    std::vector<v8::Local<v8::Value> > argv;
    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate->m_isolate, m_func);

    argv.resize(m_argv.size());
    for (i = 0; i < m_argv.size(); i++)
        argv[i] = v8::Local<v8::Value>::New(isolate->m_isolate, m_argv[i]);

    clear();

    retVal = func->Call(wrap(), (int32_t) argv.size(), argv.data());

    if (!IsEmpty(retVal))
        m_result.Reset(isolate->m_isolate, retVal);

    Unref();
}

JSFiber::scope::scope(JSFiber *fb) :
    m_hr(0), m_pFiber(fb)
{
    if (fb == NULL)
        m_pFiber = new JSFiber();

    exlib::Fiber::tlsPut(g_tlsCurrent, m_pFiber);
    m_pFiber->holder()->m_fibers.putTail(m_pFiber);
}

JSFiber::scope::~scope()
{
    v8::Local<v8::Object> o = m_pFiber->wrap();

    m_pFiber->m_quit.set();
    m_pFiber->dispose();

    s_null->Ref();
    o->SetAlignedPointerInInternalField(0, s_null);

    ReportException(try_catch, m_hr);
    m_pFiber->holder()->m_fibers.remove(m_pFiber);
    exlib::Fiber::tlsPut(g_tlsCurrent, 0);
}

} /* namespace fibjs */
