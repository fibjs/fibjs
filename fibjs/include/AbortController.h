/*
 * AbortController.h
 *
 *  Created on: Jul 13, 2024
 *      Author: lion
 */

#pragma once

#include "object.h"
#include "ifs/AbortController.h"
#include "Timer.h"
#include "v8.h"

namespace fibjs {

class AbortSignal : public AbortSignal_base {
public:
    // AbortSignal_base
    virtual result_t throwIfAborted();
    virtual result_t get_aborted(bool& retVal);
    virtual result_t get_reason(v8::Local<v8::Value>& retVal);

public:
    // Internal abort methods (called by AbortController)
    result_t do_abort(exlib::string reason, obj_ptr<AbortSignal_base>& retVal);
    result_t do_abort(v8::Local<v8::Value> reason, obj_ptr<AbortSignal_base>& retVal);

public:
    // object_base
    virtual result_t onEventEmit(exlib::string ev)
    {
        if (ev == "abort") {
            std::vector<std::function<void()>> tmp;
            m_lock.lock();
            m_aborted = true;
            m_callbacks.swap(tmp);
            m_lock.unlock();
            for (const auto& callback : tmp)
                callback();
        }
        return 0;
    }

public:
    bool is_aborted() const
    {
        return m_aborted;
    }

    void addAbortCallback(std::function<void()> callback)
    {
        // check-and-push must be atomic to avoid missing a callback that fires
        // between the aborted check and the push_back
        bool call_now = false;
        m_lock.lock();
        if (m_aborted)
            call_now = true;
        else
            m_callbacks.push_back(std::move(callback));
        m_lock.unlock();
        if (call_now)
            callback();
    }

    void setTimer(JSTimer* timer) { m_timer = timer; }

    // Clear only the C++ abort callbacks, leaving the timer alive.
    // Used when transitioning from request-phase to body-phase.
    void clearCallbacks()
    {
        std::vector<std::function<void()>> tmp;
        m_lock.lock();
        m_callbacks.swap(tmp);
        m_lock.unlock();
    }

    // Clear both callbacks and the timer.
    void clearAbort()
    {
        clearCallbacks();
        if (m_timer) {
            m_timer->clear();
            m_timer.Release();
        }
    }

private:
    exlib::spinlock m_lock;
    std::vector<std::function<void()>> m_callbacks;
    obj_ptr<JSTimer> m_timer;
    std::atomic_bool m_aborted = false;
    exlib::string m_reason;
    bool m_has_value_reason = false; // 标识是否使用了 v8::Value reason
};

// Timer subclass that aborts an AbortSignal when it fires.
class AbortTimer : public JSTimer {
public:
    AbortTimer(obj_ptr<AbortSignal> signal, int32_t ms)
        : JSTimer(ms)
        , m_signal(signal)
    {
    }

    virtual void on_js_timer() override
    {
        obj_ptr<AbortSignal_base> dummy;
        m_signal->do_abort("TimeoutError", dummy);
        m_signal->clearAbort(); // clear callbacks and self-reference after firing
    }

private:
    obj_ptr<AbortSignal> m_signal;
};

class AbortController : public AbortController_base {
public:
    AbortController(Isolate* isolate)
    {
        holder(isolate);
        m_signal = new AbortSignal();
        m_signal->holder(isolate);
    }

public:
    // AbortController_base
    virtual result_t get_signal(obj_ptr<AbortSignal_base>& retVal);
    virtual result_t abort(exlib::string reason);
    virtual result_t abort(v8::Local<v8::Value> reason);

private:
    obj_ptr<AbortSignal> m_signal;
};

}
