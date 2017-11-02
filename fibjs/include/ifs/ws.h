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
class WebSocket_base;
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
    static result_t upgrade(v8::Local<v8::Function> accept, obj_ptr<Handler_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "WebSocketMessage.h"
#include "WebSocket.h"
#include "Handler.h"

namespace fibjs {
inline ClassInfo& ws_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "upgrade", s_upgrade, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "Message", WebSocketMessage_base::class_info },
        { "Socket", WebSocket_base::class_info }
    };

    static ClassData::ClassConst s_const[] = {
        { "CONTINUE", _CONTINUE },
        { "TEXT", _TEXT },
        { "BINARY", _BINARY },
        { "CLOSE", _CLOSE },
        { "PING", _PING },
        { "PONG", _PONG },
        { "CONNECTING", _CONNECTING },
        { "OPEN", _OPEN },
        { "CLOSING", _CLOSING },
        { "CLOSED", _CLOSED }
    };

    static ClassData s_cd = {
        "ws", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void ws_base::s_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_NAME("ws.upgrade");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = upgrade(v0, vr);

    METHOD_RETURN();
}
}

#endif
