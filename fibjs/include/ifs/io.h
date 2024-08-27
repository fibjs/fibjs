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

class MemoryStream_base;
class BufferedStream_base;
class RangeStream_base;
class Stream_base;

class io_base : public object_base {
    DECLARE_CLASS(io_base);

public:
    // io_base
    static result_t copyStream(Stream_base* from, Stream_base* to, int64_t bytes, int64_t& retVal, AsyncEvent* ac);
    static result_t bridge(Stream_base* stm1, Stream_base* stm2, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_copyStream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_bridge(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE4(io_base, copyStream, Stream_base*, Stream_base*, int64_t, int64_t);
    ASYNC_STATIC2(io_base, bridge, Stream_base*, Stream_base*);
};
}

#include "ifs/MemoryStream.h"
#include "ifs/BufferedStream.h"
#include "ifs/RangeStream.h"
#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& io_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "copyStream", s_static_copyStream, true, ClassData::ASYNC_ASYNC },
        { "bridge", s_static_bridge, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "MemoryStream", MemoryStream_base::class_info },
        { "BufferedStream", BufferedStream_base::class_info },
        { "RangeStream", RangeStream_base::class_info }
    };

    static ClassData s_cd = {
        "io", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void io_base::s_static_copyStream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);
    OPT_ARG(int64_t, 2, -1);

    if (!cb.IsEmpty())
        hr = acb_copyStream(v0, v1, v2, cb, args);
    else
        hr = ac_copyStream(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void io_base::s_static_bridge(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<Stream_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_bridge(v0, v1, cb, args);
    else
        hr = ac_bridge(v0, v1);

    METHOD_VOID();
}
}
