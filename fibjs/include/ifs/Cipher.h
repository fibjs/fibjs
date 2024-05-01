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

class Cipher_base : public object_base {
    DECLARE_CLASS(Cipher_base);

public:
    // Cipher_base
    virtual result_t setAuthTag(Buffer_base* buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t setAuthTag(exlib::string buffer, exlib::string encoding, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t getAuthTag(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t setAAD(Buffer_base* buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t setAAD(exlib::string buffer, v8::Local<v8::Object> options, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t setAutoPadding(bool autoPadding, obj_ptr<Cipher_base>& retVal) = 0;
    virtual result_t update(Buffer_base* data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t update(exlib::string data, exlib::string inputEncoding, exlib::string outputEncoding, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t final(exlib::string outputEncoding, v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_setAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setAAD(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setAutoPadding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_final(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& Cipher_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setAuthTag", s_setAuthTag, false, ClassData::ASYNC_SYNC },
        { "getAuthTag", s_getAuthTag, false, ClassData::ASYNC_SYNC },
        { "setAAD", s_setAAD, false, ClassData::ASYNC_SYNC },
        { "setAutoPadding", s_setAutoPadding, false, ClassData::ASYNC_SYNC },
        { "update", s_update, false, ClassData::ASYNC_SYNC },
        { "final", s_final, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "Cipher", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Cipher_base::s_setAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "buffer");

    hr = pInst->setAuthTag(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = pInst->setAuthTag(v0, v1, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_getAuthTag(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getAuthTag(vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_setAAD(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->setAAD(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->setAAD(v0, v1, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_setAutoPadding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Cipher_base> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(bool, 0, true);

    hr = pInst->setAutoPadding(v0, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "buffer");
    OPT_ARG(exlib::string, 2, "buffer");

    hr = pInst->update(v0, v1, v2, vr);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");
    OPT_ARG(exlib::string, 2, "buffer");

    hr = pInst->update(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Cipher_base::s_final(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Cipher_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "buffer");

    hr = pInst->final(v0, vr);

    METHOD_RETURN();
}
}
