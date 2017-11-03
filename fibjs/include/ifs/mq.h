/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _mq_base_H_
#define _mq_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Message_base;
class HttpHandler_base;
class Handler_base;
class Chain_base;
class Routing_base;

class mq_base : public object_base {
    DECLARE_CLASS(mq_base);

public:
    // mq_base
    static result_t nullHandler(obj_ptr<Handler_base>& retVal);
    static result_t invoke(Handler_base* hdlr, object_base* v, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_nullHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATIC2(mq_base, invoke, Handler_base*, object_base*);
};
}

#include "Message.h"
#include "HttpHandler.h"
#include "Handler.h"
#include "Chain.h"
#include "Routing.h"

namespace fibjs {
inline ClassInfo& mq_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "nullHandler", s_nullHandler, true },
        { "invoke", s_invoke, true },
        { "invokeSync", s_invoke, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Message", Message_base::class_info },
        { "HttpHandler", HttpHandler_base::class_info },
        { "Handler", Handler_base::class_info },
        { "Chain", Chain_base::class_info },
        { "Routing", Routing_base::class_info }
    };

    static ClassData s_cd = {
        "mq", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void mq_base::s_nullHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_NAME("mq.nullHandler");
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = nullHandler(vr);

    METHOD_RETURN();
}

inline void mq_base::s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("mq.invoke");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Handler_base>, 0);
    ARG(obj_ptr<object_base>, 1);

    if (!cb.IsEmpty()) {
        acb_invoke(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = ac_invoke(v0, v1);

    METHOD_VOID();
}
}

#endif
