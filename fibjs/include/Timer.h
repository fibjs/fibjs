/*
 * Timer.h
 *
 *  Created on: Sun 14, 2016
 *      Author: lion
 */

#include "ifs/Timer.h"
#include "Fiber.h"
#include <vector>

#ifndef TIMER_H_
#define TIMER_H_

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

        if (m_hr)
            isolate->RequestInterrupt(_InterruptCallback, this);
        else
            syncCall(isolate, _callback, this);
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
            on_js_timer();
        else {
            JSFiber::scope s;
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
}

#endif
