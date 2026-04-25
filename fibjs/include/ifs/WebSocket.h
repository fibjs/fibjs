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
class WebSocketMessage_base;
class Handler_base;

class WebSocket_base : public EventEmitter_base {
    DECLARE_CLASS(WebSocket_base);
    EVENT_SUPPORT();

public:
    enum {
        C_CONTINUE = 0,
        C_TEXT = 1,
        C_BINARY = 2,
        C_CLOSE = 8,
        C_PING = 9,
        C_PONG = 10,
        C_CONNECTING = 0,
        C_OPEN = 1,
        C_CLOSING = 2,
        C_CLOSED = 3
    };

public:
    // WebSocket_base
    static result_t _new(exlib::string url, exlib::string protocol, exlib::string origin, obj_ptr<WebSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string url, std::vector<exlib::string>& protocols, exlib::string origin, obj_ptr<WebSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<WebSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_url(exlib::string& retVal) = 0;
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t get_origin(exlib::string& retVal) = 0;
    virtual result_t get_readyState(int32_t& retVal) = 0;
    virtual result_t close(int32_t code, exlib::string reason) = 0;
    virtual result_t send(exlib::string data) = 0;
    virtual result_t send(Buffer_base* data) = 0;
    virtual result_t ref(obj_ptr<WebSocket_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<WebSocket_base>& retVal) = 0;
    static result_t upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);
    static result_t upgrade(v8::Local<v8::Object> opts, v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<WebSocket_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_origin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_readyState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"
#include "ifs/WebSocketMessage.h"
#include "ifs/Handler.h"

namespace fibjs {
inline ClassInfo& WebSocket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "send", s_send, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC },
        { "upgrade", s_static_upgrade, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Message", WebSocketMessage_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "url", s_get_url, block_set, false },
        { "protocol", s_get_protocol, block_set, false },
        { "origin", s_get_origin, block_set, false },
        { "readyState", s_get_readyState, block_set, false },
        { "onopen", s_get_onopen, s_set_onopen, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onerror", s_get_onerror, s_set_onerror, false }
    };

    static ClassData::ClassConst s_const[] = {
        { "CONTINUE", ClassData::CONST_Integer, { .intValue = C_CONTINUE } },
        { "TEXT", ClassData::CONST_Integer, { .intValue = C_TEXT } },
        { "BINARY", ClassData::CONST_Integer, { .intValue = C_BINARY } },
        { "CLOSE", ClassData::CONST_Integer, { .intValue = C_CLOSE } },
        { "PING", ClassData::CONST_Integer, { .intValue = C_PING } },
        { "PONG", ClassData::CONST_Integer, { .intValue = C_PONG } },
        { "CONNECTING", ClassData::CONST_Integer, { .intValue = C_CONNECTING } },
        { "OPEN", ClassData::CONST_Integer, { .intValue = C_OPEN } },
        { "CLOSING", ClassData::CONST_Integer, { .intValue = C_CLOSING } },
        { "CLOSED", ClassData::CONST_Integer, { .intValue = C_CLOSED } }
    };

    static ClassData s_cd = {
        "WebSocket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebSocket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void WebSocket_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebSocket_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(exlib::string, 2, "");

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(std::vector<exlib::string>, 1);
    OPT_ARG(exlib::string, 2, "");

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t WebSocket_base::load(v8::Local<v8::Value> v, obj_ptr<WebSocket_base>& retVal)
{
    obj_ptr<WebSocket_base> vr;

    LOAD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(exlib::string, 2, "");

    hr = _new(v0, v1, v2, vr, args.This());

    LOAD_RETURN();
}

inline void WebSocket_base::s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_url(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_protocol(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_get_origin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_origin(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_get_readyState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_readyState(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 1000);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->close(v0, v1);

    METHOD_VOID();
}

inline void WebSocket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->send(v0);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->send(v0.get());

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("open", vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("open", v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("message", vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("message", v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close", vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebSocket_base> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebSocket_base> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_static_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = upgrade(v0, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = upgrade(v0, v1, vr);

    METHOD_RETURN();
}
}
