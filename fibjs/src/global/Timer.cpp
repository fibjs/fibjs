/*
 * Timer.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/global.h"
#include "ifs/timers.h"
#include "Timer.h"

namespace fibjs {

#define TIMEOUT_MAX 2147483647 // 2^31-1

class JSTimer : public Timer {
public:
    JSTimer(v8::Local<v8::Function> callback, int32_t timeout = 0, bool repeat = false)
        : Timer(timeout, repeat)
        , m_clear_pendding(false)
    {
        Isolate* isolate = holder();

        isolate->m_pendding.inc();
        m_callback.Reset(isolate->m_isolate, callback);
    }

public:
    virtual void resume()
    {
        syncCall(holder(), _callback, this);
    }

public:
    virtual void on_timer()
    {
        JSFiber::scope s;
        v8::Local<v8::Function> callback = v8::Local<v8::Function>::New(holder()->m_isolate, m_callback);
        callback->Call(wrap(), 0, NULL);
    }

    virtual void on_clean()
    {
        m_callback.Reset();
        if (!m_clear_pendding) {
            m_clear_pendding = true;
            holder()->m_pendding.dec();
        }
    }

private:
    bool m_clear_pendding;
    v8::Global<v8::Function> m_callback;
};

DECLARE_MODULE(timers);

result_t timers_base::clearInterval(Timer_base* t)
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

result_t timers_base::setInterval(v8::Local<v8::Function> callback, double timeout, obj_ptr<Timer_base>& retVal)
{
    if (timeout < 1 || timeout > TIMEOUT_MAX)
        timeout = 1;

    obj_ptr<Timer> timer = new JSTimer(callback, timeout, true);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t timers_base::setTimeout(v8::Local<v8::Function> callback, 
    double timeout, obj_ptr<Timer_base>& retVal)
{
    if (timeout < 1 || timeout > TIMEOUT_MAX)
        timeout = 1;

    obj_ptr<Timer> timer = new JSTimer(callback, timeout);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t timers_base::setImmediate(v8::Local<v8::Function> callback, obj_ptr<Timer_base>& retVal)
{
    obj_ptr<Timer> timer = new JSTimer(callback);
    timer->sleep();
    retVal = timer;

    return 0;
}

result_t global_base::clearInterval(Timer_base* t)
{
    return timers_base::clearInterval(t);
}

result_t global_base::clearTimeout(Timer_base* t)
{
    return timers_base::clearTimeout(t);
}

result_t global_base::clearImmediate(Timer_base* t)
{
    return timers_base::clearImmediate(t);
}

result_t global_base::setInterval(v8::Local<v8::Function> callback, double timeout, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setInterval(callback, timeout, retVal);
}

result_t global_base::setTimeout(v8::Local<v8::Function> callback, double timeout, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setTimeout(callback, timeout, retVal);
}

result_t global_base::setImmediate(v8::Local<v8::Function> callback, obj_ptr<Timer_base>& retVal)
{
    return timers_base::setImmediate(callback, retVal);
}
}
