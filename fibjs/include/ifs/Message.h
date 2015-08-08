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

namespace fibjs
{

class List_base;
class SeekableStream_base;
class Buffer_base;
class Stream_base;
class BufferedStream_base;

class Message_base : public object_base
{
    DECLARE_CLASS(Message_base);

public:
    // Message_base
    static result_t _new(obj_ptr<Message_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_value(std::string& retVal) = 0;
    virtual result_t set_value(const char* newVal) = 0;
    virtual result_t get_params(obj_ptr<List_base>& retVal) = 0;
    virtual result_t set_params(List_base* newVal) = 0;
    virtual result_t get_result(Variant& retVal) = 0;
    virtual result_t set_result(Variant newVal) = 0;
    virtual result_t get_body(obj_ptr<SeekableStream_base>& retVal) = 0;
    virtual result_t set_body(SeekableStream_base* newVal) = 0;
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t get_length(int64_t& retVal) = 0;
    virtual result_t clear() = 0;
    virtual result_t sendTo(Stream_base* stm, AsyncEvent* ac) = 0;
    virtual result_t readFrom(BufferedStream_base* stm, AsyncEvent* ac) = 0;
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t get_response(obj_ptr<Message_base>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_params(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_params(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_result(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_result(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sendTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFrom(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stream(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_response(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
    ASYNC_MEMBERVALUE2(Message_base, read, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE1(Message_base, readAll, obj_ptr<Buffer_base>);
    ASYNC_MEMBER1(Message_base, write, Buffer_base*);
    ASYNC_MEMBER1(Message_base, sendTo, Stream_base*);
    ASYNC_MEMBER1(Message_base, readFrom, BufferedStream_base*);
};

}

#include "List.h"
#include "SeekableStream.h"
#include "Buffer.h"
#include "Stream.h"
#include "BufferedStream.h"

namespace fibjs
{
    inline ClassInfo& Message_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"read", s_read, false},
            {"readAll", s_readAll, false},
            {"write", s_write, false},
            {"clear", s_clear, false},
            {"sendTo", s_sendTo, false},
            {"readFrom", s_readFrom, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"value", s_get_value, s_set_value, false},
            {"params", s_get_params, s_set_params, false},
            {"result", s_get_result, s_set_result, false},
            {"body", s_get_body, s_set_body, false},
            {"length", s_get_length, block_set, false},
            {"stream", s_get_stream, block_set, false},
            {"response", s_get_response, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "Message", s__new, 
            6, s_method, 0, NULL, 7, s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Message_base::s_get_value(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        std::string vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_value(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_set_value(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        PROPERTY_VAL(arg_string);
        hr = pInst->set_value(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Message_base::s_get_params(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<List_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_params(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_set_params(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        PROPERTY_VAL(obj_ptr<List_base>);
        hr = pInst->set_params(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Message_base::s_get_result(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        Variant vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_result(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_set_result(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        PROPERTY_VAL(Variant);
        hr = pInst->set_result(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Message_base::s_get_body(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<SeekableStream_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_body(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_set_body(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        PROPERTY_VAL(obj_ptr<SeekableStream_base>);
        hr = pInst->set_body(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void Message_base::s_get_length(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int64_t vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_length(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_get_stream(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Stream_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_stream(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_get_response(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Message_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Message_base);

        hr = pInst->get_response(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Message_base::__new(const T& args)
    {
        obj_ptr<Message_base> vr;

        CONSTRUCT_ENTER(0, 0);

        hr = _new(vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void Message_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(Message_base);
        METHOD_ENTER(1, 0);

        OPT_ARG(int32_t, 0, -1);

        hr = pInst->ac_read(v0, vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(Message_base);
        METHOD_ENTER(0, 0);

        hr = pInst->ac_readAll(vr);

        METHOD_RETURN();
    }

    inline void Message_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Message_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->ac_write(v0);

        METHOD_VOID();
    }

    inline void Message_base::s_clear(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Message_base);
        METHOD_ENTER(0, 0);

        hr = pInst->clear();

        METHOD_VOID();
    }

    inline void Message_base::s_sendTo(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Message_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Stream_base>, 0);

        hr = pInst->ac_sendTo(v0);

        METHOD_VOID();
    }

    inline void Message_base::s_readFrom(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Message_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<BufferedStream_base>, 0);

        hr = pInst->ac_readFrom(v0);

        METHOD_VOID();
    }

}

#endif

