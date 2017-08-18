/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _ws_base_H_
#define _ws_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class WebSocketMessage_base;
class WebSocketHandler_base;
class WebSocket_base;
class Stream_base;
class Handler_base;

class ws_base : public object_base {
    DECLARE_CLASS(ws_base);

public:
    enum {
        _CONTINUE = 0,
        _TEXT = 1,
        _BINARY = 2,
        _CLOSE = 8,
        _PING = 9,
        _PONG = 10,
        _CONNECTING = 0,
        _OPEN = 1,
        _CLOSING = 2,
        _CLOSED = 3
    };

public:
    // ws_base
    static result_t connect(exlib::string url, exlib::string origin, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);
    static result_t upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_CONTINUE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_TEXT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_BINARY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CLOSE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_PONG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CONNECTING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_OPEN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CLOSING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_CLOSED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(ws_base, connect, exlib::string, exlib::string, obj_ptr<Stream_base>);
};
}

#include "WebSocketMessage.h"
#include "WebSocketHandler.h"
#include "WebSocket.h"
#include "Stream.h"
#include "Handler.h"

namespace fibjs {
inline ClassInfo& ws_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_connect, true },
        { "connectSync", s_connect, true },
        { "upgrade", s_upgrade, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Message", WebSocketMessage_base::class_info },
        { "Handler", WebSocketHandler_base::class_info },
        { "Socket", WebSocket_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "CONTINUE", s_get_CONTINUE, block_set, true },
        { "TEXT", s_get_TEXT, block_set, true },
        { "BINARY", s_get_BINARY, block_set, true },
        { "CLOSE", s_get_CLOSE, block_set, true },
        { "PING", s_get_PING, block_set, true },
        { "PONG", s_get_PONG, block_set, true },
        { "CONNECTING", s_get_CONNECTING, block_set, true },
        { "OPEN", s_get_OPEN, block_set, true },
        { "CLOSING", s_get_CLOSING, block_set, true },
        { "CLOSED", s_get_CLOSED, block_set, true }
    };

    static ClassData s_cd = {
        "ws", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ws_base::s_get_CONTINUE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CONTINUE;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_TEXT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _TEXT;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_BINARY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _BINARY;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_CLOSE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CLOSE;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_PING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _PING;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_PONG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _PONG;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_CONNECTING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CONNECTING;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_OPEN(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _OPEN;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_CLOSING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CLOSING;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_get_CLOSED(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _CLOSED;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void ws_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty()) {
        acb_connect(v0, v1, cb);
        hr = CALL_RETURN_UNDEFINED;
    } else
        hr = ac_connect(v0, v1, vr);

    METHOD_RETURN();
}

inline void ws_base::s_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = upgrade(v0, vr);

    METHOD_RETURN();
}
}

#endif
