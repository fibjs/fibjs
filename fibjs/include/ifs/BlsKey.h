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
#include "ifs/ECKey.h"

namespace fibjs {

class ECKey_base;
class Buffer_base;

class BlsKey_base : public ECKey_base {
    DECLARE_CLASS(BlsKey_base);

public:
    // BlsKey_base
    static result_t _new(v8::Local<v8::Object> jsonKey, obj_ptr<BlsKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t aggregateSignature(v8::Local<v8::Array> sigs, obj_ptr<Buffer_base>& retVal);
    static result_t aggregatePublicKey(v8::Local<v8::Array> sigs, obj_ptr<BlsKey_base>& retVal);

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_aggregateSignature(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_aggregatePublicKey(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& BlsKey_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "aggregateSignature", s_static_aggregateSignature, true, false },
        { "aggregatePublicKey", s_static_aggregatePublicKey, true, false }
    };

    static ClassData s_cd = {
        "BlsKey", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &ECKey_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void BlsKey_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void BlsKey_base::__new(const T& args)
{
    obj_ptr<BlsKey_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void BlsKey_base::s_static_aggregateSignature(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = aggregateSignature(v0, vr);

    METHOD_RETURN();
}

inline void BlsKey_base::s_static_aggregatePublicKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<BlsKey_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = aggregatePublicKey(v0, vr);

    METHOD_RETURN();
}
}
