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

class WebView_base : public EventEmitter_base {
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac) = 0;
    virtual result_t reload(AsyncEvent* ac) = 0;
    virtual result_t goBack(AsyncEvent* ac) = 0;
    virtual result_t goForward(AsyncEvent* ac) = 0;
    virtual result_t print(int32_t mode, AsyncEvent* ac) = 0;
    virtual result_t executeJavaScript(exlib::string code, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t get_onopen(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onopen(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onload(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onload(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onaddress(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onaddress(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_ontitle(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_ontitle(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmove(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmove(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onresize(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onresize(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclosed(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclosed(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_ondownload(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_ondownload(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_executeJavaScript(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onopen(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onopen(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onload(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onload(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onaddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onaddress(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ontitle(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_ontitle(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmove(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmove(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onresize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onresize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onclosed(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onclosed(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmessage(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_ondownload(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_ondownload(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_MEMBER1(WebView_base, loadUrl, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getUrl, exlib::string);
    ASYNC_MEMBER1(WebView_base, setHtml, exlib::string);
    ASYNC_MEMBER0(WebView_base, reload);
    ASYNC_MEMBER0(WebView_base, goBack);
    ASYNC_MEMBER0(WebView_base, goForward);
    ASYNC_MEMBER1(WebView_base, print, int32_t);
    ASYNC_MEMBER1(WebView_base, executeJavaScript, exlib::string);
    ASYNC_MEMBER0(WebView_base, close);
    ASYNC_MEMBER1(WebView_base, postMessage, exlib::string);
};
}

namespace fibjs {
inline ClassInfo& WebView_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "loadUrl", s_loadUrl, false, true },
        { "loadUrlSync", s_loadUrl, false, false },
        { "getUrl", s_getUrl, false, true },
        { "getUrlSync", s_getUrl, false, false },
        { "setHtml", s_setHtml, false, true },
        { "setHtmlSync", s_setHtml, false, false },
        { "reload", s_reload, false, true },
        { "reloadSync", s_reload, false, false },
        { "goBack", s_goBack, false, true },
        { "goBackSync", s_goBack, false, false },
        { "goForward", s_goForward, false, true },
        { "goForwardSync", s_goForward, false, false },
        { "print", s_print, false, true },
        { "printSync", s_print, false, false },
        { "executeJavaScript", s_executeJavaScript, false, true },
        { "executeJavaScriptSync", s_executeJavaScript, false, false },
        { "close", s_close, false, true },
        { "closeSync", s_close, false, false },
        { "postMessage", s_postMessage, false, true },
        { "postMessageSync", s_postMessage, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onopen", s_get_onopen, s_set_onopen, false },
        { "onload", s_get_onload, s_set_onload, false },
        { "onaddress", s_get_onaddress, s_set_onaddress, false },
        { "ontitle", s_get_ontitle, s_set_ontitle, false },
        { "onmove", s_get_onmove, s_set_onmove, false },
        { "onresize", s_get_onresize, s_set_onresize, false },
        { "onclosed", s_get_onclosed, s_set_onclosed, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false },
        { "ondownload", s_get_ondownload, s_set_ondownload, false }
    };

    static ClassData s_cd = {
        "WebView", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebView_base::s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_loadUrl(v0, cb, args);
    else
        hr = pInst->ac_loadUrl(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getUrl(cb, args);
    else
        hr = pInst->ac_getUrl(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_setHtml(v0, cb, args);
    else
        hr = pInst->ac_setHtml(v0);

    METHOD_VOID();
}

inline void WebView_base::s_reload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_reload(cb, args);
    else
        hr = pInst->ac_reload();

    METHOD_VOID();
}

inline void WebView_base::s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_goBack(cb, args);
    else
        hr = pInst->ac_goBack();

    METHOD_VOID();
}

inline void WebView_base::s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_goForward(cb, args);
    else
        hr = pInst->ac_goForward();

    METHOD_VOID();
}

inline void WebView_base::s_print(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_print(v0, cb, args);
    else
        hr = pInst->ac_print(v0);

    METHOD_VOID();
}

inline void WebView_base::s_executeJavaScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_executeJavaScript(v0, cb, args);
    else
        hr = pInst->ac_executeJavaScript(v0);

    METHOD_VOID();
}

inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}

inline void WebView_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_postMessage(v0, cb, args);
    else
        hr = pInst->ac_postMessage(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onopen(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onopen(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onopen(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onopen(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onload(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onload(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onload(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onload(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onaddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onaddress(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onaddress(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onaddress(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_ontitle(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_ontitle(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_ontitle(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_ontitle(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onmove(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmove(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmove(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onmove(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onresize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onresize(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onresize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onresize(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onclosed(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onclosed(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onclosed(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onclosed(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onmessage(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmessage(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onmessage(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_ondownload(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_ondownload(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_ondownload(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_ondownload(v0);

    PROPERTY_SET_LEAVE();
}
}
