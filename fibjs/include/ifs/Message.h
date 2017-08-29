/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Message_base_H_
#define _Message_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class List_base;
class SeekableStream_base;
class Buffer_base;
class Stream_base;

class Message_base : public object_base {
    DECLARE_CLASS(Message_base);

public:
    enum {
        _TEXT = 1,
        _BINARY = 2
    };

public:
    // Message_base
    static result_t _new(obj_ptr<Message_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_value(exlib::string& retVal) = 0;
    virtual result_t set_value(exlib::string newVal) = 0;
    virtual result_t get_params(obj_ptr<List_base>& retVal) = 0;
    virtual result_t set_params(List_base* newVal) = 0;
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t set_type(int32_t newVal) = 0;
    virtual result_t get_data(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal) = 0;
    virtual result_t set_body(SeekableStream_base* newVal) = 0;
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t json(v8::Local<v8::Value> data, AsyncEvent* ac) = 0;
    virtual result_t get_length(int64_t& retVal) = 0;
    virtual result_t end() = 0;
    virtual result_t isEnded(bool& retVal) = 0;
    virtual result_t clear() = 0;
    virtual result_t sendTo(Stream_base* stm, AsyncEvent* ac) = 0;
    virtual result_t readFrom(Stream_base* stm, AsyncEvent* ac) = 0;
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t get_response(obj_ptr<Message_base>& retVal) = 0;
    virtual result_t get_lastError(exlib::string& retVal) = 0;
    virtual result_t set_lastError(exlib::string newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s_get_TEXT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_BINARY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_params(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_params(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_type(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_end(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isEnded(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sendTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFrom(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stream(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_response(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_lastError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_lastError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_MEMBERVALUE2(Message_base, read, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE1(Message_base, readAll, obj_ptr<Buffer_base>);
    ASYNC_MEMBER1(Message_base, write, Buffer_base*);
    ASYNC_MEMBER1(Message_base, json, v8::Local<v8::Value>);
    ASYNC_MEMBER1(Message_base, sendTo, Stream_base*);
    ASYNC_MEMBER1(Message_base, readFrom, Stream_base*);
};
}

#include "List.h"
#include "SeekableStream.h"
#include "Buffer.h"
#include "Stream.h"

namespace fibjs {
inline ClassInfo& Message_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "read", s_read, false },
        { "readSync", s_read, false },
        { "readAll", s_readAll, false },
        { "readAllSync", s_readAll, false },
        { "write", s_write, false },
        { "writeSync", s_write, false },
        { "json", s_json, false },
        { "jsonSync", s_json, false },
        { "end", s_end, false },
        { "isEnded", s_isEnded, false },
        { "clear", s_clear, false },
        { "sendTo", s_sendTo, false },
        { "sendToSync", s_sendTo, false },
        { "readFrom", s_readFrom, false },
        { "readFromSync", s_readFrom, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "TEXT", s_get_TEXT, block_set, true },
        { "BINARY", s_get_BINARY, block_set, true },
        { "value", s_get_value, s_set_value, false },
        { "params", s_get_params, s_set_params, false },
        { "type", s_get_type, s_set_type, false },
        { "data", s_get_data, block_set, false },
        { "body", s_get_body, s_set_body, false },
        { "length", s_get_length, block_set, false },
        { "stream", s_get_stream, block_set, false },
        { "response", s_get_response, block_set, false },
        { "lastError", s_get_lastError, s_set_lastError, false }
    };

    static ClassData s_cd = {
        "Message", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Message_base::s_get_TEXT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _TEXT;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void Message_base::s_get_BINARY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _BINARY;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void Message_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Message_base::__new(const T& args)
{
    obj_ptr<Message_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Message_base::s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_value(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_value(v0);

    PROPERTY_SET_LEAVE();
}

inline void Message_base::s_get_params(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<List_base> vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_params(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_params(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(obj_ptr<List_base>);

    hr = pInst->set_params(v0);

    PROPERTY_SET_LEAVE();
}

inline void Message_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_type(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_type(v0);

    PROPERTY_SET_LEAVE();
}

inline void Message_base::s_get_data(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_data(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<SeekableStream_base> vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_body(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(obj_ptr<SeekableStream_base>);

    hr = pInst->set_body(v0);

    PROPERTY_SET_LEAVE();
}

inline void Message_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, -1);

    if (!cb.IsEmpty()) {
        pInst->acb_read(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_read(v0, vr);

    METHOD_RETURN();
}

inline void Message_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_readAll(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_readAll(vr);

    METHOD_RETURN();
}

inline void Message_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_write(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_write(v0);

    METHOD_VOID();
}

inline void Message_base::s_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_json(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_json(v0);

    METHOD_VOID();
}

inline void Message_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void Message_base::s_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->end();

    METHOD_VOID();
}

inline void Message_base::s_isEnded(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isEnded(vr);

    METHOD_RETURN();
}

inline void Message_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clear();

    METHOD_VOID();
}

inline void Message_base::s_sendTo(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_sendTo(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_sendTo(v0);

    METHOD_VOID();
}

inline void Message_base::s_readFrom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_readFrom(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_readFrom(v0);

    METHOD_VOID();
}

inline void Message_base::s_get_stream(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_stream(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_response(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Message_base> vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_response(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_lastError(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();

    hr = pInst->get_lastError(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_lastError(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Message_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_lastError(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
