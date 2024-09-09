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

class Fiber_base : public object_base {
    DECLARE_CLASS(Fiber_base);

public:
    // Fiber_base
    virtual result_t join() = 0;
    virtual result_t get_id(int64_t& retVal) = 0;
    virtual result_t get_caller(obj_ptr<Fiber_base>& retVal) = 0;
    virtual result_t get_stack(exlib::string& retVal) = 0;
    virtual result_t get_stack_usage(int32_t& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_join(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_caller(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stack_usage(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Fiber_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "join", s_join, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "id", s_get_id, block_set, false },
        { "caller", s_get_caller, block_set, false },
        { "stack", s_get_stack, block_set, false },
        { "stack_usage", s_get_stack_usage, block_set, false }
    };

    static ClassData s_cd = {
        "Fiber", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Fiber_base::s_join(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Fiber_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->join();

    METHOD_VOID();
}

inline void Fiber_base::s_get_id(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Fiber_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_id(vr);

    METHOD_RETURN();
}

inline void Fiber_base::s_get_caller(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Fiber_base> vr;

    METHOD_INSTANCE(Fiber_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_caller(vr);

    METHOD_RETURN();
}

inline void Fiber_base::s_get_stack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Fiber_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stack(vr);

    METHOD_RETURN();
}

inline void Fiber_base::s_get_stack_usage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Fiber_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stack_usage(vr);

    METHOD_RETURN();
}
}
