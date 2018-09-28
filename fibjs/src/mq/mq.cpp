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
    v8::MaybeLocal<v8::Value> mv = hdlr->GetPrivate(hdlr->CreationContext(),
        v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")));

    v8::Local<v8::Value> v;
    if (!mv.IsEmpty())
        v = mv.ToLocalChecked();

    if (!IsEmpty(v))
        retVal = new JSHandler(v, true);
    else
        retVal = new JSHandler(hdlr);

    return 0;
}

result_t mq_base::invoke(Handler_base* hdlr, object_base* v,
    AsyncEvent* ac)
{
    if (ac->isSync())
        return CHECK_ERROR(CALL_E_NOSYNC);

    return (new Chain::asyncInvoke(hdlr, v, ac))->post(0);
}

result_t mq_base::nullHandler(obj_ptr<Handler_base>& retVal)
{
    retVal = new NullHandler();
    return 0;
}
}
