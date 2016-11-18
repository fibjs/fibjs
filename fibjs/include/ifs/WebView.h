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
#include "Trigger.h"

namespace fibjs
{

class Trigger_base;

class WebView_base : public Trigger_base
{
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t wait(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t onmove(v8::Local<v8::Function> func, int32_t& retVal) = 0;
    virtual result_t onsize(v8::Local<v8::Function> func, int32_t& retVal) = 0;
    virtual result_t onclose(v8::Local<v8::Function> func, int32_t& retVal) = 0;
    virtual result_t onmessage(v8::Local<v8::Function> func, int32_t& retVal) = 0;

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
    static void s_onmove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onsize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);

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
            {"postMessage", s_postMessage, false},
            {"onmove", s_onmove, false},
            {"onsize", s_onsize, false},
            {"onclose", s_onclose, false},
            {"onmessage", s_onmessage, false}
        };

        static ClassData s_cd = 
        { 
            "WebView", s__new, NULL, 
            7, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &Trigger_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(WebView_base);
        ASYNC_METHOD_ENTER(0, 0);

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
        ASYNC_METHOD_ENTER(0, 0);

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
        ASYNC_METHOD_ENTER(1, 1);

        ARG(exlib::string, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_postMessage(v0, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_postMessage(v0);

        METHOD_VOID();
    }

    inline void WebView_base::s_onmove(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onmove(v0, vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_onsize(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onsize(v0, vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onclose(v0, vr);

        METHOD_RETURN();
    }

    inline void WebView_base::s_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(WebView_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onmessage(v0, vr);

        METHOD_RETURN();
    }

}

#endif

