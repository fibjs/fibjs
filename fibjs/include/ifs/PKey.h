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

class PKey_base : public object_base {
    DECLARE_CLASS(PKey_base);

public:
    // PKey_base
    static result_t _new(Buffer_base* DerKey, exlib::string password, obj_ptr<PKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string pemKey, exlib::string password, obj_ptr<PKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> jsonKey, obj_ptr<PKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_keySize(int32_t& retVal) = 0;
    virtual result_t get_alg(exlib::string& retVal) = 0;
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;
    virtual result_t isPrivate(bool& retVal) = 0;
    virtual result_t clone(obj_ptr<PKey_base>& retVal) = 0;
    static result_t from(Buffer_base* DerKey, exlib::string password, obj_ptr<PKey_base>& retVal);
    static result_t from(exlib::string pemKey, exlib::string password, obj_ptr<PKey_base>& retVal);
    static result_t from(v8::Local<v8::Object> jsonKey, obj_ptr<PKey_base>& retVal);
    virtual result_t pem(exlib::string& retVal) = 0;
    virtual result_t der(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t json(v8::Local<v8::Object> opts, v8::Local<v8::Object>& retVal) = 0;
    virtual result_t equals(object_base* key, bool& retVal) = 0;
    virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t sign(Buffer_base* data, v8::Local<v8::Object> opts, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, v8::Local<v8::Object> opts, bool& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_keySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_alg(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_publicKey(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_isPrivate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_from(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_der(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_equals(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(PKey_base, encrypt, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(PKey_base, decrypt, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE3(PKey_base, sign, Buffer_base*, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE4(PKey_base, verify, Buffer_base*, Buffer_base*, v8::Local<v8::Object>, bool);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& PKey_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isPrivate", s_isPrivate, false, false },
        { "clone", s_clone, false, false },
        { "from", s_static_from, true, false },
        { "pem", s_pem, false, false },
        { "der", s_der, false, false },
        { "json", s_json, false, false },
        { "equals", s_equals, false, false },
        { "encrypt", s_encrypt, false, true },
        { "encryptSync", s_encrypt, false, false },
        { "decrypt", s_decrypt, false, true },
        { "decryptSync", s_decrypt, false, false },
        { "sign", s_sign, false, true },
        { "signSync", s_sign, false, false },
        { "verify", s_verify, false, true },
        { "verifySync", s_verify, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "keySize", s_get_keySize, block_set, false },
        { "alg", s_get_alg, block_set, false },
        { "publicKey", s_get_publicKey, block_set, false }
    };

    static ClassData s_cd = {
        "PKey", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void PKey_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void PKey_base::__new(const T& args)
{
    obj_ptr<PKey_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    DEPRECATED_SOON("PKey.PKey");

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    DEPRECATED_SOON("PKey.PKey");

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void PKey_base::s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_get_keySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_keySize(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_get_alg(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_alg(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_get_publicKey(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_publicKey(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_isPrivate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isPrivate(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clone(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_static_from(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = from(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = from(v0, v1, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = from(v0, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_pem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->pem(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_der(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->der(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = pInst->json(v0, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_equals(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<object_base>, 0);

    hr = pInst->equals(v0, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_encrypt(v0, cb, args);
    else
        hr = pInst->ac_encrypt(v0, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_decrypt(v0, cb, args);
    else
        hr = pInst->ac_decrypt(v0, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_sign(v0, v1, cb, args);
    else
        hr = pInst->ac_sign(v0, v1, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_verify(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_verify(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
