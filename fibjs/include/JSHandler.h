/*
 * JSHandler.h
 *
 *  Created on: Aug 25, 2012
 *      Author: lion
 */

#include "ifs/Handler.h"
#include "ifs/vm.h"

#ifndef JSHANDLER_H_
#define JSHANDLER_H_

namespace fibjs
{

class JSHandler: public Handler_base
{
    FIBER_FREE();

public:
    // object_base
    virtual result_t dispose()
    {
        return 0;
    }

public:
    // Handler_base
    virtual result_t invoke(object_base *v, obj_ptr<Handler_base> &retVal,
                            AsyncEvent *ac);

public:
    static result_t New(v8::Local<v8::Value> hdlr,
                        obj_ptr<Handler_base> &retVal)
    {
        if (hdlr->IsString() || hdlr->IsStringObject() ||
                hdlr->IsNumberObject() || hdlr->IsRegExp() ||
                (!hdlr->IsFunction() && !hdlr->IsObject()))
            return CHECK_ERROR(CALL_E_BADVARTYPE);

        retVal = Handler_base::getInstance(hdlr);
        if (retVal)
            return 0;

        obj_ptr<JSHandler> r = new JSHandler();
        r->wrap()->SetHiddenValue(v8::String::NewFromUtf8(r->holder()->m_isolate, "handler"), hdlr);

        retVal = r;
        return 0;
    }

public:
    static result_t js_invoke(Handler_base *hdlr, object_base *v,
                              obj_ptr<Handler_base> &retVal, AsyncEvent *ac);
};

} /* namespace fibjs */
#endif /* JSHANDLER_H_ */
