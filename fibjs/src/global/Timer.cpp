/*
 * Timer.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: lion
 */

#include "object.h"
#include "utils.h"
#include "QuickArray.h"
#include "ifs/global.h"
#include "ifs/timers.h"
#include "Timer.h"

namespace fibjs {

void InvokeApiInterruptCallbacks(v8::Isolate* isolate);

#define TIMEOUT_MAX 2147483647 // 2^31-1

class JSTimer : public Timer {
public:
    JSTimer(v8::Local<v8::Function> callback, OptArgs& args, int32_t timeout = 0,
        bool repeat = false, bool hr = false)
        : Timer(timeout, repeat)
        , m_hr(hr)
        , m_clear_pendding(false)
    {
        Isolate* isolate = holder();

        int32_t nArgCount = args.Length();
        m_argv.resize(nArgCount);
        for (int i = 0; i < nArgCount; i++)
            m_argv[i].Reset(isolate->m_isolate, args[i]);

        isolate->m_pendding.inc();
        m_callback.Reset(isolate->m_isolate, callback);
    }

public:
    virtual void resume()
    {
        Isolate* isolate = holder();

        if (m_hr) {
            holder()->m_isolate->RequestInterrupt(_InterruptCallback, this);

            if (m_has_worker.CompareAndSwap(0, 1) == 0) {
                Ref();
                syncCall(isolate, js_worker, this);
            }
        } else
            syncCall(isolate, _callback, this);
    }

    static void _InterruptCallback(v8::Isolate* isolate, void* data)
    {
        _callback((JSTimer*)data);
    }

public:
    void do_timer_in_fiber()
    {
        Isolate* isolate = holder();
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(isolate->m_isolate, m_callback);
        std::vector<v8::Local<v8::Value>> argv;

        int32_t nArgCount = (int32_t)m_argv.size();
        argv.resize(nArgCount);
        for (int i = 0; i < nArgCount; i++)
            argv[i] = v8::Local<v8::Value>::New(isolate->m_isolate, m_argv[i]);

        callback->Call(wrap(), (int32_t)argv.size(), argv.data());
    }

    virtual void on_timer()
    {
        if (m_hr)
            do_timer_in_fiber();
        else {
            JSFiber::scope s;
            do_timer_in_fiber();
        }
    }

    virtual void on_clean()
    {
        m_callback.Reset();
        if (!m_clear_pendding) {
            m_clear_pendding = true;
            holder()->m_pendding.dec();
        }
    }

    static result_t js_worker(JSTimer* pThis)
    {
        JSFiber::scope s;
        Isolate* isolate = pThis->holder();

        InvokeApiInterruptCallbacks(isolate->m_isolate);

        pThis->m_has_worker = 0;
        pThis->Unref();

        return 0;
    }

private:
    bool m_hr;
    exlib::atomic m_has_worker;
    bool m_clear_pendding;
    QuickArray<v8::Global<v8::Value>> m_argv;
    v8::Global<v8::Function> m_callback;
};

DECLARE_MODULE(timers);

result_t timers_base::clearInterval(Timer_base* t)
{
    return t->clear();
}

result_t timers_base::clearHrInterval(Timer_base* t)
{
    return t->clear();
}

result_t timers_base::clearTimeout(Timer_base* t)
{
    return t->clear();
}

result_t timers_base::clearImmediate(Timer_base* t)
{
    return t->clear();
}

result_t timers_base::setInterval(v8::Local<v8::Function> callback,
    double timeout, OptArgs args, obj_ptr<Timer_base>& retVal)
{
    if (timeout < 1 || timeout > TIMEOUT_MAX)
        timeout = 1;

    obj_ptr<JSTimer> timer = new JSTimer(callback, args, (int32_t)timeout, true);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t timers_base::setHrInterval(v8::Local<v8::Function> callback,
    double timeout, OptArgs args, obj_ptr<Timer_base>& retVal)
{
    if (timeout < 1 || timeout > TIMEOUT_MAX)
        timeout = 1;

    obj_ptr<JSTimer> timer = new JSTimer(callback, args, (int32_t)timeout, true, true);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t timers_base::setTimeout(v8::Local<v8::Function> callback,
    double timeout, OptArgs args, obj_ptr<Timer_base>& retVal)
{
    if (timeout < 1 || timeout > TIMEOUT_MAX)
        timeout = 1;

    obj_ptr<JSTimer> timer = new JSTimer(callback, args, (int32_t)timeout);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t timers_base::setImmediate(v8::Local<v8::Function> callback,
    OptArgs args, obj_ptr<Timer_base>& retVal)
{
    obj_ptr<JSTimer> timer = new JSTimer(callback, args);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t global_base::clearInterval(Timer_base* t)
{
    return timers_base::clearInterval(t);
}

result_t global_base::clearHrInterval(Timer_base* t)
{
    return timers_base::clearHrInterval(t);
}

result_t global_base::clearTimeout(Timer_base* t)
{
    return timers_base::clearTimeout(t);
}

result_t global_base::clearImmediate(Timer_base* t)
{
    return timers_base::clearImmediate(t);
}

result_t global_base::setInterval(v8::Local<v8::Function> callback,
    double timeout, OptArgs args, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setInterval(callback, timeout, args, retVal);
}

result_t global_base::setHrInterval(v8::Local<v8::Function> callback,
    double timeout, OptArgs args, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setHrInterval(callback, timeout, args, retVal);
}

result_t global_base::setTimeout(v8::Local<v8::Function> callback,
    double timeout, OptArgs args, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setTimeout(callback, timeout, args, retVal);
}

result_t global_base::setImmediate(v8::Local<v8::Function> callback,
    OptArgs args, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setImmediate(callback, args, retVal);
}
}
