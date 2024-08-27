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

class zlib_constants_base;
class Stream_base;
class Buffer_base;

class zlib_base : public object_base {
    DECLARE_CLASS(zlib_base);

public:
    enum {
        C_NO_COMPRESSION = 0,
        C_BEST_SPEED = 1,
        C_BEST_COMPRESSION = 9,
        C_DEFAULT_COMPRESSION = -1
    };

public:
    // zlib_base
    static result_t createDeflate(Stream_base* to, obj_ptr<Stream_base>& retVal);
    static result_t createDeflateRaw(Stream_base* to, obj_ptr<Stream_base>& retVal);
    static result_t createGunzip(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal);
    static result_t createGzip(Stream_base* to, obj_ptr<Stream_base>& retVal);
    static result_t createInflate(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal);
    static result_t createInflateRaw(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal);
    static result_t deflate(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t deflateTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t deflateTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t inflate(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t inflateTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac);
    static result_t inflateTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac);
    static result_t gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t gzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac);
    static result_t gzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac);
    static result_t gunzip(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t gunzipTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac);
    static result_t gunzipTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac);
    static result_t deflateRaw(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t deflateRawTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t deflateRawTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t inflateRaw(Buffer_base* data, int32_t maxSize, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t inflateRawTo(Buffer_base* data, Stream_base* stm, int32_t maxSize, AsyncEvent* ac);
    static result_t inflateRawTo(Stream_base* src, Stream_base* stm, int32_t maxSize, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_createDeflate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createDeflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createGunzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createGzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createInflate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createInflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deflate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deflateTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inflate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inflateTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gzipTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gunzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_gunzipTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_deflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_inflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(zlib_base, deflate, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, deflateTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, deflateTo, Stream_base*, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, inflate, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, inflateTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, inflateTo, Stream_base*, Stream_base*, int32_t);
    ASYNC_STATICVALUE2(zlib_base, gzip, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATIC2(zlib_base, gzipTo, Buffer_base*, Stream_base*);
    ASYNC_STATIC2(zlib_base, gzipTo, Stream_base*, Stream_base*);
    ASYNC_STATICVALUE3(zlib_base, gunzip, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, gunzipTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, gunzipTo, Stream_base*, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, deflateRaw, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, deflateRawTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, deflateRawTo, Stream_base*, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, inflateRaw, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, inflateRawTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, inflateRawTo, Stream_base*, Stream_base*, int32_t);
};
}

#include "ifs/zlib_constants.h"
#include "ifs/Stream.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& zlib_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createDeflate", s_static_createDeflate, true, ClassData::ASYNC_SYNC },
        { "createDeflateRaw", s_static_createDeflateRaw, true, ClassData::ASYNC_SYNC },
        { "createGunzip", s_static_createGunzip, true, ClassData::ASYNC_SYNC },
        { "createGzip", s_static_createGzip, true, ClassData::ASYNC_SYNC },
        { "createInflate", s_static_createInflate, true, ClassData::ASYNC_SYNC },
        { "createInflateRaw", s_static_createInflateRaw, true, ClassData::ASYNC_SYNC },
        { "deflate", s_static_deflate, true, ClassData::ASYNC_ASYNC },
        { "deflateTo", s_static_deflateTo, true, ClassData::ASYNC_ASYNC },
        { "inflate", s_static_inflate, true, ClassData::ASYNC_ASYNC },
        { "inflateTo", s_static_inflateTo, true, ClassData::ASYNC_ASYNC },
        { "gzip", s_static_gzip, true, ClassData::ASYNC_ASYNC },
        { "gzipTo", s_static_gzipTo, true, ClassData::ASYNC_ASYNC },
        { "gunzip", s_static_gunzip, true, ClassData::ASYNC_ASYNC },
        { "gunzipTo", s_static_gunzipTo, true, ClassData::ASYNC_ASYNC },
        { "deflateRaw", s_static_deflateRaw, true, ClassData::ASYNC_ASYNC },
        { "deflateRawTo", s_static_deflateRawTo, true, ClassData::ASYNC_ASYNC },
        { "inflateRaw", s_static_inflateRaw, true, ClassData::ASYNC_ASYNC },
        { "inflateRawTo", s_static_inflateRawTo, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", zlib_constants_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "NO_COMPRESSION", C_NO_COMPRESSION },
        { "BEST_SPEED", C_BEST_SPEED },
        { "BEST_COMPRESSION", C_BEST_COMPRESSION },
        { "DEFAULT_COMPRESSION", C_DEFAULT_COMPRESSION }
    };

    static ClassData s_cd = {
        "zlib", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void zlib_base::s_static_createDeflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    hr = createDeflate(v0, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createDeflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    hr = createDeflateRaw(v0, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createGunzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createGunzip(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createGzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    hr = createGzip(v0, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createInflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createInflate(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createInflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createInflateRaw(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_deflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflate(v0, v1, cb, args);
    else
        hr = ac_deflate(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_deflateTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateTo(v0, v1, v2, cb, args);
    else
        hr = ac_deflateTo(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateTo(v0, v1, v2, cb, args);
    else
        hr = ac_deflateTo(v0, v1, v2);

    METHOD_VOID();
}

inline void zlib_base::s_static_inflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_inflate(v0, v1, cb, args);
    else
        hr = ac_inflate(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_inflateTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateTo(v0, v1, v2, cb, args);
    else
        hr = ac_inflateTo(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateTo(v0, v1, v2, cb, args);
    else
        hr = ac_inflateTo(v0, v1, v2);

    METHOD_VOID();
}

inline void zlib_base::s_static_gzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = acb_gzip(v0, cb, args);
    else
        hr = ac_gzip(v0, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_gzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_gzipTo(v0, v1, cb, args);
    else
        hr = ac_gzipTo(v0, v1);

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_gzipTo(v0, v1, cb, args);
    else
        hr = ac_gzipTo(v0, v1);

    METHOD_VOID();
}

inline void zlib_base::s_static_gunzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_gunzip(v0, v1, cb, args);
    else
        hr = ac_gunzip(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_gunzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_gunzipTo(v0, v1, v2, cb, args);
    else
        hr = ac_gunzipTo(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_gunzipTo(v0, v1, v2, cb, args);
    else
        hr = ac_gunzipTo(v0, v1, v2);

    METHOD_VOID();
}

inline void zlib_base::s_static_deflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateRaw(v0, v1, cb, args);
    else
        hr = ac_deflateRaw(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_deflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateRawTo(v0, v1, v2, cb, args);
    else
        hr = ac_deflateRawTo(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateRawTo(v0, v1, v2, cb, args);
    else
        hr = ac_deflateRawTo(v0, v1, v2);

    METHOD_VOID();
}

inline void zlib_base::s_static_inflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateRaw(v0, v1, cb, args);
    else
        hr = ac_inflateRaw(v0, v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_inflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateRawTo(v0, v1, v2, cb, args);
    else
        hr = ac_inflateRawTo(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateRawTo(v0, v1, v2, cb, args);
    else
        hr = ac_inflateRawTo(v0, v1, v2);

    METHOD_VOID();
}
}
