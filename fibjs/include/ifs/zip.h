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

class ZipFile_base;
class Buffer_base;
class SeekableStream_base;

class zip_base : public object_base {
    DECLARE_CLASS(zip_base);

public:
    enum {
        __ZIP_STORED = 0,
        __ZIP_DEFLATED = 1
    };

public:
    // zip_base
    static result_t isZipFile(exlib::string filename, bool& retVal, AsyncEvent* ac);
    static result_t open(exlib::string path, exlib::string mod, int32_t compress_type, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);
    static result_t open(Buffer_base* data, exlib::string mod, int32_t compress_type, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);
    static result_t open(SeekableStream_base* strm, exlib::string mod, int32_t compress_type, obj_ptr<ZipFile_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_isZipFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE2(zip_base, isZipFile, exlib::string, bool);
    ASYNC_STATICVALUE4(zip_base, open, exlib::string, exlib::string, int32_t, obj_ptr<ZipFile_base>);
    ASYNC_STATICVALUE4(zip_base, open, Buffer_base*, exlib::string, int32_t, obj_ptr<ZipFile_base>);
    ASYNC_STATICVALUE4(zip_base, open, SeekableStream_base*, exlib::string, int32_t, obj_ptr<ZipFile_base>);
};
}

#include "ifs/ZipFile.h"
#include "ifs/Buffer.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& zip_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "isZipFile", s_static_isZipFile, true },
        { "isZipFileSync", s_static_isZipFile, true },
        { "open", s_static_open, true },
        { "openSync", s_static_open, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "ZIP_STORED", __ZIP_STORED },
        { "ZIP_DEFLATED", __ZIP_DEFLATED }
    };

    static ClassData s_cd = {
        "zip", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void zip_base::s_static_isZipFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("zip.isZipFile");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_isZipFile(v0, cb, args);
    else
        hr = ac_isZipFile(v0, vr);

    METHOD_RETURN();
}

inline void zip_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<ZipFile_base> vr;

    METHOD_NAME("zip.open");
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "r");
    OPT_ARG(int32_t, 2, __ZIP_DEFLATED);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(exlib::string, 1, "r");
    OPT_ARG(int32_t, 2, __ZIP_DEFLATED);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 1);

    ARG(obj_ptr<SeekableStream_base>, 0);
    OPT_ARG(exlib::string, 1, "r");
    OPT_ARG(int32_t, 2, __ZIP_DEFLATED);

    if (!cb.IsEmpty())
        hr = acb_open(v0, v1, v2, cb, args);
    else
        hr = ac_open(v0, v1, v2, vr);

    METHOD_RETURN();
}
}
