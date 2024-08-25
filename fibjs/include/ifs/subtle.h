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
class CryptoKey_base;

class subtle_base : public object_base {
    DECLARE_CLASS(subtle_base);

public:
    // subtle_base
    static result_t digest(exlib::string algorithm, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t digest(v8::Local<v8::Object> algorithm, Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t exportKey(exlib::string format, CryptoKey_base* key, Variant& retVal, AsyncEvent* ac);
    static result_t generateKey(v8::Local<v8::Object> algorithm, bool extractable, v8::Local<v8::Array> usages, Variant& retVal, AsyncEvent* ac);
    static result_t importKey(exlib::string format, v8::Local<v8::Value> keyData, v8::Local<v8::Object> algorithm, bool extractable, v8::Local<v8::Array> usages, obj_ptr<CryptoKey_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_digest(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_exportKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_generateKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_importKey(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(subtle_base, digest, exlib::string, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE3(subtle_base, digest, v8::Local<v8::Object>, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATICVALUE3(subtle_base, exportKey, exlib::string, CryptoKey_base*, Variant);
    ASYNC_STATICVALUE4(subtle_base, generateKey, v8::Local<v8::Object>, bool, v8::Local<v8::Array>, Variant);
    ASYNC_STATICVALUE6(subtle_base, importKey, exlib::string, v8::Local<v8::Value>, v8::Local<v8::Object>, bool, v8::Local<v8::Array>, obj_ptr<CryptoKey_base>);
};
}

#include "ifs/Buffer.h"
#include "ifs/CryptoKey.h"

namespace fibjs {
inline ClassInfo& subtle_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "digest", s_static_digest, true, ClassData::ASYNC_PROMISE },
        { "exportKey", s_static_exportKey, true, ClassData::ASYNC_PROMISE },
        { "generateKey", s_static_generateKey, true, ClassData::ASYNC_PROMISE },
        { "importKey", s_static_importKey, true, ClassData::ASYNC_PROMISE }
    };

    static ClassData s_cd = {
        "subtle", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void subtle_base::s_static_digest(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_digest(v0, v1, cb, args);
    else
        hr = ac_digest(v0, v1, vr);

    ASYNC_METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_digest(v0, v1, cb, args);
    else
        hr = ac_digest(v0, v1, vr);

    METHOD_RETURN();
}

inline void subtle_base::s_static_exportKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<CryptoKey_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_exportKey(v0, v1, cb, args);
    else
        hr = ac_exportKey(v0, v1, vr);

    METHOD_RETURN();
}

inline void subtle_base::s_static_generateKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(v8::Local<v8::Object>, 0);
    ARG(bool, 1);
    ARG(v8::Local<v8::Array>, 2);

    if (!cb.IsEmpty())
        hr = acb_generateKey(v0, v1, v2, cb, args);
    else
        hr = ac_generateKey(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void subtle_base::s_static_importKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<CryptoKey_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(5, 5);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Value>, 1);
    ARG(v8::Local<v8::Object>, 2);
    ARG(bool, 3);
    ARG(v8::Local<v8::Array>, 4);

    if (!cb.IsEmpty())
        hr = acb_importKey(v0, v1, v2, v3, v4, cb, args);
    else
        hr = ac_importKey(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}
}
