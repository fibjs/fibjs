/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"
#include "ifs/vm.h"
#include "Chain.h"
#include "Routing.h"

#ifndef JSHANDLER_H_
#define JSHANDLER_H_

namespace fibjs {

class JSHandler : public Handler_base {

public:
    JSHandler(v8::Local<v8::Value> proc, bool async = false);

    FIBER_FREE();

public:
    // Handler_base
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal,
        AsyncEvent* ac);

public:
    static result_t New(v8::Local<v8::Value> hdlr, obj_ptr<Handler_base>& retVal)
    {
        if (hdlr->IsString() || hdlr->IsStringObject() || hdlr->IsNumberObject() || hdlr->IsRegExp()
            || (!hdlr->IsFunction() && !hdlr->IsObject()))
            return CHECK_ERROR(CALL_E_BADVARTYPE);

        retVal = Handler_base::getInstance(hdlr);
        if (retVal)
            return 0;

        if (hdlr->IsArray()) {
            v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(hdlr);

            obj_ptr<Chain_base> chain = new Chain();
            result_t hr = Chain_base::_new(a, chain);
            if (hr < 0)
                return hr;

            retVal = chain;
            return 0;
        }

        v8::Local<v8::Object> o = v8::Local<v8::Object>::Cast(hdlr);
        if (!hdlr->IsFunction()) {
            obj_ptr<Routing_base> routing;
            result_t hr = Routing_base::_new(o, routing);
            if (hr < 0)
                return hr;

            retVal = routing;
            return 0;
        }

        Isolate* isolate = Isolate::current();
        v8::Local<v8::Value> _async = o->GetPrivate(o->CreationContext(),
                                           v8::Private::ForApi(isolate->m_isolate, isolate->NewString("_async")))
                                          .ToLocalChecked();

        if (!IsEmpty(_async))
            retVal = new JSHandler(_async, true);
        else
            retVal = new JSHandler(hdlr);

        return 0;
    }

public:
    static result_t js_invoke(Handler_base* hdlr, object_base* v,
        obj_ptr<Handler_base>& retVal, AsyncEvent* ac);

private:
    bool m_async;
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
