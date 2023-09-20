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

class FTModel_base : public object_base {
    DECLARE_CLASS(FTModel_base);

public:
    // FTModel_base
    virtual result_t predict(exlib::string text, int32_t k, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t predict(v8::Local<v8::Array> texts, int32_t k, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_predict(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(FTModel_base, predict, exlib::string, int32_t, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE3(FTModel_base, predict, v8::Local<v8::Array>, int32_t, obj_ptr<NArray>);
};
}

namespace fibjs {
inline ClassInfo& FTModel_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "predict", s_predict, false, true },
        { "predictSync", s_predict, false, false }
    };

    static ClassData s_cd = {
        "FTModel", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FTModel_base::s_predict(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(FTModel_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 3);

    if (!cb.IsEmpty())
        hr = pInst->acb_predict(v0, v1, cb, args);
    else
        hr = pInst->ac_predict(v0, v1, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Array>, 0);
    OPT_ARG(int32_t, 1, 3);

    if (!cb.IsEmpty())
        hr = pInst->acb_predict(v0, v1, cb, args);
    else
        hr = pInst->ac_predict(v0, v1, vr);

    METHOD_RETURN();
}
}
