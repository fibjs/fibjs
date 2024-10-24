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
class Buffer_base;

class WebView_base : public EventEmitter_base {
    DECLARE_CLASS(WebView_base);

public:
    // WebView_base
    virtual result_t loadURL(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t loadFile(exlib::string file, AsyncEvent* ac) = 0;
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac) = 0;
    virtual result_t getHtml(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t isReady(bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t reload(AsyncEvent* ac) = 0;
    virtual result_t goBack(AsyncEvent* ac) = 0;
    virtual result_t goForward(AsyncEvent* ac) = 0;
    virtual result_t eval(exlib::string code, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t setTitle(exlib::string title, AsyncEvent* ac) = 0;
    virtual result_t getTitle(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setSize(int32_t width, int32_t height, AsyncEvent* ac) = 0;
    virtual result_t getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t setPosition(int32_t left, int32_t top, AsyncEvent* ac) = 0;
    virtual result_t getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t isActived(bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t active(AsyncEvent* ac) = 0;
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal) = 0;
    virtual result_t capturePage(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;
    virtual result_t get_onloading(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onloading(v8::Local<v8::Function> newVal) = 0;
    virtual result_t get_onload(v8::Local<v8::Function>& retVal) = 0;
    virtual result_t set_onload(v8::Local<v8::Function> newVal) = 0;
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
    static void s_getHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isReady(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setTitle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getTitle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setPosition(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPosition(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isActived(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_active(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_capturePage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onloading(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onloading(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onload(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    ASYNC_MEMBERVALUE1(WebView_base, getHtml, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, isReady, bool);
    ASYNC_MEMBER0(WebView_base, reload);
    ASYNC_MEMBER0(WebView_base, goBack);
    ASYNC_MEMBER0(WebView_base, goForward);
    ASYNC_MEMBERVALUE2(WebView_base, eval, exlib::string, Variant);
    ASYNC_MEMBER1(WebView_base, setTitle, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getTitle, exlib::string);
    ASYNC_MEMBER2(WebView_base, setSize, int32_t, int32_t);
    ASYNC_MEMBERVALUE1(WebView_base, getSize, obj_ptr<NArray>);
    ASYNC_MEMBER2(WebView_base, setPosition, int32_t, int32_t);
    ASYNC_MEMBERVALUE1(WebView_base, getPosition, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE1(WebView_base, isActived, bool);
    ASYNC_MEMBER0(WebView_base, active);
    ASYNC_MEMBERVALUE1(WebView_base, capturePage, obj_ptr<Buffer_base>);
    ASYNC_MEMBER0(WebView_base, close);
    ASYNC_MEMBER1(WebView_base, postMessage, exlib::string);
};
}

#include "ifs/Menu.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& WebView_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "loadURL", s_loadURL, false, ClassData::ASYNC_ASYNC },
        { "loadFile", s_loadFile, false, ClassData::ASYNC_ASYNC },
        { "getUrl", s_getUrl, false, ClassData::ASYNC_ASYNC },
        { "setHtml", s_setHtml, false, ClassData::ASYNC_ASYNC },
        { "getHtml", s_getHtml, false, ClassData::ASYNC_ASYNC },
        { "isReady", s_isReady, false, ClassData::ASYNC_ASYNC },
        { "reload", s_reload, false, ClassData::ASYNC_ASYNC },
        { "goBack", s_goBack, false, ClassData::ASYNC_ASYNC },
        { "goForward", s_goForward, false, ClassData::ASYNC_ASYNC },
        { "eval", s_eval, false, ClassData::ASYNC_ASYNC },
        { "setTitle", s_setTitle, false, ClassData::ASYNC_ASYNC },
        { "getTitle", s_getTitle, false, ClassData::ASYNC_ASYNC },
        { "setSize", s_setSize, false, ClassData::ASYNC_ASYNC },
        { "getSize", s_getSize, false, ClassData::ASYNC_ASYNC },
        { "setPosition", s_setPosition, false, ClassData::ASYNC_ASYNC },
        { "getPosition", s_getPosition, false, ClassData::ASYNC_ASYNC },
        { "isActived", s_isActived, false, ClassData::ASYNC_ASYNC },
        { "active", s_active, false, ClassData::ASYNC_ASYNC },
        { "getMenu", s_getMenu, false, ClassData::ASYNC_SYNC },
        { "capturePage", s_capturePage, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "postMessage", s_postMessage, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onloading", s_get_onloading, s_set_onloading, false },
        { "onload", s_get_onload, s_set_onload, false },
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

inline void WebView_base::s_getHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getHtml(cb, args);
    else
        hr = pInst->ac_getHtml(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_isReady(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_isReady(cb, args);
    else
        hr = pInst->ac_isReady(vr);

    METHOD_RETURN();
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
    Variant vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_eval(v0, cb, args);
    else
        hr = pInst->ac_eval(v0, vr);

    METHOD_RETURN();
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

inline void WebView_base::s_setSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_setSize(v0, v1, cb, args);
    else
        hr = pInst->ac_setSize(v0, v1);

    METHOD_VOID();
}

inline void WebView_base::s_getSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getSize(cb, args);
    else
        hr = pInst->ac_getSize(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setPosition(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_setPosition(v0, v1, cb, args);
    else
        hr = pInst->ac_setPosition(v0, v1);

    METHOD_VOID();
}

inline void WebView_base::s_getPosition(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getPosition(cb, args);
    else
        hr = pInst->ac_getPosition(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_isActived(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_isActived(cb, args);
    else
        hr = pInst->ac_isActived(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_active(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_active(cb, args);
    else
        hr = pInst->ac_active();

    METHOD_VOID();
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

inline void WebView_base::s_capturePage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_capturePage(cb, args);
    else
        hr = pInst->ac_capturePage(vr);

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

inline void WebView_base::s_get_onloading(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onloading(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onloading(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onloading(v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_onload(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->set_onload(v0);

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
