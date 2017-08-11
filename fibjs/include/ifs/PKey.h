/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _PKey_base_H_
#define _PKey_base_H_

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
    static result_t _new(obj_ptr<PKey_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t get_keySize(int32_t& retVal) = 0;
    virtual result_t get_publicKey(obj_ptr<PKey_base>& retVal) = 0;
    virtual result_t genRsaKey(int32_t size, AsyncEvent* ac) = 0;
    virtual result_t genEcKey(exlib::string curve, AsyncEvent* ac) = 0;
    virtual result_t isPrivate(bool& retVal) = 0;
    virtual result_t clone(obj_ptr<PKey_base>& retVal) = 0;
    virtual result_t importKey(Buffer_base* DerKey, exlib::string password) = 0;
    virtual result_t importKey(exlib::string pemKey, exlib::string password) = 0;
    virtual result_t importKey(v8::Local<v8::Object> jsonKey) = 0;
    virtual result_t importFile(exlib::string filename, exlib::string password) = 0;
    virtual result_t exportPem(exlib::string& retVal) = 0;
    virtual result_t exportDer(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t exportJson(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t encrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t decrypt(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t sign(Buffer_base* data, int32_t alg, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t verify(Buffer_base* data, Buffer_base* sign, int32_t alg, bool& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isPrivate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_importKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_importFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exportJson(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sign(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(PKey_base, genRsaKey, int32_t);
    ASYNC_MEMBER1(PKey_base, genEcKey, exlib::string);
    ASYNC_MEMBERVALUE2(PKey_base, encrypt, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(PKey_base, decrypt, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE3(PKey_base, sign, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE4(PKey_base, verify, Buffer_base*, Buffer_base*, int32_t, bool);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& PKey_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "genRsaKey", s_genRsaKey, false },
        { "genRsaKeySync", s_genRsaKey, false },
        { "genEcKey", s_genEcKey, false },
        { "genEcKeySync", s_genEcKey, false },
        { "isPrivate", s_isPrivate, false },
        { "clone", s_clone, false },
        { "importKey", s_importKey, false },
        { "importFile", s_importFile, false },
        { "exportPem", s_exportPem, false },
        { "exportDer", s_exportDer, false },
        { "exportJson", s_exportJson, false },
        { "encrypt", s_encrypt, false },
        { "encryptSync", s_encrypt, false },
        { "decrypt", s_decrypt, false },
        { "decryptSync", s_decrypt, false },
        { "sign", s_sign, false },
        { "signSync", s_sign, false },
        { "verify", s_verify, false },
        { "verifySync", s_verify, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, block_set, false },
        { "keySize", s_get_keySize, block_set, false },
        { "publicKey", s_get_publicKey, block_set, false }
    };

    static ClassData s_cd = {
        "PKey", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
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

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void PKey_base::s_get_name(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_get_keySize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_keySize(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_get_publicKey(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<PKey_base> vr;

    METHOD_INSTANCE(PKey_base);
    PROPERTY_ENTER();

    hr = pInst->get_publicKey(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_genRsaKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_genRsaKey(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_genRsaKey(v0);

    METHOD_VOID();
}

inline void PKey_base::s_genEcKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "secp521r1");

    if (!cb.IsEmpty()) {
        pInst->acb_genEcKey(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_genEcKey(v0);

    METHOD_VOID();
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

inline void PKey_base::s_importKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->importKey(v0, v1);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->importKey(v0, v1);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->importKey(v0);

    METHOD_VOID();
}

inline void PKey_base::s_importFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->importFile(v0, v1);

    METHOD_VOID();
}

inline void PKey_base::s_exportPem(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->exportPem(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_exportDer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->exportDer(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_exportJson(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->exportJson(vr);

    METHOD_RETURN();
}

inline void PKey_base::s_encrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(PKey_base);
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

inline void PKey_base::s_decrypt(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(PKey_base);
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

inline void PKey_base::s_sign(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_sign(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_sign(v0, v1, vr);

    METHOD_RETURN();
}

inline void PKey_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(PKey_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    OPT_ARG(int32_t, 2, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_verify(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_verify(v0, v1, v2, vr);

    METHOD_RETURN();
}
}

#endif
