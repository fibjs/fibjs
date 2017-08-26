/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _timers_base_H_
#define _timers_base_H_

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
    static result_t clearInterval(Timer_base* t);
    static result_t clearTimeout(Timer_base* t);
    static result_t clearImmediate(Timer_base* t);
    static result_t setInterval(v8::Local<v8::Function> callback, int32_t timeout, obj_ptr<Timer_base>& retVal);
    static result_t setTimeout(v8::Local<v8::Function> callback, int32_t timeout, obj_ptr<Timer_base>& retVal);
    static result_t setImmediate(v8::Local<v8::Function> callback, obj_ptr<Timer_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "Timer.h"

namespace fibjs {
inline ClassInfo& timers_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "clearInterval", s_clearInterval, true },
        { "clearTimeout", s_clearTimeout, true },
        { "clearImmediate", s_clearImmediate, true },
        { "setInterval", s_setInterval, true },
        { "setTimeout", s_setTimeout, true },
        { "setImmediate", s_setImmediate, true }
    };

    static ClassData s_cd = {
        "timers", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void timers_base::s_clearInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Timer_base>, 0);

    hr = clearInterval(v0);

    METHOD_VOID();
}

inline void timers_base::s_clearTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Timer_base>, 0);

    hr = clearTimeout(v0);

    METHOD_VOID();
}

inline void timers_base::s_clearImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Timer_base>, 0);

    hr = clearImmediate(v0);

    METHOD_VOID();
}

inline void timers_base::s_setInterval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(int32_t, 1);

    hr = setInterval(v0, v1, vr);

    METHOD_RETURN();
}

inline void timers_base::s_setTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(int32_t, 1);

    hr = setTimeout(v0, v1, vr);

    METHOD_RETURN();
}

inline void timers_base::s_setImmediate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = setImmediate(v0, vr);

    METHOD_RETURN();
}
}

#endif
