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
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;

class StatsWatcher_base : public EventEmitter_base {
    DECLARE_CLASS(StatsWatcher_base);

public:
    // StatsWatcher_base
    virtual result_t get_onchange(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onchange(v8::Local<v8::Function> newVal) = 0;
    virtual result_t close() = 0;
    virtual result_t ref(obj_ptr<StatsWatcher_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<StatsWatcher_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_onchange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onchange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& StatsWatcher_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onchange", s_get_onchange, s_set_onchange, false }
    };

    static ClassData s_cd = {
        "StatsWatcher", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void StatsWatcher_base::s_get_onchange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(StatsWatcher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onchange(vr);

    METHOD_RETURN();
}

inline void StatsWatcher_base::s_set_onchange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StatsWatcher_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onchange(v0);

    METHOD_VOID();
}

inline void StatsWatcher_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(StatsWatcher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}

inline void StatsWatcher_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<StatsWatcher_base> vr;

    METHOD_INSTANCE(StatsWatcher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void StatsWatcher_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<StatsWatcher_base> vr;

    METHOD_INSTANCE(StatsWatcher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}
}
