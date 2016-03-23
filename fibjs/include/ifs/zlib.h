/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _zlib_base_H_
#define _zlib_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class Stream_base;

class zlib_base : public object_base
{
    DECLARE_CLASS(zlib_base);

public:
    enum{
        _NO_COMPRESSION = 0,
        _BEST_SPEED = 1,
        _BEST_COMPRESSION = 9,
        _DEFAULT_COMPRESSION = -1
    };

public:
    // zlib_base
    static result_t deflate(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t deflateTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t deflateTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t inflate(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t inflateTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac);
    static result_t inflateTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac);
    static result_t gzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t gzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac);
    static result_t gzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac);
    static result_t gunzip(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t gunzipTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac);
    static result_t gunzipTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac);
    static result_t deflateRaw(Buffer_base* data, int32_t level, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t deflateRawTo(Buffer_base* data, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t deflateRawTo(Stream_base* src, Stream_base* stm, int32_t level, AsyncEvent* ac);
    static result_t inflateRaw(Buffer_base* data, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac);
    static result_t inflateRawTo(Buffer_base* data, Stream_base* stm, AsyncEvent* ac);
    static result_t inflateRawTo(Stream_base* src, Stream_base* stm, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_NO_COMPRESSION(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BEST_SPEED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BEST_COMPRESSION(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_DEFAULT_COMPRESSION(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_deflate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_deflateTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_inflate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_inflateTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_gzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_gzipTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_gunzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_gunzipTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_deflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_deflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_inflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_inflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(zlib_base, deflate, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, deflateTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, deflateTo, Stream_base*, Stream_base*, int32_t);
    ASYNC_STATICVALUE2(zlib_base, inflate, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATIC2(zlib_base, inflateTo, Buffer_base*, Stream_base*);
    ASYNC_STATIC2(zlib_base, inflateTo, Stream_base*, Stream_base*);
    ASYNC_STATICVALUE2(zlib_base, gzip, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATIC2(zlib_base, gzipTo, Buffer_base*, Stream_base*);
    ASYNC_STATIC2(zlib_base, gzipTo, Stream_base*, Stream_base*);
    ASYNC_STATICVALUE2(zlib_base, gunzip, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATIC2(zlib_base, gunzipTo, Buffer_base*, Stream_base*);
    ASYNC_STATIC2(zlib_base, gunzipTo, Stream_base*, Stream_base*);
    ASYNC_STATICVALUE3(zlib_base, deflateRaw, Buffer_base*, int32_t, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, deflateRawTo, Buffer_base*, Stream_base*, int32_t);
    ASYNC_STATIC3(zlib_base, deflateRawTo, Stream_base*, Stream_base*, int32_t);
    ASYNC_STATICVALUE2(zlib_base, inflateRaw, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_STATIC2(zlib_base, inflateRawTo, Buffer_base*, Stream_base*);
    ASYNC_STATIC2(zlib_base, inflateRawTo, Stream_base*, Stream_base*);
};

}

#include "Buffer.h"
#include "Stream.h"

namespace fibjs
{
    inline ClassInfo& zlib_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"deflate", s_deflate, true},
            {"deflateTo", s_deflateTo, true},
            {"inflate", s_inflate, true},
            {"inflateTo", s_inflateTo, true},
            {"gzip", s_gzip, true},
            {"gzipTo", s_gzipTo, true},
            {"gunzip", s_gunzip, true},
            {"gunzipTo", s_gunzipTo, true},
            {"deflateRaw", s_deflateRaw, true},
            {"deflateRawTo", s_deflateRawTo, true},
            {"inflateRaw", s_inflateRaw, true},
            {"inflateRawTo", s_inflateRawTo, true}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"NO_COMPRESSION", s_get_NO_COMPRESSION, block_set, true},
            {"BEST_SPEED", s_get_BEST_SPEED, block_set, true},
            {"BEST_COMPRESSION", s_get_BEST_COMPRESSION, block_set, true},
            {"DEFAULT_COMPRESSION", s_get_DEFAULT_COMPRESSION, block_set, true}
        };

        static ClassData s_cd = 
        { 
            "zlib", s__new, NULL, 
            12, s_method, 0, NULL, 4, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void zlib_base::s_get_NO_COMPRESSION(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _NO_COMPRESSION;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void zlib_base::s_get_BEST_SPEED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BEST_SPEED;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void zlib_base::s_get_BEST_COMPRESSION(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BEST_COMPRESSION;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void zlib_base::s_get_DEFAULT_COMPRESSION(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _DEFAULT_COMPRESSION;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void zlib_base::s_deflate(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(2, 1);

        ARG(obj_ptr<Buffer_base>, 0);
        OPT_ARG(int32_t, 1, _DEFAULT_COMPRESSION);

        hr = ac_deflate(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void zlib_base::s_deflateTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(3, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);
        OPT_ARG(int32_t, 2, _DEFAULT_COMPRESSION);

        hr = ac_deflateTo(v0, v1, v2);

        METHOD_OVER(3, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);
        OPT_ARG(int32_t, 2, _DEFAULT_COMPRESSION);

        hr = ac_deflateTo(v0, v1, v2);

        METHOD_VOID();
    }

    inline void zlib_base::s_inflate(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = ac_inflate(v0, vr);

        METHOD_RETURN();
    }

    inline void zlib_base::s_inflateTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_inflateTo(v0, v1);

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_inflateTo(v0, v1);

        METHOD_VOID();
    }

    inline void zlib_base::s_gzip(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = ac_gzip(v0, vr);

        METHOD_RETURN();
    }

    inline void zlib_base::s_gzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_gzipTo(v0, v1);

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_gzipTo(v0, v1);

        METHOD_VOID();
    }

    inline void zlib_base::s_gunzip(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = ac_gunzip(v0, vr);

        METHOD_RETURN();
    }

    inline void zlib_base::s_gunzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_gunzipTo(v0, v1);

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_gunzipTo(v0, v1);

        METHOD_VOID();
    }

    inline void zlib_base::s_deflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(2, 1);

        ARG(obj_ptr<Buffer_base>, 0);
        OPT_ARG(int32_t, 1, _DEFAULT_COMPRESSION);

        hr = ac_deflateRaw(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void zlib_base::s_deflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(3, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);
        OPT_ARG(int32_t, 2, _DEFAULT_COMPRESSION);

        hr = ac_deflateRawTo(v0, v1, v2);

        METHOD_OVER(3, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);
        OPT_ARG(int32_t, 2, _DEFAULT_COMPRESSION);

        hr = ac_deflateRawTo(v0, v1, v2);

        METHOD_VOID();
    }

    inline void zlib_base::s_inflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = ac_inflateRaw(v0, vr);

        METHOD_RETURN();
    }

    inline void zlib_base::s_inflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_inflateRawTo(v0, v1);

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<Stream_base>, 1);

        hr = ac_inflateRawTo(v0, v1);

        METHOD_VOID();
    }

}

#endif

