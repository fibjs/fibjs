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
    EVENT_SUPPORT();

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t loadFile(exlib::string file, AsyncEvent* ac) = 0;
    virtual result_t getUrl(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t setHtml(exlib::string html, AsyncEvent* ac) = 0;
    virtual result_t getHtml(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t isReady(bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t waitFor(exlib::string url, AsyncEvent* ac) = 0;
    virtual result_t reload(AsyncEvent* ac) = 0;
    virtual result_t goBack(AsyncEvent* ac) = 0;
    virtual result_t goForward(AsyncEvent* ac) = 0;
    virtual result_t eval(exlib::string code, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t setTitle(exlib::string title, AsyncEvent* ac) = 0;
    virtual result_t getTitle(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t isVisible(bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t show(AsyncEvent* ac) = 0;
    virtual result_t hide(AsyncEvent* ac) = 0;
    virtual result_t setSize(int32_t width, int32_t height, AsyncEvent* ac) = 0;
    virtual result_t getSize(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t setPosition(int32_t left, int32_t top, AsyncEvent* ac) = 0;
    virtual result_t getPosition(obj_ptr<NArray>& retVal, AsyncEvent* ac) = 0;
    virtual result_t isActived(bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t active(AsyncEvent* ac) = 0;
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal) = 0;
    virtual result_t takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t postMessage(exlib::string msg, AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(Isolate* isolate, v8::Local<v8::Value> v, obj_ptr<WebView_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isReady(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_waitFor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_reload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_eval(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setTitle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getTitle(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isVisible(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_show(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hide(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setPosition(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPosition(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_isActived(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_active(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_takeScreenshot(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    ASYNC_MEMBER1(WebView_base, loadUrl, exlib::string);
    ASYNC_MEMBER1(WebView_base, loadFile, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getUrl, exlib::string);
    ASYNC_MEMBER1(WebView_base, setHtml, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getHtml, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, isReady, bool);
    ASYNC_MEMBER1(WebView_base, waitFor, exlib::string);
    ASYNC_MEMBER0(WebView_base, reload);
    ASYNC_MEMBER0(WebView_base, goBack);
    ASYNC_MEMBER0(WebView_base, goForward);
    ASYNC_MEMBERVALUE2(WebView_base, eval, exlib::string, Variant);
    ASYNC_MEMBER1(WebView_base, setTitle, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, getTitle, exlib::string);
    ASYNC_MEMBERVALUE1(WebView_base, isVisible, bool);
    ASYNC_MEMBER0(WebView_base, show);
    ASYNC_MEMBER0(WebView_base, hide);
    ASYNC_MEMBER2(WebView_base, setSize, int32_t, int32_t);
    ASYNC_MEMBERVALUE1(WebView_base, getSize, obj_ptr<NArray>);
    ASYNC_MEMBER2(WebView_base, setPosition, int32_t, int32_t);
    ASYNC_MEMBERVALUE1(WebView_base, getPosition, obj_ptr<NArray>);
    ASYNC_MEMBERVALUE1(WebView_base, isActived, bool);
    ASYNC_MEMBER0(WebView_base, active);
    ASYNC_MEMBERVALUE2(WebView_base, takeScreenshot, bool, obj_ptr<Buffer_base>);
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
        { "loadUrl", s_loadUrl, false, ClassData::ASYNC_ASYNC },
        { "loadFile", s_loadFile, false, ClassData::ASYNC_ASYNC },
        { "getUrl", s_getUrl, false, ClassData::ASYNC_ASYNC },
        { "setHtml", s_setHtml, false, ClassData::ASYNC_ASYNC },
        { "getHtml", s_getHtml, false, ClassData::ASYNC_ASYNC },
        { "isReady", s_isReady, false, ClassData::ASYNC_ASYNC },
        { "waitFor", s_waitFor, false, ClassData::ASYNC_ASYNC },
        { "reload", s_reload, false, ClassData::ASYNC_ASYNC },
        { "goBack", s_goBack, false, ClassData::ASYNC_ASYNC },
        { "goForward", s_goForward, false, ClassData::ASYNC_ASYNC },
        { "eval", s_eval, false, ClassData::ASYNC_ASYNC },
        { "setTitle", s_setTitle, false, ClassData::ASYNC_ASYNC },
        { "getTitle", s_getTitle, false, ClassData::ASYNC_ASYNC },
        { "isVisible", s_isVisible, false, ClassData::ASYNC_ASYNC },
        { "show", s_show, false, ClassData::ASYNC_ASYNC },
        { "hide", s_hide, false, ClassData::ASYNC_ASYNC },
        { "setSize", s_setSize, false, ClassData::ASYNC_ASYNC },
        { "getSize", s_getSize, false, ClassData::ASYNC_ASYNC },
        { "setPosition", s_setPosition, false, ClassData::ASYNC_ASYNC },
        { "getPosition", s_getPosition, false, ClassData::ASYNC_ASYNC },
        { "isActived", s_isActived, false, ClassData::ASYNC_ASYNC },
        { "active", s_active, false, ClassData::ASYNC_ASYNC },
        { "getMenu", s_getMenu, false, ClassData::ASYNC_SYNC },
        { "takeScreenshot", s_takeScreenshot, false, ClassData::ASYNC_ASYNC },
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

inline void WebView_base::s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.loadUrl");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_loadUrl(v0, cb, args);
    else
        hr = pInst->ac_loadUrl(v0);

    METHOD_VOID();
}

inline void WebView_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.loadFile");

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
    ASYNC_METHOD_ENTER("WebView.getUrl");

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
    ASYNC_METHOD_ENTER("WebView.setHtml");

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
    ASYNC_METHOD_ENTER("WebView.getHtml");

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
    ASYNC_METHOD_ENTER("WebView.isReady");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_isReady(cb, args);
    else
        hr = pInst->ac_isReady(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_waitFor(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.waitFor");

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_waitFor(v0, cb, args);
    else
        hr = pInst->ac_waitFor(v0);

    METHOD_VOID();
}

inline void WebView_base::s_reload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.reload");

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
    ASYNC_METHOD_ENTER("WebView.goBack");

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
    ASYNC_METHOD_ENTER("WebView.goForward");

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
    ASYNC_METHOD_ENTER("WebView.eval");

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
    ASYNC_METHOD_ENTER("WebView.setTitle");

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
    ASYNC_METHOD_ENTER("WebView.getTitle");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_getTitle(cb, args);
    else
        hr = pInst->ac_getTitle(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_isVisible(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.isVisible");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_isVisible(cb, args);
    else
        hr = pInst->ac_isVisible(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_show(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.show");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_show(cb, args);
    else
        hr = pInst->ac_show();

    METHOD_VOID();
}

inline void WebView_base::s_hide(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.hide");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_hide(cb, args);
    else
        hr = pInst->ac_hide();

    METHOD_VOID();
}

inline void WebView_base::s_setSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.setSize");

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
    ASYNC_METHOD_ENTER("WebView.getSize");

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
    ASYNC_METHOD_ENTER("WebView.setPosition");

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
    ASYNC_METHOD_ENTER("WebView.getPosition");

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
    ASYNC_METHOD_ENTER("WebView.isActived");

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
    ASYNC_METHOD_ENTER("WebView.active");

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

inline void WebView_base::s_takeScreenshot(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.takeScreenshot");

    METHOD_OVER(1, 0);

    OPT_ARG(bool, 0, false);

    if (!cb.IsEmpty())
        hr = pInst->acb_takeScreenshot(v0, cb, args);
    else
        hr = pInst->ac_takeScreenshot(v0, vr);

    METHOD_RETURN();
}

inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(WebView_base);
    ASYNC_METHOD_ENTER("WebView.close");

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
    ASYNC_METHOD_ENTER("WebView.postMessage");

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

    hr = pInst->getListener("loading", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onloading(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("loading", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("load", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("load", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onmove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("move", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("move", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onresize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("resize", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onresize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("resize", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onfocus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("focus", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onfocus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("focus", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onblur(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("blur", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onblur(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("blur", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("message", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("message", v0);

    METHOD_VOID();
}
}
