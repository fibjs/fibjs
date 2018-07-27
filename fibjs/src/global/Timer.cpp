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

DECLARE_MODULE(timers);

result_t timers_base::clearInterval(v8::Local<v8::Value> t)
{
    obj_ptr<Timer_base> _t = Timer_base::getInstance(t);
    if (!_t)
        return 0;

    return _t->clear();
}

result_t timers_base::clearHrInterval(v8::Local<v8::Value> t)
{
    obj_ptr<Timer_base> _t = Timer_base::getInstance(t);
    if (!_t)
        return 0;

    return _t->clear();
}

result_t timers_base::clearTimeout(v8::Local<v8::Value> t)
{
    obj_ptr<Timer_base> _t = Timer_base::getInstance(t);
    if (!_t)
        return 0;

    return _t->clear();
}

result_t timers_base::clearImmediate(v8::Local<v8::Value> t)
{
    obj_ptr<Timer_base> _t = Timer_base::getInstance(t);
    if (!_t)
        return 0;

    return _t->clear();
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

result_t global_base::clearInterval(v8::Local<v8::Value> t)
{
    return timers_base::clearInterval(t);
}

result_t global_base::clearHrInterval(v8::Local<v8::Value> t)
{
    return timers_base::clearHrInterval(t);
}

result_t global_base::clearTimeout(v8::Local<v8::Value> t)
{
    return timers_base::clearTimeout(t);
}

result_t global_base::clearImmediate(v8::Local<v8::Value> t)
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
