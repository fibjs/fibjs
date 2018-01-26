/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _WebSocket_base_H_
#define _WebSocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class Buffer_base;

class WebSocket_base : public EventEmitter_base {
    DECLARE_CLASS(WebSocket_base);

public:
    // WebSocket_base
    static result_t _new(exlib::string url, exlib::string protocol, exlib::string origin, obj_ptr<WebSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_url(exlib::string& retVal) = 0;
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t get_origin(exlib::string& retVal) = 0;
    virtual result_t get_readyState(int32_t& retVal) = 0;
    virtual result_t close(int32_t code, exlib::string reason) = 0;
    virtual result_t send(exlib::string data) = 0;
    virtual result_t send(Buffer_base* data) = 0;
    virtual result_t get_onopen(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onopen(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclose(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclose(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onerror(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onerror(v8::Local<v8::Function> newVal) = 0;
    virtual result_t ref(obj_ptr<WebSocket_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<WebSocket_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_url(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_protocol(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_origin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_readyState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onopen(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onopen(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmessage(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onclose(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onclose(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onerror(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onerror(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& WebSocket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "close", s_close, false },
        { "send", s_send, false },
        { "ref", s_ref, false },
        { "unref", s_unref, false }
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

    static ClassData s_cd = {
        "WebSocket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebSocket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void WebSocket_base::__new(const T& args)
{
    obj_ptr<WebSocket_base> vr;

    METHOD_NAME("new WebSocket()");
    CONSTRUCT_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(exlib::string, 2, "");

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void WebSocket_base::s_get_url(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("WebSocket.url");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_url(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_get_protocol(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("WebSocket.protocol");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_protocol(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_get_origin(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("WebSocket.origin");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_origin(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_get_readyState(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("WebSocket.readyState");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_readyState(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebSocket.close");
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
    METHOD_NAME("WebSocket.send");
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->send(v0);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->send(v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onopen(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebSocket.onopen");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_onopen(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onopen(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebSocket.onopen");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onopen(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebSocket_base::s_get_onmessage(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebSocket.onmessage");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onmessage(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebSocket.onmessage");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onmessage(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebSocket_base::s_get_onclose(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebSocket.onclose");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_onclose(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onclose(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebSocket.onclose");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onclose(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebSocket_base::s_get_onerror(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebSocket.onerror");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_onerror(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onerror(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebSocket.onerror");
    METHOD_INSTANCE(WebSocket_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onerror(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebSocket_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebSocket_base> vr;

    METHOD_NAME("WebSocket.ref");
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebSocket_base> vr;

    METHOD_NAME("WebSocket.unref");
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}
}

#endif
