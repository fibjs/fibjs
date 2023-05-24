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
        v8::Context::Scope context_scope(isolate->m_context.Get(isolate->m_isolate));

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
        v8::Local<v8::Context> context = co->GetCreationContextChecked();

        v8::Local<v8::Array> ks = co->GetOwnPropertyNames(context).FromMaybe(v8::Local<v8::Array>());
        int32_t len = ks->Length();

        int32_t i;

        for (i = 0; i < len; i++) {
            JSValue k = ks->Get(context, i);
            o->Set(context, k, JSValue(co->Get(context, k))).IsJust();
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

result_t JSFiber::get_id(int64_t& retVal)
{
    retVal = m_id;
    return 0;
}

result_t JSFiber::get_stack(exlib::string& retVal)
{
    if (JSFiber::current() == this)
        retVal = traceInfo(holder()->m_isolate, 300);
    else
        retVal = traceInfo(holder()->m_isolate, 300, m_c_entry_fp_, m_handler_);

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
    v8::Local<v8::Function> func = m_func.Get(isolate->m_isolate);
    v8::Local<v8::Object> pThis = m_this.Get(isolate->m_isolate);

    argv.resize(m_argv.size());
    for (i = 0; i < m_argv.size(); i++)
        argv[i] = m_argv[i].Get(isolate->m_isolate);

    clear();

    retVal = func->Call(func->GetCreationContextChecked(), pThis, (int32_t)argv.size(), argv.data()).FromMaybe(v8::Local<v8::Value>());
    if (!IsEmpty(retVal))
        m_result.Reset(isolate->m_isolate, retVal);

    Unref();

    return 0;
}

JSFiber::EnterJsScope::EnterJsScope(JSFiber* fb, bool task)
    : m_hr(0)
    , m_pFiber(fb)
    , m_task(task)
{
    if (fb == NULL)
        m_pFiber = new JSFiber();

    s_current = m_pFiber;
    m_pFiber->m_bind_thread = exlib::Thread_base::current();

    Isolate* isolate = m_pFiber->holder();

    isolate->m_fibers.putTail(m_pFiber);

    m_fiber.Reset(isolate->m_isolate, m_pFiber->wrap(isolate));
}

JSFiber::EnterJsScope::~EnterJsScope()
{
    Runtime* rt = Runtime::current();
    Isolate* isolate = m_pFiber->holder();

    if (!m_task)
        isolate->RunMicrotasks();

    m_pFiber->m_message = ReportException(try_catch, m_hr);

    if (!rt->m_promise_error.IsEmpty()) {
        v8::Local<v8::Context> _context = isolate->context();
        v8::Local<v8::Array> _promise_error = rt->m_promise_error.Get(isolate->m_isolate);
        JSArray ks = _promise_error->GetPropertyNames(_context);
        int32_t len = ks->Length();

        for (int32_t i = 0; i < len; i++) {
            JSValue v = _promise_error->Get(_context, JSValue(ks->Get(_context, i)));
            if (v->IsArray()) {
                v8::Local<v8::Array> o = v8::Local<v8::Array>::Cast(v);
                v = o->Get(_context, 1);

                if (v->IsNativeError()) {
                    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(v);
                    errorLog(isolate->toString(JSValue(obj->Get(_context, isolate->NewString("stack")))));
                } else
                    errorLog(isolate->toString(v));
            }
        }

        rt->m_promise_error.Reset();
        rt->m_promise_error_no = 0;
    }

    m_pFiber->m_quit.set();

    m_pFiber->holder()->m_fibers.remove(m_pFiber);
    s_current = 0;
}

} /* namespace fibjs */
