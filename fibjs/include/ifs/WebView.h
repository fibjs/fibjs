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

namespace fibjs {

class EventEmitter_base;

class WebView_base : public EventEmitter_base {
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac) = 0;
    virtual result_t print(int32_t mode, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t wait(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t get_visible(bool& retVal) = 0;
    virtual result_t set_visible(bool newVal) = 0;
    virtual result_t get_onload(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onload(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmove(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmove(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onresize(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onresize(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclosed(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclosed(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_visible(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_visible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onload(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onload(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmove(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmove(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onresize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onresize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onclosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onclosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_MEMBER1(WebView_base, setHtml, exlib::string);
    ASYNC_MEMBER1(WebView_base, print, int32_t);
    ASYNC_MEMBER0(WebView_base, close);
    ASYNC_MEMBER0(WebView_base, wait);
    ASYNC_MEMBER1(WebView_base, postMessage, exlib::string);
};
}

namespace fibjs {
inline ClassInfo& WebView_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setHtml", s_setHtml, false },
        { "setHtmlSync", s_setHtml, false },
        { "print", s_print, false },
        { "printSync", s_print, false },
        { "close", s_close, false },
        { "closeSync", s_close, false },
        { "wait", s_wait, false },
        { "waitSync", s_wait, false },
        { "postMessage", s_postMessage, false },
        { "postMessageSync", s_postMessage, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "visible", s_get_visible, s_set_visible, false },
        { "onload", s_get_onload, s_set_onload, false },
        { "onmove", s_get_onmove, s_set_onmove, false },
        { "onresize", s_get_onresize, s_set_onresize, false },
        { "onclosed", s_get_onclosed, s_set_onclosed, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false }
    };

    static ClassData s_cd = {
        "WebView", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &EventEmitter_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebView_base::s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.setHtml");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_setHtml(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_setHtml(v0);

    METHOD_VOID();
}

inline void WebView_base::s_print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.print");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_print(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_print(v0);

    METHOD_VOID();
}

inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.close");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_close(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_close();

    METHOD_VOID();
}

inline void WebView_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.wait");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_wait(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_wait();

    METHOD_VOID();
}

inline void WebView_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.postMessage");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_postMessage(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_postMessage(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_visible(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("WebView.visible");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_visible(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_visible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.visible");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_visible(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onload(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onload");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onload(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onload(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onload");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onload(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onmove(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onmove");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmove(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmove(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onmove");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onmove(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onresize(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onresize");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onresize(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onresize(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onresize");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onresize(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onclosed(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onclosed");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onclosed(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onclosed(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onclosed");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onclosed(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onmessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onmessage");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onmessage");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onmessage(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
