/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _websocket_base_H_
#define _websocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class WebSocketMessage_base;
class WebSocketHandler_base;
class Stream_base;

class websocket_base : public object_base
{
    DECLARE_CLASS(websocket_base);

public:
    enum{
        _CONTINUE = 0,
        _TEXT = 1,
        _BINARY = 2,
        _CLOSE = 8,
        _PING = 9,
        _PONG = 10
    };

public:
    // websocket_base
    static result_t connect(exlib::string url, exlib::string origin, obj_ptr<Stream_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_CONTINUE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_TEXT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_BINARY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_CLOSE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_PING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_PONG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(websocket_base, connect, exlib::string, exlib::string, obj_ptr<Stream_base>);
};

}

#include "WebSocketMessage.h"
#include "WebSocketHandler.h"
#include "Stream.h"

namespace fibjs
{
    inline ClassInfo& websocket_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"connect", s_connect, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Message", WebSocketMessage_base::class_info},
            {"Handler", WebSocketHandler_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"CONTINUE", s_get_CONTINUE, block_set, true},
            {"TEXT", s_get_TEXT, block_set, true},
            {"BINARY", s_get_BINARY, block_set, true},
            {"CLOSE", s_get_CLOSE, block_set, true},
            {"PING", s_get_PING, block_set, true},
            {"PONG", s_get_PONG, block_set, true}
        };

        static ClassData s_cd = 
        { 
            "websocket", s__new, NULL, 
            1, s_method, 2, s_object, 6, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void websocket_base::s_get_CONTINUE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _CONTINUE;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void websocket_base::s_get_TEXT(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _TEXT;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void websocket_base::s_get_BINARY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _BINARY;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void websocket_base::s_get_CLOSE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _CLOSE;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void websocket_base::s_get_PING(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _PING;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void websocket_base::s_get_PONG(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr = _PONG;
        PROPERTY_ENTER();
        METHOD_RETURN();
    }

    inline void websocket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Stream_base> vr;

        ASYNC_METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(exlib::string, 1, "");

        if(!cb.IsEmpty()) {
            acb_connect(v0, v1, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_connect(v0, v1, vr);

        METHOD_RETURN();
    }

}

#endif

