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

class AsyncResource_base : public object_base {
    DECLARE_CLASS(AsyncResource_base);

public:
    // AsyncResource_base
    static result_t _new(exlib::string type, v8::Local<v8::Value> triggerAsyncId, obj_ptr<AsyncResource_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t asyncId(double& retVal) = 0;
    virtual result_t triggerAsyncId(double& retVal) = 0;
    virtual result_t runInAsyncScope(v8::Local<v8::Function> fn, v8::Local<v8::Value> thisArg, OptArgs args, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t emitDestroy(obj_ptr<AsyncResource_base>& retVal) = 0;
    virtual result_t bind(v8::Local<v8::Function> fn, v8::Local<v8::Value> thisArg, v8::Local<v8::Function>& retVal) = 0;
    static result_t bind(v8::Local<v8::Function> fn, exlib::string type, v8::Local<v8::Value> thisArg, v8::Local<v8::Function>& retVal);

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<AsyncResource_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_asyncId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_triggerAsyncId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_runInAsyncScope(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_emitDestroy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& AsyncResource_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "asyncId", s_asyncId, false, ClassData::ASYNC_SYNC },
        { "triggerAsyncId", s_triggerAsyncId, false, ClassData::ASYNC_SYNC },
        { "runInAsyncScope", s_runInAsyncScope, false, ClassData::ASYNC_SYNC },
        { "emitDestroy", s_emitDestroy, false, ClassData::ASYNC_SYNC },
        { "bind", s_bind, false, ClassData::ASYNC_SYNC },
        { "bind", s_static_bind, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "AsyncResource", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void AsyncResource_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void AsyncResource_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AsyncResource_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t AsyncResource_base::load(v8::Local<v8::Value> v, obj_ptr<AsyncResource_base>& retVal)
{
    obj_ptr<AsyncResource_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    LOAD_RETURN();
}

inline void AsyncResource_base::s_asyncId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(AsyncResource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->asyncId(vr);

    METHOD_RETURN();
}

inline void AsyncResource_base::s_triggerAsyncId(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    double vr;

    METHOD_INSTANCE(AsyncResource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->triggerAsyncId(vr);

    METHOD_RETURN();
}

inline void AsyncResource_base::s_runInAsyncScope(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(AsyncResource_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate->m_isolate));
    ARG_LIST(2);

    hr = pInst->runInAsyncScope(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void AsyncResource_base::s_emitDestroy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AsyncResource_base> vr;

    METHOD_INSTANCE(AsyncResource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->emitDestroy(vr);

    METHOD_RETURN();
}

inline void AsyncResource_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(AsyncResource_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(v8::Local<v8::Value>, 1, v8::Undefined(isolate->m_isolate));

    hr = pInst->bind(v0, v1, vr);

    METHOD_RETURN();
}

inline void AsyncResource_base::s_static_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(exlib::string, 1, "bound-anonymous-fn");
    OPT_ARG(v8::Local<v8::Value>, 2, v8::Undefined(isolate->m_isolate));

    hr = bind(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
