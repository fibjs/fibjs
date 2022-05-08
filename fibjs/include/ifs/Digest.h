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

class Buffer_base;
class PKey_base;

class Digest_base : public object_base {
    DECLARE_CLASS(Digest_base);

public:
    // Digest_base
    virtual result_t update(Buffer_base* data, obj_ptr<Digest_base>& retVal) = 0;
    virtual result_t digest(exlib::string codec, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t sign(PKey_base* key, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t verify(PKey_base* key, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_size(int32_t& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_digest(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_size(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(Digest_base, sign, PKey_base*, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE4(Digest_base, verify, PKey_base*, Buffer_base*, v8::Local<v8::Object>, bool);
};
}

#include "ifs/Buffer.h"
#include "ifs/PKey.h"

namespace fibjs {
inline ClassInfo& Digest_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "update", s_update, false, false },
        { "digest", s_digest, false, false },
        { "sign", s_sign, false, true },
        { "signSync", s_sign, false, false },
        { "verify", s_verify, false, true },
        { "verifySync", s_verify, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "size", s_get_size, block_set, false }
    };

    static ClassData s_cd = {
        "Digest", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Digest_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Digest_base> vr;

    METHOD_NAME("Digest.update");
    METHOD_INSTANCE(Digest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->update(v0, vr);

    METHOD_RETURN();
}

inline void Digest_base::s_digest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("Digest.digest");
    METHOD_INSTANCE(Digest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "buffer");

    hr = pInst->digest(v0, vr);

    METHOD_RETURN();
}

inline void Digest_base::s_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Digest.sign");
    METHOD_INSTANCE(Digest_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<PKey_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_sign(v0, v1, cb, args);
    else
        hr = pInst->ac_sign(v0, v1, vr);

    METHOD_RETURN();
}

inline void Digest_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("Digest.verify");
    METHOD_INSTANCE(Digest_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<PKey_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_verify(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_verify(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Digest_base::s_get_size(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Digest.size");
    METHOD_INSTANCE(Digest_base);
    PROPERTY_ENTER();

    hr = pInst->get_size(vr);

    METHOD_RETURN();
}
}
