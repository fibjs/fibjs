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
result_t AbortSignal_base::any(std::vector<obj_ptr<AbortSignal_base>>& signals, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);

    for (size_t i = 0; i < signals.size(); i++) {
        obj_ptr<AbortSignal_base> src = signals[i];
        if (!src)
            return result_t::type_mismatch();

        AbortSignal* src_signal = static_cast<AbortSignal*>(src.get());
        if (src_signal->is_aborted()) {
            v8::Local<v8::Value> reason;
            src_signal->get_reason(reason);
            return signal->do_abort(reason, retVal);
        }

        obj_ptr<AbortSignal> composite = signal;
        src_signal->addAbortCallback([composite, src]() {
            if (composite->is_aborted())
                return;
            v8::Local<v8::Value> reason;
            static_cast<AbortSignal*>(src.get())->get_reason(reason);
            obj_ptr<AbortSignal_base> dummy;
            composite->do_abort(reason, dummy);
        });
    }

    retVal = signal;
    return result_t();
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
    return result_t();
}

result_t AbortController_base::_new(obj_ptr<AbortController_base>& retVal, v8::Local<v8::Object> This)
{
    retVal = new AbortController(Isolate::current(This));
    return result_t();
}

result_t AbortController::get_signal(obj_ptr<AbortSignal_base>& retVal)
{
    retVal = m_signal;
    return result_t();
}

result_t AbortController::abort(Union_abort_reason reason)
{
    if (std::holds_alternative<exlib::string>(reason))
        return abort(std::get<exlib::string>(reason));

    return abort(std::get<v8::Local<v8::Value>>(reason));
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

static result_t abort_signal_string(exlib::string reason, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);
    return signal->do_abort(reason, retVal);
}

static result_t abort_signal_value(v8::Local<v8::Value> reason, obj_ptr<AbortSignal_base>& retVal)
{
    Isolate* isolate = Isolate::current();
    obj_ptr<AbortSignal> signal = new AbortSignal();
    signal->holder(isolate);
    return signal->do_abort(reason, retVal);
}

result_t AbortSignal_base::abort(Union_abort_reason reason, obj_ptr<AbortSignal_base>& retVal)
{
    if (std::holds_alternative<exlib::string>(reason))
        return abort_signal_string(std::get<exlib::string>(reason), retVal);

    return abort_signal_value(std::get<v8::Local<v8::Value>>(reason), retVal);
}

result_t AbortSignal::do_abort(exlib::string reason, obj_ptr<AbortSignal_base>& retVal)
{
    retVal = this;
    if (m_aborted) {
        return result_t();
    }

    m_aborted = true;
    m_reason = reason;
    m_has_value_reason = false;
    m_value_reason = NULL;

    obj_ptr<EventInfo> info = new EventInfo(this, "abort", result_t(), m_reason);
    v8::Local<v8::Value> _info;
    info->valueOf(_info);

    bool r;
    return _emit("abort", &_info, 1, r);
}

result_t AbortSignal::do_abort(v8::Local<v8::Value> reason, obj_ptr<AbortSignal_base>& retVal)
{
    retVal = this;
    if (m_aborted) {
        return result_t();
    }

    m_aborted = true;
    m_reason.clear();
    m_value_reason = NULL;

    if (reason->IsUndefined()) {
        m_reason = "AbortError";
        m_has_value_reason = false;
    } else {
        // Hold the value natively: the reason has to outlive the JS wrapper.
        Isolate* isolate = holder();
        if (!isolate)
            isolate = Isolate::current();

        m_value_reason = new ValueHolder(reason, isolate);
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
                return result_t::make(CALL_E_TIMEOUT).with_type_name(errtype::kTimeoutError).with_code("TIMEOUT_ERR").with_message("The operation timed out.");
            return result_t::make(CALL_E_ABORT).with_type_name(errtype::kAbortError).with_code("ABORT_ERR").with_message("The operation was aborted.");
        }
        return result_t::make(CALL_E_ABORT).with_type_name(errtype::kAbortError).with_code("ABORT_ERR").with_message(m_reason);
    }
    return result_t();
}

result_t AbortSignal::get_aborted(bool& retVal)
{
    retVal = m_aborted;
    return result_t();
}

result_t AbortSignal::get_reason(v8::Local<v8::Value>& retVal)
{
    if (!m_aborted)
        return result_t::return_undefined();

    if (m_has_value_reason) {
        if (m_value_reason)
            retVal = m_value_reason->value();
        else
            retVal = v8::Undefined(Isolate::current()->m_isolate);
    } else
        retVal = Isolate::current()->NewString(m_reason);

    return result_t();
}

}
