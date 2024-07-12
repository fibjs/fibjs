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
    retVal = new AbortController();
    return 0;
}

result_t AbortController::get_signal(obj_ptr<AbortSignal_base>& retVal)
{
    retVal = m_signal;
    return 0;
}

result_t AbortController::abort(exlib::string reason)
{
    return m_signal->abort(reason);
}

result_t AbortSignal::abort(exlib::string reason)
{
    bool retVal;
    obj_ptr<EventInfo> info = new EventInfo(this, "abort", 0, reason);
    v8::Local<v8::Value> _info;
    info->valueOf(_info);

    return _emit("abort", &_info, 1, retVal);
}

result_t AbortSignal::get_aborted(bool& retVal)
{
    retVal = m_aborted;
    return 0;
}

}
