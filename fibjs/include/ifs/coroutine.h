/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _coroutine_base_H_
#define _coroutine_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Lock_base;
class Semaphore_base;
class Condition_base;
class Event_base;
class Trigger_base;
class BlockQueue_base;
class Fiber_base;

class coroutine_base : public object_base
{
    DECLARE_CLASS(coroutine_base);

public:
    // coroutine_base
    static result_t start(v8::Local<v8::Function> func, const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<Fiber_base>& retVal);
    static result_t parallel(v8::Local<v8::Array> funcs, int32_t fibers, v8::Local<v8::Array>& retVal);
    static result_t parallel(v8::Local<v8::Array> datas, v8::Local<v8::Function> func, int32_t fibers, v8::Local<v8::Array>& retVal);
    static result_t parallel(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Array>& retVal);
    static result_t current(obj_ptr<Fiber_base>& retVal);
    static result_t sleep(int32_t ms);
    static result_t get_fibers(v8::Local<v8::Array>& retVal);
    static result_t get_spareFibers(int32_t& retVal);
    static result_t set_spareFibers(int32_t newVal);
    static result_t get_vmid(int32_t& retVal);
    static result_t get_loglevel(int32_t& retVal);
    static result_t set_loglevel(int32_t newVal);

public:
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_parallel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_current(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sleep(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_fibers(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_spareFibers(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_spareFibers(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_vmid(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_loglevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
};

}

#include "Lock.h"
#include "Semaphore.h"
#include "Condition.h"
#include "Event.h"
#include "Trigger.h"
#include "BlockQueue.h"
#include "Fiber.h"

namespace fibjs
{
    inline ClassInfo& coroutine_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"start", s_start, true},
            {"parallel", s_parallel, true},
            {"current", s_current, true},
            {"sleep", s_sleep, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Lock", Lock_base::class_info},
            {"Semaphore", Semaphore_base::class_info},
            {"Condition", Condition_base::class_info},
            {"Event", Event_base::class_info},
            {"Trigger", Trigger_base::class_info},
            {"BlockQueue", BlockQueue_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"fibers", s_get_fibers, block_set, true},
            {"spareFibers", s_get_spareFibers, s_set_spareFibers, true},
            {"vmid", s_get_vmid, block_set, true},
            {"loglevel", s_get_loglevel, s_set_loglevel, true}
        };

        static ClassData s_cd = 
        { 
            "coroutine", NULL, 
            4, s_method, 6, s_object, 4, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void coroutine_base::s_get_fibers(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Array> vr;

        PROPERTY_ENTER();

        hr = get_fibers(vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_get_spareFibers(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_spareFibers(vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_set_spareFibers(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_spareFibers(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void coroutine_base::s_get_vmid(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_vmid(vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_get_loglevel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_loglevel(vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_set_loglevel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_loglevel(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void coroutine_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Fiber_base> vr;

        METHOD_ENTER(-1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = start(v0, args, vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_parallel(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Array> vr;

        METHOD_ENTER(2, 1);

        ARG(v8::Local<v8::Array>, 0);
        OPT_ARG(int32_t, 1, -1);

        hr = parallel(v0, v1, vr);

        METHOD_OVER(3, 2);

        ARG(v8::Local<v8::Array>, 0);
        ARG(v8::Local<v8::Function>, 1);
        OPT_ARG(int32_t, 2, -1);

        hr = parallel(v0, v1, v2, vr);

        METHOD_OVER(-1, 0);

        hr = parallel(args, vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_current(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Fiber_base> vr;

        METHOD_ENTER(0, 0);

        hr = current(vr);

        METHOD_RETURN();
    }

    inline void coroutine_base::s_sleep(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(1, 0);

        OPT_ARG(int32_t, 0, 0);

        hr = sleep(v0);

        METHOD_VOID();
    }

}

#endif

