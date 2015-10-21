/*
 * mq.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "JSHandler.h"
#include "ifs/mq.h"
#include "NullHandler.h"
#include "AsyncWaitHandler.h"
#include "HttpHandler.h"
#include "Chain.h"
#include "Routing.h"
#include "Fiber.h"

namespace fibjs
{

result_t mq_base::invoke(Handler_base *hdlr, object_base *v,
                         AsyncEvent *ac)
{
    class asyncInvoke: public AsyncState
    {
    public:
        asyncInvoke(Handler_base *hdlr, object_base *v, AsyncEvent *ac) :
            AsyncState(ac), m_next(hdlr), m_v(v)
        {
            set(call);
        }

    public:
        static int32_t call(AsyncState *pState, int32_t n)
        {
            asyncInvoke *pThis = (asyncInvoke *) pState;
            result_t hr;

            if (n == CALL_RETURN_NULL)
                return pThis->done();

            pThis->m_hdlr = pThis->m_next;
            pThis->m_next.Release();

            hr = pThis->m_hdlr->invoke(pThis->m_v, pThis->m_next, pThis);
            if (hr == CALL_E_NOASYNC)
                return JSHandler::js_invoke(pThis->m_hdlr, pThis->m_v, pThis->m_next,
                                            pThis);

            return hr;
        }

    private:
        obj_ptr<Handler_base> m_hdlr;
        obj_ptr<Handler_base> m_next;
        obj_ptr<object_base> m_v;
    };

    if (!ac)
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new asyncInvoke(hdlr, v, ac))->post(0);
}

result_t mq_base::jsHandler(v8::Local<v8::Value> hdlr,
                            obj_ptr<Handler_base> &retVal)
{
    if ((retVal = Handler_base::getInstance(hdlr)) != NULL)
        return 0;

    return JSHandler::New(hdlr, retVal);
}

result_t mq_base::await(obj_ptr<AsyncWait_base> &retVal)
{
    retVal = new AsyncWaitHandler();
    return 0;
}

result_t mq_base::nullHandler(obj_ptr<Handler_base> &retVal)
{
    retVal = new NullHandler();
    return 0;
}

}
