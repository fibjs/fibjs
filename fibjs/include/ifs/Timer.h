/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Timer_base_H_
#define _Timer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Timer_base : public object_base {
    DECLARE_CLASS(Timer_base);

public:
    // Timer_base
    virtual result_t clear() = 0;
    virtual result_t get_stopped(bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stopped(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& Timer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "clear", s_clear, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "stopped", s_get_stopped, block_set, false }
    };

    static ClassData s_cd = {
        "Timer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Timer_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Timer.clear");
    METHOD_INSTANCE(Timer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clear();

    METHOD_VOID();
}

inline void Timer_base::s_get_stopped(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Timer.stopped");
    METHOD_INSTANCE(Timer_base);
    PROPERTY_ENTER();

    hr = pInst->get_stopped(vr);

    METHOD_RETURN();
}
}

#endif
