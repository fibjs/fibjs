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
result_t AbortSignal_base::any(v8::Local<v8::Array> signals, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);

    uint32_t len = signals->Length();
    for (uint32_t i = 0; i < len; i++) {
        v8::Local<v8::Value> item;
        if (!signals->Get(isolate->context(), i).ToLocal(&item))
            return CALL_E_JAVASCRIPT;

        AbortSignal_base* src = AbortSignal_base::getInstance(item);
        if (!src)
            return CHECK_ERROR(CALL_E_TYPEMISMATCH);

        AbortSignal* src_signal = static_cast<AbortSignal*>(src);
        if (src_signal->is_aborted()) {
            v8::Local<v8::Value> reason;
            src_signal->get_reason(reason);
            return signal->do_abort(reason, retVal);
        }

        obj_ptr<AbortSignal> composite = signal;
        obj_ptr<AbortSignal_base> src_ref = src;
        src_signal->addAbortCallback([composite, src_ref]() {
            if (composite->is_aborted())
                return;
            v8::Local<v8::Value> reason;
            static_cast<AbortSignal*>((AbortSignal_base*)src_ref)->get_reason(reason);
            obj_ptr<AbortSignal_base> dummy;
            composite->do_abort(reason, dummy);
        });
    }

    retVal = signal;
    return 0;
}

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
        // WHATWG: throw the abort reason, which is a DOMException named
        // AbortError (or TimeoutError for AbortSignal.timeout()) by default.
        if (!m_has_value_reason) {
            if (m_reason == "TimeoutError")
                return Runtime::setError(kTimeoutError, "The operation timed out.");
            return Runtime::setError(kAbortError, "The operation was aborted.");
        }
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
