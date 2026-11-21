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
class WindowBounds;
class Menu_base;
class Buffer_base;

class WebView_base : public EventEmitter_base {
    DECLARE_CLASS(WebView_base);
    EVENT_SUPPORT();

public:
    class GetBoundsType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("x"), GetReturnValue(isolate, x)).Check();
            retVal->Set(context, isolate->NewString("y"), GetReturnValue(isolate, y)).Check();
            retVal->Set(context, isolate->NewString("width"), GetReturnValue(isolate, width)).Check();
            retVal->Set(context, isolate->NewString("height"), GetReturnValue(isolate, height)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, x));
            args.push_back(GetReturnValue(isolate, y));
            args.push_back(GetReturnValue(isolate, width));
            args.push_back(GetReturnValue(isolate, height));
        }

    public:
        double x;
        double y;
        double width;
        double height;
    };

public:
    // WebView_base
    virtual result_t loadUrl(exlib::string url) = 0;
    virtual result_t loadFile(exlib::string file) = 0;
    virtual result_t getUrl(exlib::string& retVal) = 0;
    virtual result_t setHtml(exlib::string html) = 0;
    virtual result_t getHtml(exlib::string& retVal) = 0;
    virtual result_t get_loading(bool& retVal) = 0;
    virtual result_t waitForLoad(exlib::string url) = 0;
    virtual result_t reload() = 0;
    virtual result_t goBack(bool& retVal) = 0;
    virtual result_t goForward(bool& retVal) = 0;
    virtual result_t eval(exlib::string code, Variant& retVal) = 0;
    virtual result_t setTitle(exlib::string title) = 0;
    virtual result_t getTitle(exlib::string& retVal) = 0;
    virtual result_t isVisible(bool& retVal) = 0;
    virtual result_t show() = 0;
    virtual result_t hide() = 0;
    virtual result_t setSize(int32_t width, int32_t height) = 0;
    virtual result_t getSize(std::vector<double>& retVal) = 0;
    virtual result_t setPosition(int32_t left, int32_t top) = 0;
    virtual result_t getPosition(std::vector<double>& retVal) = 0;
    virtual result_t isActived(bool& retVal) = 0;
    virtual result_t active() = 0;
    virtual result_t get_focused(bool& retVal) = 0;
    virtual result_t get_state(exlib::string& retVal) = 0;
    virtual result_t set_state(exlib::string newVal) = 0;
    virtual result_t getBounds(obj_ptr<GetBoundsType>& retVal) = 0;
    virtual result_t setBounds(WindowBounds* bounds) = 0;
    virtual result_t focus(bool& retVal) = 0;
    virtual result_t waitForClose() = 0;
    virtual result_t getMenu(obj_ptr<Menu_base>& retVal) = 0;
    virtual result_t takeScreenshot(bool fullPage, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t close(bool& retVal) = 0;
    virtual result_t destroy() = 0;
    virtual result_t postMessage(exlib::string msg) = 0;
    virtual result_t ref(obj_ptr<WebView_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<WebView_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<WebView_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getHtml(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_loading(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_waitForLoad(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_get_focused(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_state(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_state(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getBounds(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setBounds(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_focus(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_waitForClose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_takeScreenshot(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onloading(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onloading(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onload(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onload_failed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onload_failed(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_get_onclose_requested(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose_requested(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onshow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onshow(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onhide(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onhide(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onminimize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onminimize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmaximize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmaximize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onrestore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onrestore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onenter_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onenter_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onleave_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onleave_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/WindowBounds.h"
#include "ifs/Menu.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& WebView_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "loadUrl", s_loadUrl, false, ClassData::ASYNC_SYNC },
        { "loadFile", s_loadFile, false, ClassData::ASYNC_SYNC },
        { "getUrl", s_getUrl, false, ClassData::ASYNC_SYNC },
        { "setHtml", s_setHtml, false, ClassData::ASYNC_SYNC },
        { "getHtml", s_getHtml, false, ClassData::ASYNC_SYNC },
        { "waitForLoad", s_waitForLoad, false, ClassData::ASYNC_SYNC },
        { "reload", s_reload, false, ClassData::ASYNC_SYNC },
        { "goBack", s_goBack, false, ClassData::ASYNC_SYNC },
        { "goForward", s_goForward, false, ClassData::ASYNC_SYNC },
        { "eval", s_eval, false, ClassData::ASYNC_SYNC },
        { "setTitle", s_setTitle, false, ClassData::ASYNC_SYNC },
        { "getTitle", s_getTitle, false, ClassData::ASYNC_SYNC },
        { "isVisible", s_isVisible, false, ClassData::ASYNC_SYNC },
        { "show", s_show, false, ClassData::ASYNC_SYNC },
        { "hide", s_hide, false, ClassData::ASYNC_SYNC },
        { "setSize", s_setSize, false, ClassData::ASYNC_SYNC },
        { "getSize", s_getSize, false, ClassData::ASYNC_SYNC },
        { "setPosition", s_setPosition, false, ClassData::ASYNC_SYNC },
        { "getPosition", s_getPosition, false, ClassData::ASYNC_SYNC },
        { "isActived", s_isActived, false, ClassData::ASYNC_SYNC },
        { "active", s_active, false, ClassData::ASYNC_SYNC },
        { "getBounds", s_getBounds, false, ClassData::ASYNC_SYNC },
        { "setBounds", s_setBounds, false, ClassData::ASYNC_SYNC },
        { "focus", s_focus, false, ClassData::ASYNC_SYNC },
        { "waitForClose", s_waitForClose, false, ClassData::ASYNC_SYNC },
        { "getMenu", s_getMenu, false, ClassData::ASYNC_SYNC },
        { "takeScreenshot", s_takeScreenshot, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "destroy", s_destroy, false, ClassData::ASYNC_SYNC },
        { "postMessage", s_postMessage, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "loading", s_get_loading, block_set, false },
        { "focused", s_get_focused, block_set, false },
        { "state", s_get_state, s_set_state, false },
        { "onloading", s_get_onloading, s_set_onloading, false },
        { "onload", s_get_onload, s_set_onload, false },
        { "onload-failed", s_get_onload_failed, s_set_onload_failed, false },
        { "onmove", s_get_onmove, s_set_onmove, false },
        { "onresize", s_get_onresize, s_set_onresize, false },
        { "onfocus", s_get_onfocus, s_set_onfocus, false },
        { "onblur", s_get_onblur, s_set_onblur, false },
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onclose-requested", s_get_onclose_requested, s_set_onclose_requested, false },
        { "onshow", s_get_onshow, s_set_onshow, false },
        { "onhide", s_get_onhide, s_set_onhide, false },
        { "onminimize", s_get_onminimize, s_set_onminimize, false },
        { "onmaximize", s_get_onmaximize, s_set_onmaximize, false },
        { "onrestore", s_get_onrestore, s_set_onrestore, false },
        { "onenter-fullscreen", s_get_onenter_fullscreen, s_set_onenter_fullscreen, false },
        { "onleave-fullscreen", s_get_onleave_fullscreen, s_set_onleave_fullscreen, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false }
    };

    static ClassData s_cd = {
        "WebView", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebView_base::s_loadUrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "url");

    ARG_NAMED(exlib::string, 0, "url");

    hr = pInst->loadUrl(v0);

    METHOD_VOID();
}

inline void WebView_base::s_loadFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "file");

    ARG_NAMED(exlib::string, 0, "file");

    hr = pInst->loadFile(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getUrl(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getUrl(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "html");

    ARG_NAMED(exlib::string, 0, "html");

    hr = pInst->setHtml(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getHtml(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getHtml(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_get_loading(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_loading(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_waitForLoad(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(exlib::string, 0, "", "url");

    hr = pInst->waitForLoad(v0);

    METHOD_VOID();
}

inline void WebView_base::s_reload(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->reload();

    METHOD_VOID();
}

inline void WebView_base::s_goBack(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->goBack(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_goForward(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->goForward(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_eval(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "code");

    ARG_NAMED(exlib::string, 0, "code");

    hr = pInst->eval(v0, vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setTitle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "title");

    ARG_NAMED(exlib::string, 0, "title");

    hr = pInst->setTitle(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getTitle(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getTitle(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_isVisible(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isVisible(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_show(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->show();

    METHOD_VOID();
}

inline void WebView_base::s_hide(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->hide();

    METHOD_VOID();
}

inline void WebView_base::s_setSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "height");

    ARG_NAMED(int32_t, 0, "width");
    ARG_NAMED(int32_t, 1, "height");

    hr = pInst->setSize(v0, v1);

    METHOD_VOID();
}

inline void WebView_base::s_getSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<double> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getSize(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setPosition(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 2, "top");

    ARG_NAMED(int32_t, 0, "left");
    ARG_NAMED(int32_t, 1, "top");

    hr = pInst->setPosition(v0, v1);

    METHOD_VOID();
}

inline void WebView_base::s_getPosition(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<double> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getPosition(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_isActived(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->isActived(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_active(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->active();

    METHOD_VOID();
}

inline void WebView_base::s_get_focused(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_focused(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_get_state(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_state(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_state(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(exlib::string, 0, "state");

    hr = pInst->set_state(v0);

    METHOD_VOID();
}

inline void WebView_base::s_getBounds(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<GetBoundsType> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getBounds(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_setBounds(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "bounds");

    ARG_NAMED(obj_ptr<WindowBounds>, 0, "bounds");

    hr = pInst->setBounds((WindowBounds*)v0);

    METHOD_VOID();
}

inline void WebView_base::s_focus(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->focus(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_waitForClose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->waitForClose();

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

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(bool, 0, false, "fullPage");

    hr = pInst->takeScreenshot(v0, vr);

    METHOD_RETURN();
}

inline void WebView_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->destroy();

    METHOD_VOID();
}

inline void WebView_base::s_postMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "msg");

    ARG_NAMED(exlib::string, 0, "msg");

    hr = pInst->postMessage(v0);

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onloading");

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onload");

    hr = pInst->setListener("load", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onload_failed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("load-failed", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onload_failed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onload-failed");

    hr = pInst->setListener("load-failed", v0);

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onmove");

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onresize");

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onfocus");

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onblur");

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onclose");

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onclose_requested(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close-requested", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onclose_requested(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onclose-requested");

    hr = pInst->setListener("close-requested", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onshow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("show", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onshow(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onshow");

    hr = pInst->setListener("show", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onhide(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("hide", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onhide(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onhide");

    hr = pInst->setListener("hide", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onminimize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("minimize", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onminimize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onminimize");

    hr = pInst->setListener("minimize", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onmaximize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("maximize", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onmaximize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onmaximize");

    hr = pInst->setListener("maximize", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onrestore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("restore", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onrestore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onrestore");

    hr = pInst->setListener("restore", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onenter_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("enter-fullscreen", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onenter_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onenter-fullscreen");

    hr = pInst->setListener("enter-fullscreen", v0);

    METHOD_VOID();
}

inline void WebView_base::s_get_onleave_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("leave-fullscreen", vr);

    METHOD_RETURN();
}

inline void WebView_base::s_set_onleave_fullscreen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onleave-fullscreen");

    hr = pInst->setListener("leave-fullscreen", v0);

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

    ARG_NAMED(v8::Local<v8::Function>, 0, "onmessage");

    hr = pInst->setListener("message", v0);

    METHOD_VOID();
}

inline void WebView_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void WebView_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_INSTANCE(WebView_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}
}
