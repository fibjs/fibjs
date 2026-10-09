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
class Gzip_base;
class Gunzip_base;
class Deflate_base;
class Inflate_base;
class DeflateRaw_base;
class InflateRaw_base;
class Unzip_base;
class Stream_base;
class Buffer_base;

class zlib_base : public object_base {
    DECLARE_CLASS(zlib_base);

public:
    using Union_deflate_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_deflate_level = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_deflateTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_inflate_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_inflate_maxSize = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_inflateTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_gzip_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_gzipTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_gunzip_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_gunzip_maxSize = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_gunzipTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_deflateRaw_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_deflateRaw_level = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_deflateRawTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_inflateRaw_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_inflateRaw_maxSize = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_inflateRawTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_zip_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_zip_level = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_zipTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;
    using Union_unzip_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_unzip_maxSize = std::variant<int32_t, v8::Local<v8::Object>>;
    using Union_unzipTo_data = std::variant<obj_ptr<Buffer_base>, obj_ptr<Stream_base>, exlib::string>;

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
    static result_t deflate(Union_deflate_data data, Union_deflate_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t deflateTo(Union_deflateTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac);
    static result_t inflate(Union_inflate_data data, Union_inflate_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t inflateTo(Union_inflateTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac);
    static result_t gzip(Union_gzip_data data, v8::Local<v8::Object> options, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t gzipTo(Union_gzipTo_data data, Stream_base* stm, AsyncHandle ac);
    static result_t gunzip(Union_gunzip_data data, Union_gunzip_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t gunzipTo(Union_gunzipTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac);
    static result_t deflateRaw(Union_deflateRaw_data data, Union_deflateRaw_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t deflateRawTo(Union_deflateRawTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac);
    static result_t inflateRaw(Union_inflateRaw_data data, Union_inflateRaw_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t inflateRawTo(Union_inflateRawTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac);
    static result_t createZip(Stream_base* to, int32_t level, obj_ptr<Stream_base>& retVal);
    static result_t createUnzip(Stream_base* to, int32_t maxSize, obj_ptr<Stream_base>& retVal);
    static result_t zip(Union_zip_data data, Union_zip_level level, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t zipTo(Union_zipTo_data data, Stream_base* stm, int32_t level, AsyncHandle ac);
    static result_t unzip(Union_unzip_data data, Union_unzip_maxSize maxSize, obj_ptr<Buffer_base>& retVal, AsyncHandle ac);
    static result_t unzipTo(Union_unzipTo_data data, Stream_base* stm, int32_t maxSize, AsyncHandle ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<zlib_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

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
    static void s_static_createZip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createUnzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_zip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_zipTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unzip(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unzipTo(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(zlib_base, deflate, Union_deflate_data, Union_deflate_level, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, deflateTo, Union_deflateTo_data, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, inflate, Union_inflate_data, Union_inflate_maxSize, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, inflateTo, Union_inflateTo_data, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, gzip, Union_gzip_data, v8::Local<v8::Object>, obj_ptr<Buffer_base>);
    ASYNC_STATIC2(zlib_base, gzipTo, Union_gzipTo_data, Stream_base*);
    ASYNC_STATICVALUE3(zlib_base, gunzip, Union_gunzip_data, Union_gunzip_maxSize, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, gunzipTo, Union_gunzipTo_data, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, deflateRaw, Union_deflateRaw_data, Union_deflateRaw_level, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, deflateRawTo, Union_deflateRawTo_data, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, inflateRaw, Union_inflateRaw_data, Union_inflateRaw_maxSize, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, inflateRawTo, Union_inflateRawTo_data, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, zip, Union_zip_data, Union_zip_level, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, zipTo, Union_zipTo_data, Stream_base*, int32_t);
    ASYNC_STATICVALUE3(zlib_base, unzip, Union_unzip_data, Union_unzip_maxSize, obj_ptr<Buffer_base>);
    ASYNC_STATIC3(zlib_base, unzipTo, Union_unzipTo_data, Stream_base*, int32_t);
};
}

#include "ifs/zlib_constants.h"
#include "ifs/Gzip.h"
#include "ifs/Gunzip.h"
#include "ifs/Deflate.h"
#include "ifs/Inflate.h"
#include "ifs/DeflateRaw.h"
#include "ifs/InflateRaw.h"
#include "ifs/Unzip.h"
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
        { "inflateRawTo", s_static_inflateRawTo, true, ClassData::ASYNC_ASYNC },
        { "createZip", s_static_createZip, true, ClassData::ASYNC_SYNC },
        { "createUnzip", s_static_createUnzip, true, ClassData::ASYNC_SYNC },
        { "zip", s_static_zip, true, ClassData::ASYNC_ASYNC },
        { "zipTo", s_static_zipTo, true, ClassData::ASYNC_ASYNC },
        { "unzip", s_static_unzip, true, ClassData::ASYNC_ASYNC },
        { "unzipTo", s_static_unzipTo, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "constants", zlib_constants_base::class_info },
        { "Gzip", Gzip_base::class_info },
        { "Gunzip", Gunzip_base::class_info },
        { "Deflate", Deflate_base::class_info },
        { "Inflate", Inflate_base::class_info },
        { "DeflateRaw", DeflateRaw_base::class_info },
        { "InflateRaw", InflateRaw_base::class_info },
        { "Unzip", Unzip_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "NO_COMPRESSION", ClassData::CONST_Integer, { .intValue = C_NO_COMPRESSION } },
        { "BEST_SPEED", ClassData::CONST_Integer, { .intValue = C_BEST_SPEED } },
        { "BEST_COMPRESSION", ClassData::CONST_Integer, { .intValue = C_BEST_COMPRESSION } },
        { "DEFAULT_COMPRESSION", ClassData::CONST_Integer, { .intValue = C_DEFAULT_COMPRESSION } }
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

    hr = createDeflate(v0.get(), vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createDeflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    hr = createDeflateRaw(v0.get(), vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createGunzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createGunzip(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createGzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    hr = createGzip(v0.get(), vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createInflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createInflate(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createInflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createInflateRaw(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_deflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.deflate");

    METHOD_OVER(2, 1);

    ARG(Union_deflate_data, 0);
    OPT_ARG(Union_deflate_level, 1, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflate(v0, v1, cb, args);
    else
        hr = ac_deflate(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_deflateTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.deflateTo");

    METHOD_OVER(3, 2);

    ARG(Union_deflateTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_deflateTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_inflate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.inflate");

    METHOD_OVER(2, 1);

    ARG(Union_inflate_data, 0);
    OPT_ARG(Union_inflate_maxSize, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_inflate(v0, v1, cb, args);
    else
        hr = ac_inflate(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_inflateTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.inflateTo");

    METHOD_OVER(3, 2);

    ARG(Union_inflateTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_inflateTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_gzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.gzip");

    METHOD_OVER(2, 1);

    ARG(Union_gzip_data, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_gzip(v0, v1, cb, args);
    else
        hr = ac_gzip(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_gzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.gzipTo");

    METHOD_OVER(2, 2);

    ARG(Union_gzipTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_gzipTo(v0, v1.get(), cb, args);
    else
        hr = ac_gzipTo(v0, v1.get());

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_gunzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.gunzip");

    METHOD_OVER(2, 1);

    ARG(Union_gunzip_data, 0);
    OPT_ARG(Union_gunzip_maxSize, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_gunzip(v0, v1, cb, args);
    else
        hr = ac_gunzip(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_gunzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.gunzipTo");

    METHOD_OVER(3, 2);

    ARG(Union_gunzipTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_gunzipTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_gunzipTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_deflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.deflateRaw");

    METHOD_OVER(2, 1);

    ARG(Union_deflateRaw_data, 0);
    OPT_ARG(Union_deflateRaw_level, 1, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateRaw(v0, v1, cb, args);
    else
        hr = ac_deflateRaw(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_deflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.deflateRawTo");

    METHOD_OVER(3, 2);

    ARG(Union_deflateRawTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_deflateRawTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_deflateRawTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_inflateRaw(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.inflateRaw");

    METHOD_OVER(2, 1);

    ARG(Union_inflateRaw_data, 0);
    OPT_ARG(Union_inflateRaw_maxSize, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateRaw(v0, v1, cb, args);
    else
        hr = ac_inflateRaw(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_inflateRawTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.inflateRawTo");

    METHOD_OVER(3, 2);

    ARG(Union_inflateRawTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_inflateRawTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_inflateRawTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_createZip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, C_DEFAULT_COMPRESSION);

    hr = createZip(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_createUnzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int32_t, 1, -1);

    hr = createUnzip(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void zlib_base::s_static_zip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.zip");

    METHOD_OVER(2, 1);

    ARG(Union_zip_data, 0);
    OPT_ARG(Union_zip_level, 1, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_zip(v0, v1, cb, args);
    else
        hr = ac_zip(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_zipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.zipTo");

    METHOD_OVER(3, 2);

    ARG(Union_zipTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, C_DEFAULT_COMPRESSION);

    if (!cb.IsEmpty())
        hr = acb_zipTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_zipTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}

inline void zlib_base::s_static_unzip(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_ENTER("zlib.unzip");

    METHOD_OVER(2, 1);

    ARG(Union_unzip_data, 0);
    OPT_ARG(Union_unzip_maxSize, 1, -1);

    if (!cb.IsEmpty())
        hr = acb_unzip(v0, v1, cb, args);
    else
        hr = ac_unzip(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void zlib_base::s_static_unzipTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("zlib.unzipTo");

    METHOD_OVER(3, 2);

    ARG(Union_unzipTo_data, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_unzipTo(v0, v1.get(), v2, cb, args);
    else
        hr = ac_unzipTo(v0, v1.get(), v2);

    ASYNC_METHOD_VOID();
}
}
