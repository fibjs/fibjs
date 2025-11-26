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
class HttpResponse_base;

class EventSource_base : public EventEmitter_base {
    DECLARE_CLASS(EventSource_base);
    EVENT_SUPPORT();

public:
    // EventSource_base
    static result_t _new(exlib::string url, v8::Local<v8::Object> options, obj_ptr<EventSource_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t send(exlib::string data, v8::Local<v8::Object> options, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t get_readyState(int32_t& retVal) = 0;
    virtual result_t get_url(exlib::string& retVal) = 0;
    virtual result_t get_withCredentials(bool& retVal) = 0;
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<EventSource_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_readyState(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_withCredentials(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_response(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER0(EventSource_base, close);
    ASYNC_MEMBERVALUE3(EventSource_base, send, exlib::string, v8::Local<v8::Object>, int32_t);
};
}

#include "ifs/HttpResponse.h"

namespace fibjs {
inline ClassInfo& EventSource_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "send", s_send, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "readyState", s_get_readyState, block_set, false },
        { "url", s_get_url, block_set, false },
        { "withCredentials", s_get_withCredentials, block_set, false },
        { "response", s_get_response, block_set, false },
        { "onopen", s_get_onopen, s_set_onopen, false },
        { "onerror", s_get_onerror, s_set_onerror, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "onclose", s_get_onclose, s_set_onclose, false }
    };

    static ClassData s_cd = {
        "EventSource", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void EventSource_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void EventSource_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<EventSource_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t EventSource_base::load(v8::Local<v8::Value> v, obj_ptr<EventSource_base>& retVal)
{
    obj_ptr<EventSource_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    LOAD_RETURN();
}

inline void EventSource_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(EventSource_base);
    ASYNC_METHOD_ENTER("EventSource.close");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}

inline void EventSource_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(EventSource_base);
    ASYNC_METHOD_ENTER("EventSource.send");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_send(v0, v1, cb, args);
    else
        hr = pInst->ac_send(v0, v1, vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_get_readyState(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_readyState(vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_url(vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_get_withCredentials(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_withCredentials(vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_get_response(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_response(vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("open", vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("open", v0);

    METHOD_VOID();
}

inline void EventSource_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}

inline void EventSource_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("message", vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("message", v0);

    METHOD_VOID();
}

inline void EventSource_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close", vr);

    METHOD_RETURN();
}

inline void EventSource_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(EventSource_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}
}
