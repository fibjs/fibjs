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

class Stream_base;
class Buffer_base;
class Blob_base;

class Message_base : public object_base {
    DECLARE_CLASS(Message_base);

public:
    enum {
        C_TEXT = 1,
        C_BINARY = 2
    };

public:
    // Message_base
    static result_t _new(obj_ptr<Message_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_sent(bool& retVal) = 0;
    virtual result_t get_value(exlib::string& retVal) = 0;
    virtual result_t set_value(exlib::string newVal) = 0;
    virtual result_t get_params(obj_ptr<NArray>& retVal) = 0;
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t set_type(int32_t newVal) = 0;
    virtual result_t get_body(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t set_body(Stream_base* newVal) = 0;
    virtual result_t get_bodyUsed(bool& retVal) = 0;
    virtual result_t read(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t readAll(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t write(Buffer_base* data, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t text(exlib::string data, exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac) = 0;
    virtual result_t blob(exlib::string type, obj_ptr<Blob_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t json(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t json(Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t pack(v8::Local<v8::Value> data, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t pack(Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_length(int64_t& retVal) = 0;
    virtual result_t end(int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t end(Buffer_base* data, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t end(Buffer_base* data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t end(exlib::string data, exlib::string encoding, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t isEnded(bool& retVal) = 0;
    virtual result_t clear() = 0;
    virtual result_t sendTo(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac) = 0;
    virtual result_t readFrom(Stream_base* stm, v8::Local<v8::Object> options, AsyncEvent* ac) = 0;
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t get_lastError(exlib::string& retVal) = 0;
    virtual result_t set_lastError(exlib::string newVal) = 0;
    virtual result_t clone(obj_ptr<Message_base>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<Message_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_sent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_value(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_value(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_params(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_body(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_body(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_bodyUsed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_read(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_write(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_text(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_arrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_blob(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_end(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isEnded(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clear(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sendTo(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_readFrom(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stream(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_lastError(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_lastError(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(Message_base, read, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE1(Message_base, readAll, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(Message_base, write, Buffer_base*, int32_t);
    ASYNC_MEMBERVALUE2(Message_base, text, exlib::string, exlib::string);
    ASYNC_MEMBERVALUE1(Message_base, text, exlib::string);
    ASYNC_MEMBERVALUE1(Message_base, arrayBuffer, std::shared_ptr<v8::BackingStore>);
    ASYNC_MEMBERVALUE2(Message_base, blob, exlib::string, obj_ptr<Blob_base>);
    ASYNC_MEMBERVALUE1(Message_base, bytes, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(Message_base, json, v8::Local<v8::Value>, Variant);
    ASYNC_MEMBERVALUE1(Message_base, json, Variant);
    ASYNC_MEMBERVALUE2(Message_base, pack, v8::Local<v8::Value>, Variant);
    ASYNC_MEMBERVALUE1(Message_base, pack, Variant);
    ASYNC_MEMBERVALUE1(Message_base, end, int32_t);
    ASYNC_MEMBERVALUE2(Message_base, end, Buffer_base*, int32_t);
    ASYNC_MEMBERVALUE3(Message_base, end, Buffer_base*, exlib::string, int32_t);
    ASYNC_MEMBERVALUE3(Message_base, end, exlib::string, exlib::string, int32_t);
    ASYNC_MEMBER2(Message_base, sendTo, Stream_base*, v8::Local<v8::Object>);
    ASYNC_MEMBER2(Message_base, readFrom, Stream_base*, v8::Local<v8::Object>);
};
}

#include "ifs/Stream.h"
#include "ifs/Buffer.h"
#include "ifs/Blob.h"

namespace fibjs {
inline ClassInfo& Message_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "read", s_read, false, ClassData::ASYNC_ASYNC },
        { "readAll", s_readAll, false, ClassData::ASYNC_ASYNC },
        { "write", s_write, false, ClassData::ASYNC_ASYNC },
        { "text", s_text, false, ClassData::ASYNC_ASYNC },
        { "arrayBuffer", s_arrayBuffer, false, ClassData::ASYNC_ASYNC },
        { "blob", s_blob, false, ClassData::ASYNC_ASYNC },
        { "bytes", s_bytes, false, ClassData::ASYNC_ASYNC },
        { "json", s_json, false, ClassData::ASYNC_ASYNC },
        { "pack", s_pack, false, ClassData::ASYNC_ASYNC },
        { "end", s_end, false, ClassData::ASYNC_ASYNC },
        { "isEnded", s_isEnded, false, ClassData::ASYNC_SYNC },
        { "clear", s_clear, false, ClassData::ASYNC_SYNC },
        { "sendTo", s_sendTo, false, ClassData::ASYNC_ASYNC },
        { "readFrom", s_readFrom, false, ClassData::ASYNC_ASYNC },
        { "clone", s_clone, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "sent", s_get_sent, block_set, false },
        { "value", s_get_value, s_set_value, false },
        { "params", s_get_params, block_set, false },
        { "type", s_get_type, s_set_type, false },
        { "body", s_get_body, s_set_body, false },
        { "bodyUsed", s_get_bodyUsed, block_set, false },
        { "length", s_get_length, block_set, false },
        { "stream", s_get_stream, block_set, false },
        { "lastError", s_get_lastError, s_set_lastError, false }
    };

    static ClassData::ClassConst s_const[] = {
        { "TEXT", ClassData::CONST_Integer, { .intValue = C_TEXT } },
        { "BINARY", ClassData::CONST_Integer, { .intValue = C_BINARY } }
    };

    static ClassData s_cd = {
        "Message", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Message_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void Message_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Message_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t Message_base::load(v8::Local<v8::Value> v, obj_ptr<Message_base>& retVal)
{
    obj_ptr<Message_base> vr;

    LOAD_ENTER();

    LOAD_RETURN();
}

inline void Message_base::s_get_sent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_sent(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_value(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_value(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_value(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_value(v0);

    METHOD_VOID();
}

inline void Message_base::s_get_params(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_params(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_type(v0);

    METHOD_VOID();
}

inline void Message_base::s_get_body(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_body(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_body(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    hr = pInst->set_body(v0);

    METHOD_VOID();
}

inline void Message_base::s_get_bodyUsed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_bodyUsed(vr);

    METHOD_RETURN();
}

inline void Message_base::s_read(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.read");

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_read(v0, cb, args);
    else
        hr = pInst->ac_read(v0, vr);

    METHOD_RETURN();
}

inline void Message_base::s_readAll(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.readAll");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_readAll(cb, args);
    else
        hr = pInst->ac_readAll(vr);

    METHOD_RETURN();
}

inline void Message_base::s_write(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.write");

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_write(v0.get(), cb, args);
    else
        hr = pInst->ac_write(v0.get(), vr);

    METHOD_RETURN();
}

inline void Message_base::s_text(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.text");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_text(v0, cb, args);
    else
        hr = pInst->ac_text(v0, vr);

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_text(cb, args);
    else
        hr = pInst->ac_text(vr);

    METHOD_RETURN();
}

inline void Message_base::s_arrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::shared_ptr<v8::BackingStore> vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.arrayBuffer");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_arrayBuffer(cb, args);
    else
        hr = pInst->ac_arrayBuffer(vr);

    METHOD_RETURN();
}

inline void Message_base::s_blob(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Blob_base> vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.blob");

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_blob(v0, cb, args);
    else
        hr = pInst->ac_blob(v0, vr);

    METHOD_RETURN();
}

inline void Message_base::s_bytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.bytes");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_bytes(cb, args);
    else
        hr = pInst->ac_bytes(vr);

    METHOD_RETURN();
}

inline void Message_base::s_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.json");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_json(v0, cb, args);
    else
        hr = pInst->ac_json(v0, vr);

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_json(cb, args);
    else
        hr = pInst->ac_json(vr);

    METHOD_RETURN();
}

inline void Message_base::s_pack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.pack");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Value>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_pack(v0, cb, args);
    else
        hr = pInst->ac_pack(v0, vr);

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_pack(cb, args);
    else
        hr = pInst->ac_pack(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_length(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_length(vr);

    METHOD_RETURN();
}

inline void Message_base::s_end(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.end");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_end(cb, args);
    else
        hr = pInst->ac_end(vr);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_end(v0.get(), cb, args);
    else
        hr = pInst->ac_end(v0.get(), vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_end(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_end(v0.get(), v1, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "utf8");

    if (!cb.IsEmpty())
        hr = pInst->acb_end(v0, v1, cb, args);
    else
        hr = pInst->ac_end(v0, v1, vr);

    METHOD_RETURN();
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
    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.sendTo");

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_sendTo(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_sendTo(v0.get(), v1);

    METHOD_VOID();
}

inline void Message_base::s_readFrom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Message_base);
    ASYNC_METHOD_ENTER("Message.readFrom");

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_readFrom(v0.get(), v1, cb, args);
    else
        hr = pInst->ac_readFrom(v0.get(), v1);

    METHOD_VOID();
}

inline void Message_base::s_get_stream(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_stream(vr);

    METHOD_RETURN();
}

inline void Message_base::s_get_lastError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_lastError(vr);

    METHOD_RETURN();
}

inline void Message_base::s_set_lastError(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_lastError(v0);

    METHOD_VOID();
}

inline void Message_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Message_base> vr;

    METHOD_INSTANCE(Message_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clone(vr);

    METHOD_RETURN();
}
}
