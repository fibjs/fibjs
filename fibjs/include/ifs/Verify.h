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
class KeyObject_base;

class Verify_base : public object_base {
    DECLARE_CLASS(Verify_base);

public:
    // Verify_base
    virtual result_t update(Buffer_base* data, obj_ptr<Verify_base>& retVal) = 0;
    virtual result_t update(exlib::string data, exlib::string codec, obj_ptr<Verify_base>& retVal) = 0;
    virtual result_t verify(Buffer_base* privateKey, Buffer_base* signature, bool& retVal) = 0;
    virtual result_t verify(KeyObject_base* privateKey, Buffer_base* signature, bool& retVal) = 0;
    virtual result_t verify(v8::Local<v8::Object> key, Buffer_base* signature, bool& retVal) = 0;
    virtual result_t verify(Buffer_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal) = 0;
    virtual result_t verify(KeyObject_base* privateKey, exlib::string signature, exlib::string encoding, bool& retVal) = 0;
    virtual result_t verify(v8::Local<v8::Object> key, exlib::string signature, exlib::string encoding, bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_update(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_verify(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"
#include "ifs/KeyObject.h"

namespace fibjs {
inline ClassInfo& Verify_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "update", s_update, false, ClassData::ASYNC_SYNC },
        { "verify", s_verify, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "Verify", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Verify_base::s_update(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Verify_base> vr;

    METHOD_INSTANCE(Verify_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->update(v0, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    hr = pInst->update(v0, v1, vr);

    METHOD_RETURN();
}

inline void Verify_base::s_verify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Verify_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = pInst->verify(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<KeyObject_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = pInst->verify(v0, v1, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = pInst->verify(v0, v1, vr);

    METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "buffer");

    hr = pInst->verify(v0, v1, v2, vr);

    METHOD_OVER(3, 2);

    ARG(obj_ptr<KeyObject_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "buffer");

    hr = pInst->verify(v0, v1, v2, vr);

    METHOD_OVER(3, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "buffer");

    hr = pInst->verify(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
