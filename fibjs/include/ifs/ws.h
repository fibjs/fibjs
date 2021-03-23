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

class WebSocketMessage_base;
class WebSocket_base;
class Handler_base;

class ws_base : public object_base {
    DECLARE_CLASS(ws_base);

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
    // ws_base
    static result_t upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);
    static result_t upgrade(v8::Local<v8::Object> opts, v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/WebSocketMessage.h"
#include "ifs/WebSocket.h"
#include "ifs/Handler.h"

namespace fibjs {
inline ClassInfo& ws_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "upgrade", s_static_upgrade, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Message", WebSocketMessage_base::class_info },
        { "Socket", WebSocket_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "CONTINUE", C_CONTINUE },
        { "TEXT", C_TEXT },
        { "BINARY", C_BINARY },
        { "CLOSE", C_CLOSE },
        { "PING", C_PING },
        { "PONG", C_PONG },
        { "CONNECTING", C_CONNECTING },
        { "OPEN", C_OPEN },
        { "CLOSING", C_CLOSING },
        { "CLOSED", C_CLOSED }
    };

    static ClassData s_cd = {
        "ws", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ws_base::s_static_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_NAME("ws.upgrade");
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
