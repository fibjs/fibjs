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
#include "ifs/PKey.h"

namespace fibjs {

class PKey_base;
class Buffer_base;

class ECCKey_base : public PKey_base {
    DECLARE_CLASS(ECCKey_base);

public:
    // ECCKey_base
    static result_t _new(Buffer_base* DerKey, exlib::string password, obj_ptr<ECCKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string pemKey, exlib::string password, obj_ptr<ECCKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> jsonKey, obj_ptr<ECCKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_curve(exlib::string& retVal) = 0;
    static result_t recover(Buffer_base* data, Buffer_base* sig, obj_ptr<ECCKey_base>& retVal, AsyncEvent* ac);
    static result_t aggregateSignatures(v8::Local<v8::Array> sigs, obj_ptr<Buffer_base>& retVal);
    virtual result_t toX25519(obj_ptr<ECCKey_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t computeSecret(ECCKey_base* publicKey, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_curve(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_recover(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_aggregateSignatures(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_toX25519(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_computeSecret(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(ECCKey_base, recover, Buffer_base*, Buffer_base*, obj_ptr<ECCKey_base>);
    ASYNC_MEMBERVALUE1(ECCKey_base, toX25519, obj_ptr<ECCKey_base>);
    ASYNC_MEMBERVALUE2(ECCKey_base, computeSecret, ECCKey_base*, obj_ptr<Buffer_base>);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& ECCKey_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "recover", s_static_recover, true, true },
        { "recoverSync", s_static_recover, true, false },
        { "aggregateSignatures", s_static_aggregateSignatures, true, false },
        { "toX25519", s_toX25519, false, true },
        { "toX25519Sync", s_toX25519, false, false },
        { "computeSecret", s_computeSecret, false, true },
        { "computeSecretSync", s_computeSecret, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "curve", s_get_curve, block_set, false }
    };

    static ClassData s_cd = {
        "ECCKey", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &PKey_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ECCKey_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void ECCKey_base::__new(const T& args)
{
    obj_ptr<ECCKey_base> vr;

    METHOD_NAME("new ECCKey()");
    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void ECCKey_base::s_get_curve(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("ECCKey.curve");
    METHOD_INSTANCE(ECCKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_curve(vr);

    METHOD_RETURN();
}

inline void ECCKey_base::s_static_recover(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ECCKey_base> vr;

    METHOD_NAME("ECCKey.recover");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_recover(v0, v1, cb, args);
    else
        hr = ac_recover(v0, v1, vr);

    METHOD_RETURN();
}

inline void ECCKey_base::s_static_aggregateSignatures(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("ECCKey.aggregateSignatures");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = aggregateSignatures(v0, vr);

    METHOD_RETURN();
}

inline void ECCKey_base::s_toX25519(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ECCKey_base> vr;

    METHOD_NAME("ECCKey.toX25519");
    METHOD_INSTANCE(ECCKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_toX25519(cb, args);
    else
        hr = pInst->ac_toX25519(vr);

    METHOD_RETURN();
}

inline void ECCKey_base::s_computeSecret(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("ECCKey.computeSecret");
    METHOD_INSTANCE(ECCKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<ECCKey_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_computeSecret(v0, cb, args);
    else
        hr = pInst->ac_computeSecret(v0, vr);

    METHOD_RETURN();
}
}
