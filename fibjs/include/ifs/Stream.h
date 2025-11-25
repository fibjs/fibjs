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

class Stream_base : public EventEmitter_base {
    DECLARE_CLASS(Stream_base);
    EVENT_SUPPORT();

public:
    // Stream_base
    virtual result_t get_fd(int32_t& retVal) = 0;
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t resume(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t pause(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t flush(AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t copyTo(Stream_base* stm, int64_t bytes, int64_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t ref(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<Stream_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<Stream_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_fd(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resume(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pause(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_flush(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_copyTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ondata(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_ondata(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(Stream_base, read, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(Stream_base, write, Buffer_base*, int32_t);
    ASYNC_MEMBERVALUE3(Stream_base, write, Buffer_base*, exlib::string, int32_t);
    ASYNC_MEMBERVALUE3(Stream_base, write, exlib::string, exlib::string, int32_t);
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
        { "resume", s_resume, false, ClassData::ASYNC_SYNC },
        { "pause", s_pause, false, ClassData::ASYNC_SYNC },
        { "flush", s_flush, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "copyTo", s_copyTo, false, ClassData::ASYNC_ASYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "fd", s_get_fd, block_set, false },
        { "ondata", s_get_ondata, s_set_ondata, false },
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onerror", s_get_onerror, s_set_onerror, false }
    };

    static ClassData s_cd = {
        "Stream", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
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
    ASYNC_METHOD_ENTER("Stream.read");

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
    int32_t vr;

    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER("Stream.write");

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0.get(), cb, args);
    else
        hr = pInst->ac_write(v0.get(), vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_write(v0.get(), v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0, v1, cb, args);
    else
        hr = pInst->ac_write(v0, v1, vr);

    METHOD_RETURN();
}

inline void Stream_base::s_resume(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->resume(vr);

    METHOD_RETURN();
}

inline void Stream_base::s_pause(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->pause(vr);

    METHOD_RETURN();
}

inline void Stream_base::s_flush(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Stream_base);
    ASYNC_METHOD_ENTER("Stream.flush");

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
    ASYNC_METHOD_ENTER("Stream.close");

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
    ASYNC_METHOD_ENTER("Stream.copyTo");

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(int64_t, 1, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_copyTo(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_copyTo(v0.get(), v1, vr);

    METHOD_RETURN();
}

inline void Stream_base::s_get_ondata(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("data", vr);

    METHOD_RETURN();
}

inline void Stream_base::s_set_ondata(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("data", v0);

    METHOD_VOID();
}

inline void Stream_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close", vr);

    METHOD_RETURN();
}

inline void Stream_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}

inline void Stream_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void Stream_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}

inline void Stream_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void Stream_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Stream_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}
}
