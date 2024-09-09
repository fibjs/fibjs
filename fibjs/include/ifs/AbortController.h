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

class AbortSignal_base;

class AbortController_base : public object_base {
    DECLARE_CLASS(AbortController_base);

public:
    // AbortController_base
    static result_t _new(obj_ptr<AbortController_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_signal(obj_ptr<AbortSignal_base>& retVal) = 0;
    virtual result_t abort(exlib::string reason) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_signal(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_abort(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/AbortSignal.h"

namespace fibjs {
inline ClassInfo& AbortController_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "abort", s_abort, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "signal", s_get_signal, block_set, false }
    };

    static ClassData s_cd = {
        "AbortController", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void AbortController_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void AbortController_base::__new(const T& args)
{
    obj_ptr<AbortController_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void AbortController_base::s_get_signal(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AbortSignal_base> vr;

    METHOD_INSTANCE(AbortController_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_signal(vr);

    METHOD_RETURN();
}

inline void AbortController_base::s_abort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(AbortController_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "AbortError");

    hr = pInst->abort(v0);

    METHOD_VOID();
}
}
