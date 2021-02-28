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
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;

class WebView_base : public EventEmitter_base {
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac) = 0;
    virtual result_t reload(AsyncEvent* ac) = 0;
    virtual result_t goBack(AsyncEvent* ac) = 0;
    virtual result_t goForward(AsyncEvent* ac) = 0;
    virtual result_t print(int32_t mode, AsyncEvent* ac) = 0;
    virtual result_t printToPDF(exlib::string file, AsyncEvent* ac) = 0;
    virtual result_t executeJavaScript(exlib::string code, AsyncEvent* ac) = 0;
    virtual result_t executeDevToolsMethod(exlib::string method, v8::Local<v8::Object> params, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t get_dev(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t get_onopen(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onopen(v8::Local<v8::Function> newVal) = 0;
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
    static void s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_print(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_printToPDF(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_executeJavaScript(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_executeDevToolsMethod(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_dev(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_onopen(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onopen(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onload(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onload(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmove(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmove(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onresize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onresize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onclosed(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onclosed(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_onmessage(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_MEMBER1(WebView_base, loadUrl, exlib::string);
    ASYNC_MEMBER1(WebView_base, setHtml, exlib::string);
    ASYNC_MEMBER0(WebView_base, reload);
    ASYNC_MEMBER0(WebView_base, goBack);
    ASYNC_MEMBER0(WebView_base, goForward);
    ASYNC_MEMBER1(WebView_base, print, int32_t);
    ASYNC_MEMBER1(WebView_base, printToPDF, exlib::string);
    ASYNC_MEMBER1(WebView_base, executeJavaScript, exlib::string);
    ASYNC_MEMBERVALUE3(WebView_base, executeDevToolsMethod, exlib::string, v8::Local<v8::Object>, Variant);
    ASYNC_MEMBER0(WebView_base, close);
    ASYNC_MEMBER1(WebView_base, postMessage, exlib::string);
};
}

namespace fibjs {
inline ClassInfo& WebView_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "loadUrl", s_loadUrl, false },
        { "loadUrlSync", s_loadUrl, false },
        { "setHtml", s_setHtml, false },
        { "setHtmlSync", s_setHtml, false },
        { "reload", s_reload, false },
        { "reloadSync", s_reload, false },
        { "goBack", s_goBack, false },
        { "goBackSync", s_goBack, false },
        { "goForward", s_goForward, false },
        { "goForwardSync", s_goForward, false },
        { "print", s_print, false },
        { "printSync", s_print, false },
        { "printToPDF", s_printToPDF, false },
        { "printToPDFSync", s_printToPDF, false },
        { "executeJavaScript", s_executeJavaScript, false },
        { "executeJavaScriptSync", s_executeJavaScript, false },
        { "executeDevToolsMethod", s_executeDevToolsMethod, false },
        { "executeDevToolsMethodSync", s_executeDevToolsMethod, false },
        { "close", s_close, false },
        { "closeSync", s_close, false },
        { "postMessage", s_postMessage, false },
        { "postMessageSync", s_postMessage, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "dev", s_get_dev, block_set, false },
        { "onopen", s_get_onopen, s_set_onopen, false },
        { "onload", s_get_onload, s_set_onload, false },
        { "onmove", s_get_onmove, s_set_onmove, false },
        { "onresize", s_get_onresize, s_set_onresize, false },
        { "onclosed", s_get_onclosed, s_set_onclosed, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false }
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
    METHOD_NAME("WebView.loadUrl");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_loadUrl(v0, cb, args);
    else
        hr = pInst->ac_loadUrl(v0);

    METHOD_VOID();
}

inline void WebView_base::s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.setHtml");
    METHOD_INSTANCE(WebView_base);
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
    METHOD_NAME("WebView.reload");
    METHOD_INSTANCE(WebView_base);
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
    METHOD_NAME("WebView.goBack");
    METHOD_INSTANCE(WebView_base);
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
    METHOD_NAME("WebView.goForward");
    METHOD_INSTANCE(WebView_base);
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
    METHOD_NAME("WebView.print");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_print(v0, cb, args);
    else
        hr = pInst->ac_print(v0);

    METHOD_VOID();
}

inline void WebView_base::s_printToPDF(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.printToPDF");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_printToPDF(v0, cb, args);
    else
        hr = pInst->ac_printToPDF(v0);

    METHOD_VOID();
}

inline void WebView_base::s_executeJavaScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.executeJavaScript");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_executeJavaScript(v0, cb, args);
    else
        hr = pInst->ac_executeJavaScript(v0);

    METHOD_VOID();
}

inline void WebView_base::s_executeDevToolsMethod(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_NAME("WebView.executeDevToolsMethod");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_executeDevToolsMethod(v0, v1, cb, args);
    else
        hr = pInst->ac_executeDevToolsMethod(v0, v1, vr);

    METHOD_RETURN();
}

inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("WebView.close");
    METHOD_INSTANCE(WebView_base);
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
    METHOD_NAME("WebView.postMessage");
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_postMessage(v0, cb, args);
    else
        hr = pInst->ac_postMessage(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_dev(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_NAME("WebView.dev");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_dev(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_get_onopen(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onopen");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onopen(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onopen(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onopen");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onopen(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onload(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onload");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onload(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onload(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onload");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onload(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onmove(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onmove");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmove(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmove(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onmove");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onmove(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onresize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onresize");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onresize(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onresize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onresize");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onresize(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onclosed(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onclosed");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onclosed(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onclosed(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("WebView.onclosed");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(v8::Local<v8::Function>);

    hr = pInst->set_onclosed(v0);

    PROPERTY_SET_LEAVE();
}

inline void WebView_base::s_get_onmessage(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_NAME("WebView.onmessage");
    METHOD_INSTANCE(WebView_base);
    PROPERTY_ENTER();

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmessage(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
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
