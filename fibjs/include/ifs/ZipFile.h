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
class SeekableStream_base;

class ZipFile_base : public object_base {
    DECLARE_CLASS(ZipFile_base);

public:
    // ZipFile_base
    virtual result_t namelist(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t infolist(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t getinfo(exlib::string member, obj_ptr<NObject>& retVal, AsyncEvent* ac) = 0;
    virtual result_t read(exlib::string member, exlib::string password, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t readAll(exlib::string password, obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t extract(exlib::string member, exlib::string path, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t extract(exlib::string member, SeekableStream_base* strm, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t extractAll(exlib::string path, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t write(exlib::string filename, exlib::string inZipName, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, exlib::string inZipName, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t write(SeekableStream_base* strm, exlib::string inZipName, exlib::string password, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_namelist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_infolist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getinfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extract(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_extractAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(ZipFile_base, namelist, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE1(ZipFile_base, infolist, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE2(ZipFile_base, getinfo, exlib::string, obj_ptr<NObject>);
    ASYNC_MEMBERVALUE3(ZipFile_base, read, exlib::string, exlib::string, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(ZipFile_base, readAll, exlib::string, obj_ptr<NArray>);
    ASYNC_MEMBER3(ZipFile_base, extract, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER3(ZipFile_base, extract, exlib::string, SeekableStream_base*, exlib::string);
    ASYNC_MEMBER2(ZipFile_base, extractAll, exlib::string, exlib::string);
    ASYNC_MEMBER3(ZipFile_base, write, exlib::string, exlib::string, exlib::string);
    ASYNC_MEMBER3(ZipFile_base, write, Buffer_base*, exlib::string, exlib::string);
    ASYNC_MEMBER3(ZipFile_base, write, SeekableStream_base*, exlib::string, exlib::string);
    ASYNC_MEMBER0(ZipFile_base, close);
};
}

#include "ifs/Buffer.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& ZipFile_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "namelist", s_namelist, false, true },
        { "namelistSync", s_namelist, false, false },
        { "infolist", s_infolist, false, true },
        { "infolistSync", s_infolist, false, false },
        { "getinfo", s_getinfo, false, true },
        { "getinfoSync", s_getinfo, false, false },
        { "read", s_read, false, true },
        { "readSync", s_read, false, false },
        { "readAll", s_readAll, false, true },
        { "readAllSync", s_readAll, false, false },
        { "extract", s_extract, false, true },
        { "extractSync", s_extract, false, false },
        { "extractAll", s_extractAll, false, true },
        { "extractAllSync", s_extractAll, false, false },
        { "write", s_write, false, true },
        { "writeSync", s_write, false, false },
        { "close", s_close, false, true },
        { "closeSync", s_close, false, false }
    };

    static ClassData s_cd = {
        "ZipFile", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ZipFile_base::s_namelist(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_namelist(cb, args);
    else
        hr = pInst->ac_namelist(vr);

    METHOD_RETURN();
}

inline void ZipFile_base::s_infolist(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_infolist(cb, args);
    else
        hr = pInst->ac_infolist(vr);

    METHOD_RETURN();
}

inline void ZipFile_base::s_getinfo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getinfo(v0, cb, args);
    else
        hr = pInst->ac_getinfo(v0, vr);

    METHOD_RETURN();
}

inline void ZipFile_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_read(v0, v1, cb, args);
    else
        hr = pInst->ac_read(v0, v1, vr);

    METHOD_RETURN();
}

inline void ZipFile_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_readAll(v0, cb, args);
    else
        hr = pInst->ac_readAll(v0, vr);

    METHOD_RETURN();
}

inline void ZipFile_base::s_extract(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_extract(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_extract(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<SeekableStream_base>, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_extract(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_extract(v0, v1, v2);

    METHOD_VOID();
}

inline void ZipFile_base::s_extractAll(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_extractAll(v0, v1, cb, args);
    else
        hr = pInst->ac_extractAll(v0, v1);

    METHOD_VOID();
}

inline void ZipFile_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_write(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_write(v0, v1, v2);

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<SeekableStream_base>, 0);
    ARG(exlib::string, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_write(v0, v1, v2);

    METHOD_VOID();
}

inline void ZipFile_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(ZipFile_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}
}
