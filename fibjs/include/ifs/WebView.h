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
class Menu_base;

class WebView_base : public EventEmitter_base {
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t loadURL(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t loadFile(exlib::string file, AsyncEvent* ac) = 0;
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac) = 0;
    virtual result_t reload(AsyncEvent* ac) = 0;
    virtual result_t goBack(AsyncEvent* ac) = 0;
    virtual result_t goForward(AsyncEvent* ac) = 0;
    virtual result_t eval(exlib::string code, AsyncEvent* ac) = 0;
    virtual result_t setTitle(exlib::string title, AsyncEvent* ac) = 0;
    virtual result_t getTitle(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t get_onopen(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onopen(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmove(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmove(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onresize(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onresize(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onfocus(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onfocus(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onblur(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onblur(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onclose(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onclose(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onmessage(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onmessage(v8::Local<v8::Function> newVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_loadURL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setTitle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getTitle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onresize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onresize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onfocus(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onfocus(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onblur(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onblur(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(WebView_base, loadURL, exlib::string);
    ASYNC_MEMBER1(WebView_base, loadFile, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getUrl, exlib::string);
    ASYNC_MEMBER1(WebView_base, setHtml, exlib::string);
    ASYNC_MEMBER0(WebView_base, reload);
    ASYNC_MEMBER0(WebView_base, goBack);
    ASYNC_MEMBER0(WebView_base, goForward);
    ASYNC_MEMBER1(WebView_base, eval, exlib::string);
    ASYNC_MEMBER1(WebView_base, setTitle, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getTitle, exlib::string);
    ASYNC_MEMBER0(WebView_base, close);
    ASYNC_MEMBER1(WebView_base, postMessage, exlib::string);
};
}

#include "ifs/Menu.h"

namespace fibjs {
inline ClassInfo& WebView_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "loadURL", s_loadURL, false, ClassData::ASYNC_ASYNC },
        { "loadFile", s_loadFile, false, ClassData::ASYNC_ASYNC },
        { "getUrl", s_getUrl, false, ClassData::ASYNC_ASYNC },
        { "setHtml", s_setHtml, false, ClassData::ASYNC_ASYNC },
        { "reload", s_reload, false, ClassData::ASYNC_ASYNC },
        { "goBack", s_goBack, false, ClassData::ASYNC_ASYNC },
        { "goForward", s_goForward, false, ClassData::ASYNC_ASYNC },
        { "eval", s_eval, false, ClassData::ASYNC_ASYNC },
        { "setTitle", s_setTitle, false, ClassData::ASYNC_ASYNC },
        { "getTitle", s_getTitle, false, ClassData::ASYNC_ASYNC },
        { "getMenu", s_getMenu, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "postMessage", s_postMessage, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onopen", s_get_onopen, s_set_onopen, false },
        { "onmove", s_get_onmove, s_set_onmove, false },
        { "onresize", s_get_onresize, s_set_onresize, false },
        { "onfocus", s_get_onfocus, s_set_onfocus, false },
        { "onblur", s_get_onblur, s_set_onblur, false },
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false }
    };

    static ClassData s_cd = {
        "WebView", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebView_base::s_loadURL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_loadURL(v0, cb, args);
    else
        hr = pInst->ac_loadURL(v0);

    METHOD_VOID();
}

inline void WebView_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_loadFile(v0, cb, args);
    else
        hr = pInst->ac_loadFile(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getUrl(cb, args);
    else
        hr = pInst->ac_getUrl(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

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
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_reload(cb, args);
    else
        hr = pInst->ac_reload();

    METHOD_VOID();
}

inline void WebView_base::s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_goBack(cb, args);
    else
        hr = pInst->ac_goBack();

    METHOD_VOID();
}

inline void WebView_base::s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_goForward(cb, args);
    else
        hr = pInst->ac_goForward();

    METHOD_VOID();
}

inline void WebView_base::s_eval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_eval(v0, cb, args);
    else
        hr = pInst->ac_eval(v0);

    METHOD_VOID();
}

inline void WebView_base::s_setTitle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_setTitle(v0, cb, args);
    else
        hr = pInst->ac_setTitle(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getTitle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getTitle(cb, args);
    else
        hr = pInst->ac_getTitle(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Menu_base> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getMenu(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}

inline void WebView_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_postMessage(v0, cb, args);
    else
        hr = pInst->ac_postMessage(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onopen(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onopen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onopen(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onmove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onmove(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onmove(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onresize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onresize(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onresize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onresize(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onfocus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onfocus(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onfocus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onfocus(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onblur(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onblur(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onblur(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onblur(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onclose(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onclose(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onmessage(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onmessage(v0);

    METHOD_VOID();
}
}
