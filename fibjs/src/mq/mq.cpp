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
#include "HttpHandler.h"
#include "Chain.h"
#include "Routing.h"
#include "Fiber.h"

namespace fibjs {

DECLARE_MODULE(mq);

result_t Handler_base::_new(v8::Local<v8::Array> hdlrs, obj_ptr<Handler_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Chain_base> chain;
    result_t hr = Chain_base::_new(hdlrs, chain, This);
    if (hr < 0)
        return hr;

    retVal = chain;

    return 0;
}

result_t Handler_base::_new(v8::Local<v8::Object> map, obj_ptr<Handler_base>& retVal,
    v8::Local<v8::Object> This)
{
    obj_ptr<Routing_base> routing;
    result_t hr = Routing_base::_new(map, routing, This);
    if (hr < 0)
        return hr;

    retVal = routing;

    return 0;
}

result_t Handler_base::_new(v8::Local<v8::Function> hdlr, obj_ptr<Handler_base>& retVal,
    v8::Local<v8::Object> This)
{
    Isolate* isolate = Isolate::current();
    v8::Local<v8::Value> _async = hdlr->GetPrivate(hdlr->CreationContext(),
                                          v8::Private::ForApi(isolate->m_isolate, isolate->NewFromUtf8("_async")))
                                      .ToLocalChecked();

    if (!IsEmpty(_async))
        retVal = new JSHandler(_async, true);
    else
        retVal = new JSHandler(hdlr);

    return 0;
}

result_t mq_base::invoke(Handler_base* hdlr, object_base* v,
    AsyncEvent* ac)
{
    class asyncInvoke : public AsyncState {
    public:
        asyncInvoke(Handler_base* hdlr, object_base* v, AsyncEvent* ac)
            : AsyncState(ac)
            , m_next(hdlr)
            , m_v(v)
        {
            set(call);
        }

    public:
        static int32_t call(AsyncState* pState, int32_t n)
        {
            asyncInvoke* pThis = (asyncInvoke*)pState;
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

result_t mq_base::nullHandler(obj_ptr<Handler_base>& retVal)
{
    retVal = new NullHandler();
    return 0;
}
}
