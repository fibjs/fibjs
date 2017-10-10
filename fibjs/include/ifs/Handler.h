/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Handler_base_H_
#define _Handler_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Handler_base : public object_base {
    DECLARE_CLASS(Handler_base);

public:
    // Handler_base
    static result_t _new(v8::Local<v8::Array> hdlrs, obj_ptr<Handler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> map, obj_ptr<Handler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Function> hdlr, obj_ptr<Handler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t invoke(object_base* v, obj_ptr<Handler_base>& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(Handler_base, invoke, object_base*, obj_ptr<Handler_base>);
};
}

namespace fibjs {
inline ClassInfo& Handler_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "invoke", s_invoke, false },
        { "invokeSync", s_invoke, false }
    };

    static ClassData s_cd = {
        "Handler", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Handler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Handler_base::__new(const T& args)
{
    obj_ptr<Handler_base> vr;

    CONSTRUCT_ENTER("new Handler()");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Handler_base::s_invoke(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_INSTANCE(Handler_base);
    METHOD_ENTER("Handler.invoke");

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<object_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_invoke(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_invoke(v0, vr);

    METHOD_RETURN();
}
}

#endif
