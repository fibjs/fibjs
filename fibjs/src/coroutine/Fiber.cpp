/*
 * Fiber.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: lion
 */

#include "object.h"
#include "Fiber.h"
#include "ifs/os.h"
#include "ifs/process.h"
#include "options.h"

namespace fibjs {

#define MAX_IDLE 256

int32_t g_spareFibers = MAX_IDLE;
static exlib::fiber_local<JSFiber*> s_current;

void JSFiber::FiberProcRunJavascript(void* p)
{
    result_t hr = 0;
    Isolate* isolate = (Isolate*)p;

    {
        Runtime rtForThread(isolate);
        v8::Locker locker(isolate->m_isolate);
        v8::Isolate::Scope isolate_scope(isolate->m_isolate);

        v8::HandleScope handle_scope(isolate->m_isolate);
        v8::Context::Scope context_scope(
            v8::Local<v8::Context>::New(isolate->m_isolate, isolate->m_context));

        isolate->m_idleFibers--;
        while (1) {
            if (!isolate->m_sem.trywait()) {
                isolate->m_idleFibers++;
                if (isolate->m_idleFibers > g_spareFibers) {
                    isolate->m_idleFibers--;
                    break;
                }

                {
                    v8::Unlocker unlocker(isolate->m_isolate);
                    isolate->m_sem.wait();
                }

                isolate->m_idleFibers--;
            }

            if (isolate->m_idleFibers == 0) {
                isolate->m_currentFibers++;
                isolate->m_idleFibers++;

                exlib::Service::CreateFiber(FiberProcRunJavascript, isolate, stack_size * 1024, "JSFiber");
            }

            {
                v8::HandleScope handle_scope(isolate->m_isolate);
                AsyncEvent* ae = (AsyncEvent*)isolate->m_jobs.getHead();

                hr = ae->js_invoke();
            }

            isolate->Unref(hr);
        }

        isolate->m_currentFibers--;
    }

    isolate->m_isolate->DiscardThreadSpecificMetadata();
}

void JSFiber::set_caller(Fiber_base* caller)
{
    m_caller = caller;

    if (m_caller) {
        v8::Local<v8::Object> co = m_caller->wrap();
        v8::Local<v8::Object> o = wrap();
        v8::Local<v8::Context> context = co->CreationContext();

        v8::Local<v8::Array> ks;
        co->GetOwnPropertyNames(context).ToLocal(&ks);
        int32_t len = ks->Length();

        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);
            o->Set(context, k, JSValue(co->Get(context, k)));
        }
    }
}

result_t sync_invoke(JSFiber* fiber)
{
    return fiber->js_invoke();
}

void JSFiber::start()
{
    Ref();
    set_caller(JSFiber::current());
    syncCall(holder(), sync_invoke, this);
}

result_t JSFiber::join()
{
    if (!m_quit.isSet()) {
        Isolate::LeaveJsScope _rt(holder());
        m_quit.wait();
    }

    return 0;
}

save_method_name::save_method_name(const char* name)
{
    m_fb = JSFiber::current();

    m_name = m_fb->m_native_name;
    m_fb->m_native_name = name;
}

save_method_name::~save_method_name()
{
    m_fb->m_native_name = m_name;

    if (m_fb->m_termed && !m_fb->holder()->m_isolate->IsExecutionTerminating())
        m_fb->holder()->m_isolate->TerminateExecution();
}

result_t JSFiber::get_id(int64_t& retVal)
{
    retVal = m_id;
    return 0;
}

result_t JSFiber::get_stack(exlib::string& retVal)
{
    if (JSFiber::current() == this)
        retVal = traceInfo(holder()->m_isolate, 300);
    else {
        exlib::string str;

        if (m_native_name) {
            str = "    at ";
            str += m_native_name;
            str += " (native code)";
        }

        exlib::string str1 = traceInfo(holder()->m_isolate, 300, m_c_entry_fp_, m_handler_);
        if (!str1.empty()) {
            if (m_native_name)
                str += '\n';
            str += str1;
        }

        retVal = str;
    }

    return 0;
}

result_t JSFiber::get_caller(obj_ptr<Fiber_base>& retVal)
{
    if (m_caller == NULL)
        return CALL_RETURN_NULL;

    retVal = m_caller;
    return 0;
}

result_t JSFiber::get_stack_usage(int32_t& retVal)
{
    if (JSFiber::current() == this) {
        V8FrameInfo _fi = save_fi(holder()->m_isolate);
        if (_fi.entry_fp)
            retVal = (int32_t)(m_bind_thread->stackguard() - (intptr_t)_fi.entry_fp);
        else
            retVal = 0;
    } else if (m_bind_thread)
        retVal = (int32_t)(m_bind_thread->stackguard() - (intptr_t)m_c_entry_fp_);
    else
        retVal = 0;

    return 0;
}

JSFiber* JSFiber::current()
{
    return s_current;
}

result_t JSFiber::js_invoke()
{
    EnterJsScope s(this);
    v8::Local<v8::Value> retVal;

    size_t i;
    Isolate* isolate = holder();
    std::vector<v8::Local<v8::Value>> argv;
    v8::Local<v8::Function> func = v8::Local<v8::Function>::New(isolate->m_isolate, m_func);
    v8::Local<v8::Object> pThis = v8::Local<v8::Object>::New(isolate->m_isolate, m_this);

    argv.resize(m_argv.size());
    for (i = 0; i < m_argv.size(); i++)
        argv[i] = v8::Local<v8::Value>::New(isolate->m_isolate, m_argv[i]);

    clear();

    func->Call(func->CreationContext(), pThis, (int32_t)argv.size(), argv.data()).ToLocal(&retVal);

    if (!IsEmpty(retVal))
        m_result.Reset(isolate->m_isolate, retVal);

    Unref();

    return 0;
}

JSFiber::EnterJsScope::EnterJsScope(JSFiber* fb)
    : m_hr(0)
    , m_pFiber(fb)
{
    if (fb == NULL)
        m_pFiber = new JSFiber();

    s_current = m_pFiber;
    m_pFiber->m_bind_thread = exlib::Thread_base::current();

    m_pFiber->holder()->m_fibers.putTail(m_pFiber);

    m_fiber.Reset(m_pFiber->holder()->m_isolate, m_pFiber->wrap());
}

JSFiber::EnterJsScope::~EnterJsScope()
{
    m_pFiber->holder()->RunMicrotasks();

    m_pFiber->m_message = ReportException(try_catch, m_hr);

    m_pFiber->m_quit.set();

    m_pFiber->holder()->m_fibers.remove(m_pFiber);
    s_current = 0;
}

} /* namespace fibjs */
