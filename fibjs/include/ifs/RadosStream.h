/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RadosStream_base_H_
#define _RadosStream_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "SeekableStream.h"

namespace fibjs {

class SeekableStream_base;
class RadosStat_base;
class Buffer_base;

class RadosStream_base : public SeekableStream_base {
    DECLARE_CLASS(RadosStream_base);

public:
    // RadosStream_base
    virtual result_t get_key(exlib::string& retVal) = 0;
    virtual result_t radosStat(obj_ptr<RadosStat_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t writeFull(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t append(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t truncate(int64_t bytes, AsyncEvent* ac) = 0;
    virtual result_t flush(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_key(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_radosStat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeFull(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_flush(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(RadosStream_base, radosStat, obj_ptr<RadosStat_base>);
    ASYNC_MEMBER1(RadosStream_base, writeFull, Buffer_base*);
    ASYNC_MEMBER1(RadosStream_base, append, Buffer_base*);
    ASYNC_MEMBER1(RadosStream_base, truncate, int64_t);
    ASYNC_MEMBER0(RadosStream_base, flush);
};
}

#include "RadosStat.h"
#include "Buffer.h"

namespace fibjs {
inline ClassInfo& RadosStream_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "radosStat", s_radosStat, false },
        { "writeFull", s_writeFull, false },
        { "append", s_append, false },
        { "truncate", s_truncate, false },
        { "flush", s_flush, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "key", s_get_key, block_set, false }
    };

    static ClassData s_cd = {
        "RadosStream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &SeekableStream_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RadosStream_base::s_get_key(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(RadosStream_base);
    PROPERTY_ENTER();

    hr = pInst->get_key(vr);

    METHOD_RETURN();
}

inline void RadosStream_base::s_radosStat(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RadosStat_base> vr;

    METHOD_INSTANCE(RadosStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_radosStat(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_radosStat(vr);

    METHOD_RETURN();
}

inline void RadosStream_base::s_writeFull(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_writeFull(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_writeFull(v0);

    METHOD_VOID();
}

inline void RadosStream_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_append(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_append(v0);

    METHOD_VOID();
}

inline void RadosStream_base::s_truncate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(int64_t, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_truncate(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_truncate(v0);

    METHOD_VOID();
}

inline void RadosStream_base::s_flush(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RadosStream_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_flush(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_flush();

    METHOD_VOID();
}
}

#endif
