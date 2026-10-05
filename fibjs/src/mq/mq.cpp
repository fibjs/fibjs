/*
 * mq.cpp
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "object.h"
#include "JSHandler.h"
#include "ifs/mq.h"
#include "ifs/HttpRepeater.h"
#include "NullHandler.h"
#include "HttpHandler.h"
#include "Chain.h"
#include "union_helpers.h"
#include "Routing.h"
#include "HttpFileHandler.h"

namespace fibjs {

DECLARE_MODULE(mq);

result_t Handler_base::_new(exlib::string hdlr, obj_ptr<Handler_base>& retVal,
    v8::Local<v8::Object> This)
{
    if (!qstrcmp(hdlr.c_str(), "http:", 5) || !qstrcmp(hdlr.c_str(), "https:", 6)) {
        obj_ptr<HttpRepeater_base> repeater;
        result_t hr = HttpRepeater_base::_new(hdlr, repeater, This);
        if (hr < 0)
            return hr;

        retVal = repeater;
    } else
        return HttpFileHandler::create(hdlr, false, retVal);

    return 0;
}

result_t Handler_base::_new(std::vector<obj_ptr<Handler_base>>& hdlrs, obj_ptr<Handler_base>& retVal,
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
    Isolate* isolate = Isolate::current(hdlr);

    v8::Local<v8::Context> hdlr_context;
    if (!hdlr->GetCreationContext().ToLocal(&hdlr_context))
        hdlr_context = isolate->context();

    JSValue v = hdlr->GetPrivate(hdlr_context,
        v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")));

    if (!IsEmpty(v))
        retVal = new JSHandler(v, true);
    else
        retVal = new JSHandler(hdlr);

    return 0;
}

result_t mq_base::invoke(Union_invoke_hdlr hdlr, object_base* v,
    AsyncEvent* ac)
{
    // the async-aware helper keeps the phase logic inside: the alternatives
    // whose conversion needs V8/Isolate (the callback, the routing map, the
    // array and the address string) are converted in the sync phase and
    // carried in m_ctx; the class alternative is resolved in the async phase,
    // so a cc_ caller can pass it directly
    // (plans/async-phase-discipline-audit-2026-10-05.md §4.5)
    obj_ptr<Handler_base> handler;
    result_t hr = handler_from_union(hdlr, handler, ac);
    if (hr < 0)
        return hr;

    return (new Chain::asyncInvoke(handler, v, ac))->post(0);
}

result_t mq_base::nullHandler(obj_ptr<Handler_base>& retVal)
{
    retVal = new NullHandler();
    return 0;
}
}
