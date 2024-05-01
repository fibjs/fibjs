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

class Stat_base;
class Buffer_base;

class FileHandle_base : public object_base {
    DECLARE_CLASS(FileHandle_base);

public:
    // FileHandle_base
    virtual result_t get_fd(int32_t& retVal) = 0;
    virtual result_t chmod(int32_t mode, AsyncEvent* ac) = 0;
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t read(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* buffer, int32_t offset, int32_t length, int32_t position, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(exlib::string string, int32_t position, exlib::string encoding, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_fd(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(FileHandle_base, chmod, int32_t);
    ASYNC_MEMBERVALUE1(FileHandle_base, stat, obj_ptr<Stat_base>);
    ASYNC_MEMBERVALUE5(FileHandle_base, read, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE5(FileHandle_base, write, Buffer_base*, int32_t, int32_t, int32_t, int32_t);
    ASYNC_MEMBERVALUE4(FileHandle_base, write, exlib::string, int32_t, exlib::string, int32_t);
    ASYNC_MEMBER0(FileHandle_base, close);
};
}

#include "ifs/Stat.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& FileHandle_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "chmod", s_chmod, false, ClassData::ASYNC_ASYNC },
        { "chmodSync", s_chmod, false, ClassData::ASYNC_SYNC },
        { "stat", s_stat, false, ClassData::ASYNC_ASYNC },
        { "statSync", s_stat, false, ClassData::ASYNC_SYNC },
        { "read", s_read, false, ClassData::ASYNC_ASYNC },
        { "readSync", s_read, false, ClassData::ASYNC_SYNC },
        { "write", s_write, false, ClassData::ASYNC_ASYNC },
        { "writeSync", s_write, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "closeSync", s_close, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "fd", s_get_fd, block_set, false }
    };

    static ClassData s_cd = {
        "FileHandle", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void FileHandle_base::s_get_fd(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(FileHandle_base);
    PROPERTY_ENTER();

    hr = pInst->get_fd(vr);

    METHOD_RETURN();
}

inline void FileHandle_base::s_chmod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(FileHandle_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_chmod(v0, cb, args);
    else
        hr = pInst->ac_chmod(v0);

    METHOD_VOID();
}

inline void FileHandle_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_stat(cb, args);
    else
        hr = pInst->ac_stat(vr);

    METHOD_RETURN();
}

inline void FileHandle_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, 0);
    OPT_ARG(int32_t, 3, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_read(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_read(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void FileHandle_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(FileHandle_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(4, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(int32_t, 2, -1);
    OPT_ARG(int32_t, 3, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, v2, v3, cb, args);
    else
        hr = pInst->ac_write(v0, v1, v2, v3, vr);

    ASYNC_METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, -1);
    OPT_ARG(exlib::string, 2, "utf8");

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_write(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void FileHandle_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(FileHandle_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}
}
