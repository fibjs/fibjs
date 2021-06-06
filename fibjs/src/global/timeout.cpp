/*
 * Timer.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: lion
 */

#include "object.h"
#include "ifs/timers.h"
#include "Timer.h"

namespace fibjs {

result_t timers_base::call(v8::Local<v8::Function> func, double timeout,
    OptArgs args, v8::Local<v8::Value>& retVal)
{
    std::vector<v8::Local<v8::Value>> argv;
    args.GetData(argv);

    TimeoutScope ts(timeout);
    func->Call(func->CreationContext(), v8::Undefined(ts.m_isolate->m_isolate), (int32_t)argv.size(), argv.data()).ToLocal(&retVal);
    return ts.result(retVal.IsEmpty() ? CALL_E_JAVASCRIPT : 0);
}
}
