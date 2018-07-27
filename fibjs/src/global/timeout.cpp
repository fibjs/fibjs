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

class FuncTimer : public JSTimer {
public:
    FuncTimer(JSFiber* fiber, int32_t timeout)
        : JSTimer(timeout, false, true)
        , m_fiber(fiber)
    {
    }

public:
    virtual void on_js_timer()
    {
        m_fiber->m_termed = true;
        if (JSFiber::current() == m_fiber)
            m_fiber->holder()->m_isolate->TerminateExecution();
    }

private:
    obj_ptr<JSFiber> m_fiber;
};

result_t timers_base::call(v8::Local<v8::Function> func, double timeout,
    OptArgs args, v8::Local<v8::Value>& retVal)
{
    if (timeout < 1 || timeout > TIMEOUT_MAX)
        timeout = 1;

    Isolate* isolate = Isolate::current();
    obj_ptr<JSFiber> this_fiber = JSFiber::current();
    std::vector<v8::Local<v8::Value>> argv;

    args.GetData(argv);

    obj_ptr<JSTimer> timer = new FuncTimer(this_fiber, (int32_t)timeout);

    TryCatch try_catch;

    timer->sleep();
    retVal = func->Call(v8::Undefined(isolate->m_isolate), (int32_t)argv.size(), argv.data());
    timer->clear();

    if (try_catch.HasCaught()) {
        if (this_fiber->m_termed) {
            this_fiber->m_termed = false;
            isolate->m_isolate->CancelTerminateExecution();
            return CHECK_ERROR(CALL_E_TIMEOUT);
        } else {
            try_catch.ReThrow();
            return CHECK_ERROR(CALL_E_JAVASCRIPT);
        }
    }

    return 0;
}
}
