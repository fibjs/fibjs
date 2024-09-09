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

class AbortSignal_base : public EventEmitter_base {
    DECLARE_CLASS(AbortSignal_base);

public:
    // AbortSignal_base
    virtual result_t abort(exlib::string reason) = 0;
    virtual result_t get_aborted(bool& retVal) = 0;
    virtual result_t get_onabort(v8::Local<v8::Function>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_abort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_aborted(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onabort(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& AbortSignal_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "abort", s_abort, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "aborted", s_get_aborted, block_set, false },
        { "onabort", s_get_onabort, block_set, false }
    };

    static ClassData s_cd = {
        "AbortSignal", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void AbortSignal_base::s_abort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(AbortSignal_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "AbortError");

    hr = pInst->abort(v0);

    METHOD_VOID();
}

inline void AbortSignal_base::s_get_aborted(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(AbortSignal_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_aborted(vr);

    METHOD_RETURN();
}

inline void AbortSignal_base::s_get_onabort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(AbortSignal_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onabort(vr);

    METHOD_RETURN();
}
}
