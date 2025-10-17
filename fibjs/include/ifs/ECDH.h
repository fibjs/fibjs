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

class ECDH_base : public object_base {
    DECLARE_CLASS(ECDH_base);

public:
    // ECDH_base
    static result_t convertKey(v8::Local<v8::Value> key, exlib::string curve, exlib::string inputEncoding, exlib::string outputEncoding, exlib::string format, v8::Local<v8::Value>& retVal);
    virtual result_t computeSecret(v8::Local<v8::Value> otherPublicKey, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t generateKeys(exlib::string outputEncoding, exlib::string format, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_curveName(exlib::string& retVal) = 0;
    virtual result_t getPrivateKey(exlib::string encoding, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t getPublicKey(exlib::string encoding, exlib::string format, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t setPrivateKey(v8::Local<v8::Value> privateKey, exlib::string encoding) = 0;
    virtual result_t setPublicKey(v8::Local<v8::Value> publicKey, exlib::string encoding) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<ECDH_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_convertKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_computeSecret(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_generateKeys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_curveName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& ECDH_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "convertKey", s_static_convertKey, true, ClassData::ASYNC_SYNC },
        { "computeSecret", s_computeSecret, false, ClassData::ASYNC_SYNC },
        { "generateKeys", s_generateKeys, false, ClassData::ASYNC_SYNC },
        { "getPrivateKey", s_getPrivateKey, false, ClassData::ASYNC_SYNC },
        { "getPublicKey", s_getPublicKey, false, ClassData::ASYNC_SYNC },
        { "setPrivateKey", s_setPrivateKey, false, ClassData::ASYNC_SYNC },
        { "setPublicKey", s_setPublicKey, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "curveName", s_get_curveName, block_set, false }
    };

    static ClassData s_cd = {
        "ECDH", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ECDH_base::s_static_convertKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_ENTER();

    METHOD_OVER(5, 2);

    ARG(v8::Local<v8::Value>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "hex");
    OPT_ARG(exlib::string, 3, "hex");
    OPT_ARG(exlib::string, 4, "uncompressed");

    hr = convertKey(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void ECDH_base::s_computeSecret(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "hex");
    OPT_ARG(exlib::string, 2, "buffer");

    hr = pInst->computeSecret(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void ECDH_base::s_generateKeys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(exlib::string, 0, "buffer");
    OPT_ARG(exlib::string, 1, "uncompressed");

    hr = pInst->generateKeys(v0, v1, vr);

    METHOD_RETURN();
}

inline void ECDH_base::s_get_curveName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_curveName(vr);

    METHOD_RETURN();
}

inline void ECDH_base::s_getPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "buffer");

    hr = pInst->getPrivateKey(v0, vr);

    METHOD_RETURN();
}

inline void ECDH_base::s_getPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(exlib::string, 0, "buffer");
    OPT_ARG(exlib::string, 1, "uncompressed");

    hr = pInst->getPublicKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void ECDH_base::s_setPrivateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "hex");

    hr = pInst->setPrivateKey(v0, v1);

    METHOD_VOID();
}

inline void ECDH_base::s_setPublicKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ECDH_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(exlib::string, 1, "hex");

    hr = pInst->setPublicKey(v0, v1);

    METHOD_VOID();
}
}
