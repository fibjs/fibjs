/*
 * AbortController.cpp
 *
 *  Created on: Jul 13, 2024
 *      Author: lion
 */

#include "object.h"
#include "AbortController.h"
#include "EventInfo.h"

namespace fibjs {
result_t AbortSignal_base::timeout(double ms, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);

    int32_t msInt = (ms < 1) ? 1 : (ms > TIMEOUT_MAX) ? TIMEOUT_MAX : (int32_t)ms;
    obj_ptr<AbortTimer> timer = new AbortTimer(signal, msInt);
    signal->setTimer(timer);
    timer->sleep();

    retVal = signal;
    return 0;
}

result_t AbortController_base::_new(obj_ptr<AbortController_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new AbortController(Isolate::current(This));
    return 0;
}

result_t AbortController::get_signal(obj_ptr<AbortSignal_base>& retVal)
{
    retVal = m_signal;
    return 0;
}

result_t AbortController::abort(exlib::string reason)
{
    obj_ptr<AbortSignal_base> retVal;
    return m_signal->do_abort(reason, retVal);
}

result_t AbortController::abort(v8::Local<v8::Value> reason)
{
    obj_ptr<AbortSignal_base> retVal;
    return m_signal->do_abort(reason, retVal);
}

result_t AbortSignal_base::abort(exlib::string reason, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);
    return signal->do_abort(reason, retVal);
}

result_t AbortSignal_base::abort(v8::Local<v8::Value> reason, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);
    return signal->do_abort(reason, retVal);
}

result_t AbortSignal::do_abort(exlib::string reason, obj_ptr<AbortSignal_base>& retVal)
{
    retVal = this;
    if (m_aborted) {
        return 0;
    }

    m_aborted = true;
    m_reason = reason;
    m_has_value_reason = false;

    obj_ptr<EventInfo> info = new EventInfo(this, "abort", 0, m_reason);
    v8::Local<v8::Value> _info;
    info->valueOf(_info);

    bool r;
    return _emit("abort", &_info, 1, r);
}

result_t AbortSignal::do_abort(v8::Local<v8::Value> reason, obj_ptr<AbortSignal_base>& retVal)
{
    retVal = this;
    if (m_aborted) {
        return 0;
    }

    m_aborted = true;
    m_reason.clear();

    if (reason->IsUndefined()) {
        m_reason = "AbortError";
        m_has_value_reason = false;
    } else {
        SetPrivate("reason", reason);
        m_has_value_reason = true;
    }

    obj_ptr<EventInfo> info = new EventInfo(this, "abort");

    v8::Local<v8::Value> _info;
    info->valueOf(_info);

    bool r;
    return _emit("abort", &_info, 1, r);
}

result_t AbortSignal::throwIfAborted()
{
    if (m_aborted) {
        return Runtime::setError(m_reason);
    }
    return 0;
}

result_t AbortSignal::get_aborted(bool& retVal)
{
    retVal = m_aborted;
    return 0;
}

result_t AbortSignal::get_reason(v8::Local<v8::Value>& retVal)
{
    if (!m_aborted)
        return CALL_RETURN_UNDEFINED;

    if (m_has_value_reason)
        retVal = GetPrivate("reason");
    else
        retVal = Isolate::current()->NewString(m_reason);

    return 0;
}

}
