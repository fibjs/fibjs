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

class EventSource_base;
class Handler_base;

class sse_base : public object_base {
    DECLARE_CLASS(sse_base);

public:
    enum {
        C_CONNECTING = 0,
        C_OPEN = 1,
        C_CLOSED = 2,
        C_SENDER = 3
    };

public:
    // sse_base
    static result_t upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<sse_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/EventSource.h"
#include "ifs/Handler.h"

namespace fibjs {
inline ClassInfo& sse_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "upgrade", s_static_upgrade, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "EventSource", EventSource_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "CONNECTING", C_CONNECTING },
        { "OPEN", C_OPEN },
        { "CLOSED", C_CLOSED },
        { "SENDER", C_SENDER }
    };

    static ClassData s_cd = {
        "sse", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void sse_base::s_static_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = upgrade(v0, vr);

    METHOD_RETURN();
}
}
