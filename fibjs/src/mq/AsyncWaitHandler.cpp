/*
 * AsyncWaitHandler.cpp
 *
 *  Created on: Dec 2, 2012
 *      Author: lion
 */

#include "object.h"
#include "AsyncWaitHandler.h"
#include "ifs/mq.h"

namespace fibjs
{

#define AC_INIT    0
#define AC_WAIT    1
#define AC_END     2

result_t AsyncWaitHandler::invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                                  AsyncEvent *ac)
{
    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    if (m_invoked.xchg(1) != 0)
        return 0;

    m_as = new asyncWaiter(ac);
    if (m_stat.CompareAndSwap(AC_INIT, AC_WAIT) == AC_INIT)
        return CHECK_ERROR(CALL_E_PENDDING);

    delete m_as;
    m_as = NULL;

    return CHECK_ERROR(CALL_RETURN_NULL);
}

result_t AsyncWaitHandler::end()
{
    if (m_stat.xchg(AC_END) == AC_WAIT)
    {
        m_as->async(CALL_E_NOSYNC);
        m_as = NULL;
    }

    return 0;
}

result_t mq_base::await(obj_ptr<Handler_base> &retVal)
{
    retVal = new AsyncWaitHandler();
    return 0;
}

class AsyncHandler: public Handler_base
{
public:
    AsyncHandler(v8::Local<v8::Function> proc)
    {
        SetPrivate("_proc", proc);
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            AsyncEvent *ac)
    {
        if (ac)
            return CHECK_ERROR(CALL_E_NOASYNC);

        v8::Local<v8::Value> v1;
        v1 = GetPrivate("_proc");
        if (IsEmpty(v1))
            return CALL_RETURN_NULL;

        Isolate* isolate = holder();

        retVal = new AsyncWaitHandler();
        v8::Local<v8::Function> proc = v8::Local<v8::Function>::Cast(v1);

        v8::Local<v8::Value> args[2];

        args[0] = v->wrap();
        args[1] = createV8Function("done", isolate->m_isolate, _done, retVal->wrap());

        proc->Call(args[0], 2, args);
        return 0;
    }

public:
    static void _done(const v8::FunctionCallbackInfo<v8::Value> &args)
    {
        AsyncWait_base *v = AsyncWait_base::getInstance(args.Data()->ToObject());
        if (v)
            v->end();
        args.GetReturnValue().SetUndefined();
    }
};

result_t mq_base::await(v8::Local<v8::Function> proc, obj_ptr<Handler_base>& retVal)
{
    retVal = new AsyncHandler(proc);
    return 0;
}

} /* namespace fibjs */
