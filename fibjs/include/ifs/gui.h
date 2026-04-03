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

namespace fibjs {

class WebView_base;
class Menu_base;
class Tray_base;

class gui_base : public object_base {
    DECLARE_CLASS(gui_base);

public:
    // gui_base
    static result_t open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static result_t open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static result_t openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static result_t createMenu(std::vector<v8::Local<v8::Object>>& items, obj_ptr<Menu_base>& retVal);
    static result_t createTray(v8::Local<v8::Object> opt, obj_ptr<Tray_base>& retVal);
    static result_t alert(exlib::string message, AsyncEvent* ac);
    static result_t alert(exlib::string title, exlib::string message, AsyncEvent* ac);
    static result_t confirm(exlib::string message, bool& retVal, AsyncEvent* ac);
    static result_t confirm(exlib::string title, exlib::string message, bool& retVal, AsyncEvent* ac);
    static result_t input(exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac);
    static result_t input(exlib::string title, exlib::string message, bool password, exlib::string& retVal, AsyncEvent* ac);
    static result_t chooseFile(v8::Local<v8::Object> options, obj_ptr<NArray>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<gui_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createTray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_confirm(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_input(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chooseFile(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATIC1(gui_base, alert, exlib::string);
    ASYNC_STATIC2(gui_base, alert, exlib::string, exlib::string);
    ASYNC_STATICVALUE2(gui_base, confirm, exlib::string, bool);
    ASYNC_STATICVALUE3(gui_base, confirm, exlib::string, exlib::string, bool);
    ASYNC_STATICVALUE3(gui_base, input, exlib::string, bool, exlib::string);
    ASYNC_STATICVALUE4(gui_base, input, exlib::string, exlib::string, bool, exlib::string);
    ASYNC_STATICVALUE2(gui_base, chooseFile, v8::Local<v8::Object>, obj_ptr<NArray>);
};
}

#include "ifs/WebView.h"
#include "ifs/Menu.h"
#include "ifs/Tray.h"

namespace fibjs {
inline ClassInfo& gui_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "open", s_static_open, true, ClassData::ASYNC_SYNC },
        { "openFile", s_static_openFile, true, ClassData::ASYNC_SYNC },
        { "createMenu", s_static_createMenu, true, ClassData::ASYNC_SYNC },
        { "createTray", s_static_createTray, true, ClassData::ASYNC_SYNC },
        { "alert", s_static_alert, true, ClassData::ASYNC_ASYNC },
        { "confirm", s_static_confirm, true, ClassData::ASYNC_ASYNC },
        { "input", s_static_input, true, ClassData::ASYNC_ASYNC },
        { "chooseFile", s_static_chooseFile, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "WebView", WebView_base::class_info }
    };

    static ClassData s_cd = {
        "gui", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void gui_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = open(v0, v1, vr);

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = open(v0, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = openFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_createMenu(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Menu_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(std::vector<v8::Local<v8::Object>>, 0, std::vector<v8::Local<v8::Object>>());

    hr = createMenu(v0, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_createTray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Tray_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = createTray(v0, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_ENTER("gui.alert");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_alert(v0, cb, args);
    else
        hr = ac_alert(v0);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_alert(v0, v1, cb, args);
    else
        hr = ac_alert(v0, v1);

    ASYNC_METHOD_VOID();
}

inline void gui_base::s_static_confirm(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_ENTER("gui.confirm");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    if (!cb.IsEmpty())
        hr = acb_confirm(v0, cb, args);
    else
        hr = ac_confirm(v0, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    if (!cb.IsEmpty())
        hr = acb_confirm(v0, v1, cb, args);
    else
        hr = ac_confirm(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void gui_base::s_static_input(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_ENTER("gui.input");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    if (!cb.IsEmpty())
        hr = acb_input(v0, v1, cb, args);
    else
        hr = ac_input(v0, v1, vr);

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(bool, 2, false);

    if (!cb.IsEmpty())
        hr = acb_input(v0, v1, v2, cb, args);
    else
        hr = ac_input(v0, v1, v2, vr);

    ASYNC_METHOD_RETURN();
}

inline void gui_base::s_static_chooseFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    ASYNC_METHOD_ENTER("gui.chooseFile");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = acb_chooseFile(v0, cb, args);
    else
        hr = ac_chooseFile(v0, vr);

    ASYNC_METHOD_RETURN();
}
}
