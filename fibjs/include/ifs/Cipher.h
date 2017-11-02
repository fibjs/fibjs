/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Cipher_base_H_
#define _Cipher_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Buffer_base;

class Cipher_base : public object_base {
    DECLARE_CLASS(Cipher_base);

public:
    // Cipher_base
    static result_t _new(int32_t provider, Buffer_base* key, obj_ptr<Cipher_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(int32_t provider, int32_t mode, Buffer_base* key, obj_ptr<Cipher_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(int32_t provider, int32_t mode, Buffer_base* key, Buffer_base* iv, obj_ptr<Cipher_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_keySize(int32_t& retVal) = 0;
    virtual result_t get_ivSize(int32_t& retVal) = 0;
    virtual result_t get_blockSize(int32_t& retVal) = 0;
    virtual result_t paddingMode(int32_t mode) = 0;
    virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_ivSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_blockSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_paddingMode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(Cipher_base, encrypt, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(Cipher_base, decrypt, Buffer_base*, obj_ptr<Buffer_base>);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& Cipher_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "paddingMode", s_paddingMode, false },
        { "encrypt", s_encrypt, false },
        { "encryptSync", s_encrypt, false },
        { "decrypt", s_decrypt, false },
        { "decryptSync", s_decrypt, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "keySize", s_get_keySize, block_set, false },
        { "ivSize", s_get_ivSize, block_set, false },
        { "blockSize", s_get_blockSize, block_set, false }
    };

    static ClassData s_cd = {
        "Cipher", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Cipher_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Cipher_base::__new(const T& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_NAME("new Cipher()");
    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(3, 3);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Buffer_base>, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(4, 4);

    ARG(int32_t, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Buffer_base>, 2);
    ARG(obj_ptr<Buffer_base>, 3);

    hr = _new(v0, v1, v2, v3, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Cipher_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("Cipher.name");
    METHOD_INSTANCE(Cipher_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Cipher.keySize");
    METHOD_INSTANCE(Cipher_base);
    PROPERTY_ENTER();

    hr = pInst->get_keySize(vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_get_ivSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Cipher.ivSize");
    METHOD_INSTANCE(Cipher_base);
    PROPERTY_ENTER();

    hr = pInst->get_ivSize(vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_get_blockSize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("Cipher.blockSize");
    METHOD_INSTANCE(Cipher_base);
    PROPERTY_ENTER();

    hr = pInst->get_blockSize(vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_paddingMode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("Cipher.paddingMode");
    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->paddingMode(v0);

    METHOD_VOID();
}

inline void Cipher_base::s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Cipher.encrypt");
    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_encrypt(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_encrypt(v0, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("Cipher.decrypt");
    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_decrypt(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_decrypt(v0, vr);

    METHOD_RETURN();
}
}

#endif
