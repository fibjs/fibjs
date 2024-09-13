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

class Timer_base : public object_base {
    DECLARE_CLASS(Timer_base);

public:
    // Timer_base
    virtual result_t ref(obj_ptr<Timer_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<Timer_base>& retVal) = 0;
    virtual result_t clear() = 0;
    virtual result_t get_stopped(bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stopped(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Timer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC },
        { "clear", s_clear, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "stopped", s_get_stopped, block_set, false }
    };

    static ClassData s_cd = {
        "Timer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Timer_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_INSTANCE(Timer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void Timer_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Timer_base> vr;

    METHOD_INSTANCE(Timer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}

inline void Timer_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Timer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clear();

    METHOD_VOID();
}

inline void Timer_base::s_get_stopped(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Timer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stopped(vr);

    METHOD_RETURN();
}
}
