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

class WebSocket_base : public EventEmitter_base {
    DECLARE_CLASS(WebSocket_base);

public:
    // WebSocket_base
    static result_t _new(exlib::string url, exlib::string protocol, exlib::string origin, obj_ptr<WebSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<WebSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
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
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& WebSocket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "send", s_send, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
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

template <typename T>
void WebSocket_base::__new(const T& args)
{
    obj_ptr<WebSocket_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(exlib::string, 2, "");

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
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

    hr = pInst->send(v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onopen(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onopen(v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onmessage(v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onclose(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onclose(v0);

    METHOD_VOID();
}

inline void WebSocket_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onerror(vr);

    METHOD_RETURN();
}

inline void WebSocket_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onerror(v0);

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
}
