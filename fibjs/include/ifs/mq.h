/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#pragma once

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

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_nullHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_invoke(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATIC2(mq_base, invoke, Handler_base*, object_base*);
};
}

#include "ifs/Message.h"
#include "ifs/HttpHandler.h"
#include "ifs/Handler.h"
#include "ifs/Chain.h"
#include "ifs/Routing.h"

namespace fibjs {
inline ClassInfo& mq_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "nullHandler", s_static_nullHandler, true, ClassData::ASYNC_SYNC },
        { "invoke", s_static_invoke, true, ClassData::ASYNC_ASYNC }
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
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void mq_base::s_static_nullHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = nullHandler(vr);

    METHOD_RETURN();
}

inline void mq_base::s_static_invoke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Handler_base>, 0);
    ARG(obj_ptr<object_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_invoke(v0, v1, cb, args);
    else
        hr = ac_invoke(v0, v1);

    METHOD_VOID();
}
}
