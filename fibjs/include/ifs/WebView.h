/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _WebView_base_H_
#define _WebView_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "EventEmitter.h"

namespace fibjs
{

class EventEmitter_base;

class WebView_base : public EventEmitter_base
{
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t wait(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t get_onmove(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmove(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onsize(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onsize(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclose(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclose(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmove(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_onmove(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_onsize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_onsize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_onclose(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_onclose(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_onmessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_onmessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);

public:
    ASYNC_MEMBER0(WebView_base, close);
    ASYNC_MEMBER0(WebView_base, wait);
    ASYNC_MEMBER1(WebView_base, postMessage, exlib::string);
};

}

namespace fibjs
{
    inline ClassInfo& WebView_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"close", s_close, false},
            {"wait", s_wait, false},
            {"postMessage", s_postMessage, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"onmove", s_get_onmove, s_set_onmove, false},
            {"onsize", s_get_onsize, s_set_onsize, false},
            {"onclose", s_get_onclose, s_set_onclose, false},
            {"onmessage", s_get_onmessage, s_set_onmessage, false}
        };

        static ClassData s_cd = 
        { 
            "WebView", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &EventEmitter_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_close(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_close();

        METHOD_VOID();
    }

    inline void WebView_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_wait(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_wait();

        METHOD_VOID();
    }

    inline void WebView_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_postMessage(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_postMessage(v0);

        METHOD_VOID();
    }

    inline void WebView_base::s_get_onmove(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();

        hr = pInst->get_onmove(vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_set_onmove(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_onmove(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void WebView_base::s_get_onsize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();

        hr = pInst->get_onsize(vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_set_onsize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_onsize(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void WebView_base::s_get_onclose(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();

        hr = pInst->get_onclose(vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_set_onclose(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_onclose(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void WebView_base::s_get_onmessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        v8::Local<v8::Function> vr;

        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();

        hr = pInst->get_onmessage(vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_set_onmessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        METHOD_INSTANCE(WebView_base);
        PROPERTY_ENTER();
        PROPERTY_VAL(v8::Local<v8::Function>);

        hr = pInst->set_onmessage(v0);

        PROPERTY_SET_LEAVE();
    }

}

#endif

