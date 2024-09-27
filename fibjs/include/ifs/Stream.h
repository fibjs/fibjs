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

class Stream_base : public object_base {
    DECLARE_CLASS(Stream_base);

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal) = 0;
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t flush(AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_get_fd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_flush(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_copyTo(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(Stream_base, read, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBER1(Stream_base, write, Buffer_base*);
    ASYNC_MEMBER0(Stream_base, flush);
    ASYNC_MEMBER0(Stream_base, close);
    ASYNC_MEMBERVALUE3(Stream_base, copyTo, Stream_base*, int64_t, int64_t);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& Stream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "read", s_read, false, ClassData::ASYNC_ASYNC },
        { "write", s_write, false, ClassData::ASYNC_ASYNC },
        { "flush", s_flush, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "copyTo", s_copyTo, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "fd", s_get_fd, block_set, false }
    };

    static ClassData s_cd = {
        "Stream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Stream_base::s_get_fd(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_fd(vr);

    METHOD_RETURN();
}

inline void Stream_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_read(v0, cb, args);
    else
        hr = pInst->ac_read(v0, vr);

    METHOD_RETURN();
}

inline void Stream_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, cb, args);
    else
        hr = pInst->ac_write(v0);

    METHOD_VOID();
}

inline void Stream_base::s_flush(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_flush(cb, args);
    else
        hr = pInst->ac_flush();

    METHOD_VOID();
}

inline void Stream_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}

inline void Stream_base::s_copyTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int64_t, 1, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_copyTo(v0, v1, cb, args);
    else
        hr = pInst->ac_copyTo(v0, v1, vr);

    METHOD_RETURN();
}
}
