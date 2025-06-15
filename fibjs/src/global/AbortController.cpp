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
    return m_signal->abort(reason, retVal);
}

result_t AbortSignal::abort(exlib::string reason, obj_ptr<AbortSignal_base>& retVal)
{
    retVal = this;
    if (m_aborted) {
        return 0;
    }

    m_aborted = true;
    m_reason = reason;

    obj_ptr<EventInfo> info = new EventInfo(this, "abort", 0, reason);
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

result_t AbortSignal::get_reason(exlib::string& retVal)
{
    if (!m_aborted)
        return CALL_RETURN_UNDEFINED;

    retVal = m_reason;
    return 0;
}

}
