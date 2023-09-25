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

class FTModel_base;

class fasttext_base : public object_base {
    DECLARE_CLASS(fasttext_base);

public:
    // fasttext_base
    static result_t loadModel(exlib::string path, obj_ptr<FTModel_base>& retVal, AsyncEvent* ac);
    static result_t train(exlib::string trainFile, v8::Local<v8::Object> args, obj_ptr<FTModel_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_loadModel(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_train(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(fasttext_base, loadModel, exlib::string, obj_ptr<FTModel_base>);
    ASYNC_STATICVALUE3(fasttext_base, train, exlib::string, v8::Local<v8::Object>, obj_ptr<FTModel_base>);
};
}

#include "ifs/FTModel.h"

namespace fibjs {
inline ClassInfo& fasttext_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "loadModel", s_static_loadModel, true, true },
        { "loadModelSync", s_static_loadModel, true, false },
        { "train", s_static_train, true, true },
        { "trainSync", s_static_train, true, false }
    };

    static ClassData s_cd = {
        "fasttext", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void fasttext_base::s_static_loadModel(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FTModel_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_loadModel(v0, cb, args);
    else
        hr = ac_loadModel(v0, vr);

    METHOD_RETURN();
}

inline void fasttext_base::s_static_train(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<FTModel_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_train(v0, v1, cb, args);
    else
        hr = ac_train(v0, v1, vr);

    METHOD_RETURN();
}
}
