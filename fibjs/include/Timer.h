/*
 * Timer.h
 *
 *  Created on: Sun 14, 2016
 *      Author: lion
 */

#pragma once

#include "ifs/Timer.h"
#include "Fiber.h"
#include <vector>

namespace fibjs {

#define TIMEOUT_MAX 2147483647 // 2^31-1

class Timer : public Timer_base,
              public exlib::Task_base {
public:
    Timer(int32_t timeout = 0, bool repeat = false)
        : m_timeout(timeout)
        , m_repeat(repeat)
        , m_cancel(0)
    {
        Ref();
    }

public:
    // Timer_base
    virtual result_t ref(obj_ptr<Timer_base>& retVal)
    {
        return 0;
    }

    virtual result_t unref(obj_ptr<Timer_base>& retVal)
    {
        return 0;
    }

    virtual result_t clear()
    {
        if (m_cancel.CompareAndSwap(0, 1) == 0)
            exlib::Fiber::cancel_sleep(this);

        return 0;
    }

    virtual result_t get_stopped(bool& retVal)
    {
        retVal = m_cancel != 0;
        return 0;
    }

public:
    // exlib::Task_base
    virtual void resume()
    {
        asyncCall(_callback, this);
    }

    void sleep()
    {
        if (m_timeout > 0)
            exlib::Fiber::sleep(m_timeout, this);
        else
            resume();
    }

    bool is_stopped()
    {
        return m_cancel != 0;
    }

public:
    virtual void on_timer()
    {
    }

    virtual void on_clean()
    {
    }

protected:
    result_t callback()
    {
        if (!m_cancel) {
            on_timer();

            if (m_repeat && !m_cancel)
                sleep();
            else {
                m_cancel = 1;
                on_clean();
                Unref();
            }
        } else {
            on_clean();
            Unref();
        }

        return 0;
    }

    static result_t _callback(Timer* pThis)
    {
        return pThis->callback();
    }

private:
    int32_t m_timeout;
    bool m_repeat;
    exlib::atomic m_cancel;
};

class JSTimer : public Timer {
public:
    JSTimer(v8::Local<v8::Function> callback, OptArgs& args, int32_t timeout = 0,
        bool repeat = false, bool hr = false)
        : Timer(timeout, repeat)
        , m_hr(hr)
    {
        Isolate* isolate = holder();
        obj_ptr<Timer_base> retVal;

        int32_t nArgCount = args.Length();
        m_argv.resize(nArgCount);
        for (int i = 0; i < nArgCount; i++)
            m_argv[i].Reset(isolate->m_isolate, args[i]);

        ref(retVal);

        m_callback.Reset(isolate->m_isolate, callback);
    }

protected:
    JSTimer(int32_t timeout = 0, bool repeat = false, bool hr = false)
        : Timer(timeout, repeat)
        , m_hr(hr)
    {
        obj_ptr<Timer_base> retVal;
        ref(retVal);
    }

public:
    virtual void resume()
    {
        Isolate* isolate = holder();

        if (is_stopped())
            m_hr = false;

        if (m_hr)
            isolate->RequestInterrupt(_InterruptCallback, this);
        else
            isolate->sync([this]() -> int {
                return callback();
            });
    }

    static void _InterruptCallback(v8::Isolate* isolate, void* data)
    {
        _callback((JSTimer*)data);
    }

public:
    // Timer_base
    virtual result_t ref(obj_ptr<Timer_base>& retVal)
    {
        isolate_ref();
        retVal = this;
        return 0;
    }

    virtual result_t unref(obj_ptr<Timer_base>& retVal)
    {
        isolate_unref();
        retVal = this;
        return 0;
    }

public:
    virtual void on_js_timer()
    {
        Isolate* isolate = holder();
        v8::Local<v8::Function> callback = m_callback.Get(isolate->m_isolate);
        std::vector<v8::Local<v8::Value>> argv;

        int32_t nArgCount = (int32_t)m_argv.size();
        argv.resize(nArgCount);
        for (int i = 0; i < nArgCount; i++)
            argv[i] = m_argv[i].Get(isolate->m_isolate);

        callback->Call(callback->GetCreationContextChecked(), wrap(), (int32_t)argv.size(), argv.data()).IsEmpty();
    }

    virtual void on_timer()
    {
        if (m_hr)
            on_js_timer();
        else {
            JSFiber::EnterJsScope s;
            on_js_timer();
        }
    }

    virtual void on_clean()
    {
        obj_ptr<Timer_base> retVal;

        m_callback.Reset();
        unref(retVal);
    }

private:
    bool m_hr;
    QuickArray<v8::Global<v8::Value>> m_argv;
    v8::Global<v8::Function> m_callback;
};

class TimeoutScope {
private:
    class FuncTimer : public JSTimer {
    public:
        FuncTimer(JSFiber* fiber, int32_t timeout)
            : JSTimer(timeout, false, true)
            , m_fiber(fiber)
        {
        }

    public:
        virtual void on_js_timer()
        {
            m_fiber->m_termed = true;
            if (JSFiber::current() == m_fiber)
                m_fiber->holder()->m_isolate->TerminateExecution();
        }

    private:
        obj_ptr<JSFiber> m_fiber;
    };

public:
    TimeoutScope(double timeout)
        : m_isolate(Isolate::current())
        , this_fiber(JSFiber::current())
    {
        if (timeout < 1 || timeout > TIMEOUT_MAX)
            timeout = 1;

        m_timer = new FuncTimer(this_fiber, (int32_t)timeout);
        m_timer->sleep();
    }

    result_t result(result_t hr = 0)
    {
        m_timer->clear();

        if (this_fiber->m_termed) {
            this_fiber->m_termed = false;
            m_isolate->m_isolate->CancelTerminateExecution();
            return CHECK_ERROR(CALL_E_TIMEOUT);
        }

        return hr;
    }

public:
    Isolate* m_isolate;
    obj_ptr<JSFiber> this_fiber;
    obj_ptr<JSTimer> m_timer;
};
}
