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
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class Buffer_base;

class ZlibCodec_base : public EventEmitter_base {
    DECLARE_CLASS(ZlibCodec_base);
    EVENT_SUPPORT();

public:
    // ZlibCodec_base
    virtual result_t _processChunk(Buffer_base* chunk, int32_t flushFlag, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t close() = 0;
    virtual result_t reset() = 0;
    virtual result_t get__handle(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t set__handle(v8::Local<v8::Value> newVal) = 0;
    virtual result_t params(int32_t level, int32_t strategy) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<ZlibCodec_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s__processChunk(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get__handle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set__handle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_params(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& ZlibCodec_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "_processChunk", s__processChunk, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "reset", s_reset, false, ClassData::ASYNC_SYNC },
        { "params", s_params, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "_handle", s_get__handle, s_set__handle, false }
    };

    static ClassData s_cd = {
        "ZlibCodec", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ZlibCodec_base::s__processChunk(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(ZlibCodec_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);

    hr = pInst->_processChunk(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void ZlibCodec_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ZlibCodec_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_VOID();
}

inline void ZlibCodec_base::s_reset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ZlibCodec_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->reset();

    METHOD_VOID();
}

inline void ZlibCodec_base::s_get__handle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(ZlibCodec_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get__handle(vr);

    METHOD_RETURN();
}

inline void ZlibCodec_base::s_set__handle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ZlibCodec_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    hr = pInst->set__handle(v0);

    METHOD_VOID();
}

inline void ZlibCodec_base::s_params(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(ZlibCodec_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    hr = pInst->params(v0, v1);

    METHOD_VOID();
}
}
