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

class Timer_base;

class timers_base : public object_base {
    DECLARE_CLASS(timers_base);

public:
    // timers_base
    static result_t setTimeout(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearTimeout(v8::Local<v8::Value> t);
    static result_t setInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearInterval(v8::Local<v8::Value> t);
    static result_t setHrInterval(v8::Local<v8::Function> callback, double timeout, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearHrInterval(v8::Local<v8::Value> t);
    static result_t setImmediate(v8::Local<v8::Function> callback, OptArgs args, obj_ptr<Timer_base>& retVal);
    static result_t clearImmediate(v8::Local<v8::Value> t);
    static result_t call(v8::Local<v8::Function> func, double timeout, OptArgs args, v8::Local<v8::Value>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_call(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Timer.h"

namespace fibjs {
inline ClassInfo& timers_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setTimeout", s_static_setTimeout, true, false },
        { "clearTimeout", s_static_clearTimeout, true, false },
        { "setInterval", s_static_setInterval, true, false },
        { "clearInterval", s_static_clearInterval, true, false },
        { "setHrInterval", s_static_setHrInterval, true, false },
        { "clearHrInterval", s_static_clearHrInterval, true, false },
        { "setImmediate", s_static_setImmediate, true, false },
        { "clearImmediate", s_static_clearImmediate, true, false },
        { "call", s_static_call, true, false }
    };

    static ClassData s_cd = {
        "timers", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void timers_base::s_static_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    OPT_ARG(double, 1, 1);
    ARG_LIST(2);

    hr = setTimeout(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void timers_base::s_static_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearTimeout(v0);

    METHOD_VOID();
}

inline void timers_base::s_static_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = setInterval(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void timers_base::s_static_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearInterval(v0);

    METHOD_VOID();
}

inline void timers_base::s_static_setHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = setHrInterval(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void timers_base::s_static_clearHrInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearHrInterval(v0);

    METHOD_VOID();
}

inline void timers_base::s_static_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = setImmediate(v0, v1, vr);

    METHOD_RETURN();
}

inline void timers_base::s_static_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = clearImmediate(v0);

    METHOD_VOID();
}

inline void timers_base::s_static_call(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(double, 1);
    ARG_LIST(2);

    hr = call(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
