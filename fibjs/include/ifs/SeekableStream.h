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
#include "ifs/Stream.h"

namespace fibjs {

class Stream_base;
class fs_base;
class Buffer_base;
class Stat_base;

class SeekableStream_base : public Stream_base {
    DECLARE_CLASS(SeekableStream_base);

public:
    // SeekableStream_base
    virtual result_t seek(int64_t offset, int32_t whence) = 0;
    virtual result_t tell(int64_t& retVal) = 0;
    virtual result_t rewind() = 0;
    virtual result_t size(int64_t& retVal) = 0;
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac) = 0;
    virtual result_t eof(bool& retVal) = 0;
    virtual result_t stat(obj_ptr<Stat_base>& retVal, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_seek(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_tell(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rewind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_size(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eof(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stat(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(SeekableStream_base, readAll, obj_ptr<Buffer_base>);
    ASYNC_MEMBER1(SeekableStream_base, truncate, int64_t);
    ASYNC_MEMBERVALUE1(SeekableStream_base, stat, obj_ptr<Stat_base>);
};
}

#include "ifs/fs.h"
#include "ifs/Buffer.h"
#include "ifs/Stat.h"

namespace fibjs {
inline ClassInfo& SeekableStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "seek", s_seek, false, ClassData::ASYNC_SYNC },
        { "tell", s_tell, false, ClassData::ASYNC_SYNC },
        { "rewind", s_rewind, false, ClassData::ASYNC_SYNC },
        { "size", s_size, false, ClassData::ASYNC_SYNC },
        { "readAll", s_readAll, false, ClassData::ASYNC_ASYNC },
        { "readAllSync", s_readAll, false, ClassData::ASYNC_SYNC },
        { "truncate", s_truncate, false, ClassData::ASYNC_ASYNC },
        { "truncateSync", s_truncate, false, ClassData::ASYNC_SYNC },
        { "eof", s_eof, false, ClassData::ASYNC_SYNC },
        { "stat", s_stat, false, ClassData::ASYNC_ASYNC },
        { "statSync", s_stat, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "SeekableStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &Stream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void SeekableStream_base::s_seek(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int64_t, 0);
    OPT_ARG(int32_t, 1, fs_base::C_SEEK_SET);

    hr = pInst->seek(v0, v1);

    METHOD_VOID();
}

inline void SeekableStream_base::s_tell(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->tell(vr);

    METHOD_RETURN();
}

inline void SeekableStream_base::s_rewind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->rewind();

    METHOD_VOID();
}

inline void SeekableStream_base::s_size(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->size(vr);

    METHOD_RETURN();
}

inline void SeekableStream_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_readAll(cb, args);
    else
        hr = pInst->ac_readAll(vr);

    METHOD_RETURN();
}

inline void SeekableStream_base::s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int64_t, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_truncate(v0, cb, args);
    else
        hr = pInst->ac_truncate(v0);

    METHOD_VOID();
}

inline void SeekableStream_base::s_eof(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->eof(vr);

    METHOD_RETURN();
}

inline void SeekableStream_base::s_stat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stat_base> vr;

    ASYNC_METHOD_INSTANCE(SeekableStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_stat(cb, args);
    else
        hr = pInst->ac_stat(vr);

    METHOD_RETURN();
}
}
