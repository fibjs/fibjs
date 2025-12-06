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

class AsyncLocalStorage_base : public object_base {
    DECLARE_CLASS(AsyncLocalStorage_base);

public:
    // AsyncLocalStorage_base
    static result_t _new(v8::Local<v8::Object> options, obj_ptr<AsyncLocalStorage_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    static result_t snapshot(v8::Local<v8::Function>& retVal);
    static result_t bind(v8::Local<v8::Function> fn, v8::Local<v8::Function>& retVal);
    virtual result_t disable() = 0;
    virtual result_t getStore(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t enterWith(v8::Local<v8::Value> store) = 0;
    virtual result_t run(v8::Local<v8::Value> store, v8::Local<v8::Function> callback, OptArgs args, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t exit(v8::Local<v8::Function> callback, OptArgs args, v8::Local<v8::Value>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<AsyncLocalStorage_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_snapshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_disable(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getStore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_enterWith(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_run(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exit(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& AsyncLocalStorage_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "snapshot", s_static_snapshot, true, ClassData::ASYNC_SYNC },
        { "bind", s_static_bind, true, ClassData::ASYNC_SYNC },
        { "disable", s_disable, false, ClassData::ASYNC_SYNC },
        { "getStore", s_getStore, false, ClassData::ASYNC_SYNC },
        { "enterWith", s_enterWith, false, ClassData::ASYNC_SYNC },
        { "run", s_run, false, ClassData::ASYNC_SYNC },
        { "exit", s_exit, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false }
    };

    static ClassData s_cd = {
        "AsyncLocalStorage", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void AsyncLocalStorage_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void AsyncLocalStorage_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AsyncLocalStorage_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t AsyncLocalStorage_base::load(v8::Local<v8::Value> v, obj_ptr<AsyncLocalStorage_base>& retVal)
{
    obj_ptr<AsyncLocalStorage_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void AsyncLocalStorage_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(AsyncLocalStorage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void AsyncLocalStorage_base::s_static_snapshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = snapshot(vr);

    METHOD_RETURN();
}

inline void AsyncLocalStorage_base::s_static_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = bind(v0, vr);

    METHOD_RETURN();
}

inline void AsyncLocalStorage_base::s_disable(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(AsyncLocalStorage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->disable();

    METHOD_VOID();
}

inline void AsyncLocalStorage_base::s_getStore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(AsyncLocalStorage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getStore(vr);

    METHOD_RETURN();
}

inline void AsyncLocalStorage_base::s_enterWith(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(AsyncLocalStorage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->enterWith(v0);

    METHOD_VOID();
}

inline void AsyncLocalStorage_base::s_run(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(AsyncLocalStorage_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(v8::Local<v8::Function>, 1);
    ARG_LIST(2);

    hr = pInst->run(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void AsyncLocalStorage_base::s_exit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(AsyncLocalStorage_base);
    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = pInst->exit(v0, v1, vr);

    METHOD_RETURN();
}
}
