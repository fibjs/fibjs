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

class PerformanceObserver_base : public object_base {
    DECLARE_CLASS(PerformanceObserver_base);

public:
    // PerformanceObserver_base
    static result_t _new(v8::Local<v8::Function> callback, obj_ptr<PerformanceObserver_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t observe(v8::Local<v8::Object> options) = 0;
    virtual result_t disconnect() = 0;
    virtual result_t takeRecords(obj_ptr<NArray>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<PerformanceObserver_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_observe(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_takeRecords(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& PerformanceObserver_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "observe", s_observe, false, ClassData::ASYNC_SYNC },
        { "disconnect", s_disconnect, false, ClassData::ASYNC_SYNC },
        { "takeRecords", s_takeRecords, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "PerformanceObserver", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PerformanceObserver_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void PerformanceObserver_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PerformanceObserver_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t PerformanceObserver_base::load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<PerformanceObserver_base>& retVal)
{
    obj_ptr<PerformanceObserver_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void PerformanceObserver_base::s_observe(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(PerformanceObserver_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->observe(v0);

    METHOD_VOID();
}

inline void PerformanceObserver_base::s_disconnect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(PerformanceObserver_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->disconnect();

    METHOD_VOID();
}

inline void PerformanceObserver_base::s_takeRecords(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(PerformanceObserver_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->takeRecords(vr);

    METHOD_RETURN();
}
}
