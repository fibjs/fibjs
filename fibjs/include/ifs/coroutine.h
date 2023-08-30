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

class Lock_base;
class Semaphore_base;
class Condition_base;
class Event_base;
class Worker_base;
class Fiber_base;

class coroutine_base : public object_base {
    DECLARE_CLASS(coroutine_base);

public:
    // coroutine_base
    static result_t start(v8::Local<v8::Function> func, OptArgs args, obj_ptr<Fiber_base>& retVal);
    static result_t parallel(v8::Local<v8::Array> funcs, int32_t fibers, v8::Local<v8::Array>& retVal);
    static result_t parallel(v8::Local<v8::Array> datas, v8::Local<v8::Function> func, int32_t fibers, v8::Local<v8::Array>& retVal);
    static result_t parallel(v8::Local<v8::Function> func, int32_t num, int32_t fibers, v8::Local<v8::Array>& retVal);
    static result_t parallel(OptArgs funcs, v8::Local<v8::Array>& retVal);
    static result_t current(obj_ptr<Fiber_base>& retVal);
    static result_t sleep(int32_t ms, AsyncEvent* ac);
    static result_t get_fibers(v8::Local<v8::Array>& retVal);
    static result_t get_spareFibers(int32_t& retVal);
    static result_t set_spareFibers(int32_t newVal);
    static result_t get_vmid(int32_t& retVal);
    static result_t get_loglevel(int32_t& retVal);
    static result_t set_loglevel(int32_t newVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parallel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_current(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_sleep(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_fibers(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_spareFibers(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_spareFibers(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_vmid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_loglevel(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_loglevel(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_STATIC1(coroutine_base, sleep, int32_t);
};
}

#include "ifs/Lock.h"
#include "ifs/Semaphore.h"
#include "ifs/Condition.h"
#include "ifs/Event.h"
#include "ifs/Worker.h"
#include "ifs/Fiber.h"

namespace fibjs {
inline ClassInfo& coroutine_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "start", s_static_start, true, false },
        { "parallel", s_static_parallel, true, false },
        { "current", s_static_current, true, false },
        { "sleep", s_static_sleep, true, true },
        { "sleepSync", s_static_sleep, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Lock", Lock_base::class_info },
        { "Semaphore", Semaphore_base::class_info },
        { "Condition", Condition_base::class_info },
        { "Event", Event_base::class_info },
        { "Worker", Worker_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "fibers", s_static_get_fibers, block_set, true },
        { "spareFibers", s_static_get_spareFibers, s_static_set_spareFibers, true },
        { "vmid", s_static_get_vmid, block_set, true },
        { "loglevel", s_static_get_loglevel, s_static_set_loglevel, true }
    };

    static ClassData s_cd = {
        "coroutine", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void coroutine_base::s_static_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Fiber_base> vr;

    METHOD_ENTER();

    METHOD_OVER(-1, 1);

    ARG(v8::Local<v8::Function>, 0);
    ARG_LIST(1);

    hr = start(v0, v1, vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_parallel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Array>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = parallel(v0, v1, vr);

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Array>, 0);
    ARG(v8::Local<v8::Function>, 1);
    OPT_ARG(int32_t, 2, -1);

    hr = parallel(v0, v1, v2, vr);

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Function>, 0);
    ARG(int32_t, 1);
    OPT_ARG(int32_t, 2, -1);

    hr = parallel(v0, v1, v2, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = parallel(v0, vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_current(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Fiber_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = current(vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_sleep(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 0);

    if (!cb.IsEmpty())
        hr = acb_sleep(v0, cb, args);
    else
        hr = ac_sleep(v0);

    METHOD_VOID();
}

inline void coroutine_base::s_static_get_fibers(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    PROPERTY_ENTER();

    hr = get_fibers(vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_get_spareFibers(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_spareFibers(vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_set_spareFibers(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_spareFibers(v0);

    PROPERTY_SET_LEAVE();
}

inline void coroutine_base::s_static_get_vmid(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_vmid(vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_get_loglevel(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_loglevel(vr);

    METHOD_RETURN();
}

inline void coroutine_base::s_static_set_loglevel(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_loglevel(v0);

    PROPERTY_SET_LEAVE();
}
}
