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
    virtual result_t bbsSign(Buffer_base* header, v8::Local<v8::Array> messages, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t bbsVerify(Buffer_base* header, v8::Local<v8::Array> messages, Buffer_base* sig, bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t proofGen(Buffer_base* sig, Buffer_base* header, Buffer_base* proofHeader, v8::Local<v8::Array> messages, v8::Local<v8::Array> idx, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t proofVerify(Buffer_base* header, Buffer_base* proofHeader, v8::Local<v8::Array> messages, v8::Local<v8::Array> idx, Buffer_base* proof, bool& retVal, AsyncEvent* ac) = 0;
    static result_t aggregateSignature(v8::Local<v8::Array> sigs, obj_ptr<Buffer_base>& retVal);
    static result_t aggregatePublicKey(v8::Local<v8::Array> keys, obj_ptr<BlsKey_base>& retVal);

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bbsSign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bbsVerify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_proofGen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_proofVerify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_aggregateSignature(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_aggregatePublicKey(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(BlsKey_base, bbsSign, Buffer_base*, v8::Local<v8::Array>, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE4(BlsKey_base, bbsVerify, Buffer_base*, v8::Local<v8::Array>, Buffer_base*, bool);
    ASYNC_MEMBERVALUE6(BlsKey_base, proofGen, Buffer_base*, Buffer_base*, Buffer_base*, v8::Local<v8::Array>, v8::Local<v8::Array>, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE6(BlsKey_base, proofVerify, Buffer_base*, Buffer_base*, v8::Local<v8::Array>, v8::Local<v8::Array>, Buffer_base*, bool);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& BlsKey_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "bbsSign", s_bbsSign, false, true },
        { "bbsSignSync", s_bbsSign, false, false },
        { "bbsVerify", s_bbsVerify, false, true },
        { "bbsVerifySync", s_bbsVerify, false, false },
        { "proofGen", s_proofGen, false, true },
        { "proofGenSync", s_proofGen, false, false },
        { "proofVerify", s_proofVerify, false, true },
        { "proofVerifySync", s_proofVerify, false, false },
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

inline void BlsKey_base::s_bbsSign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(BlsKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Array>, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_bbsSign(v0, v1, cb, args);
    else
        hr = pInst->ac_bbsSign(v0, v1, vr);

    METHOD_RETURN();
}

inline void BlsKey_base::s_bbsVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(BlsKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Array>, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    if (!cb.IsEmpty())
        hr = pInst->acb_bbsVerify(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_bbsVerify(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void BlsKey_base::s_proofGen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(BlsKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    ARG(v8::Local<v8::Array>, 3);
    ARG(v8::Local<v8::Array>, 4);

    if (!cb.IsEmpty())
        hr = pInst->acb_proofGen(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_proofGen(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void BlsKey_base::s_proofVerify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(BlsKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(v8::Local<v8::Array>, 2);
    ARG(v8::Local<v8::Array>, 3);
    ARG(obj_ptr<Buffer_base>, 4);

    if (!cb.IsEmpty())
        hr = pInst->acb_proofVerify(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_proofVerify(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
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
