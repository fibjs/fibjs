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
class TrayOptions;
class ChooseFileOptions;

class gui_base : public object_base {
    DECLARE_CLASS(gui_base);

public:
    // gui_base
    static result_t get_capabilities(v8::Local<v8::Object>& retVal);
    static result_t quit();
    static result_t open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static result_t open(v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static result_t openFile(exlib::string file, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);
    static result_t createMenu(std::vector<v8::Local<v8::Object>>& items, obj_ptr<Menu_base>& retVal);
    static result_t createTray(TrayOptions* opt, obj_ptr<Tray_base>& retVal);
    static result_t alert(exlib::string message);
    static result_t alert(exlib::string title, exlib::string message);
    static result_t confirm(exlib::string message, bool& retVal);
    static result_t confirm(exlib::string title, exlib::string message, bool& retVal);
    static result_t input(exlib::string message, bool password, exlib::string& retVal);
    static result_t input(exlib::string title, exlib::string message, bool password, exlib::string& retVal);
    static result_t chooseFile(ChooseFileOptions* options, std::vector<exlib::string>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<gui_base>& retVal)
    { return result_t::type_mismatch(); }

public:
    static void s_static_get_capabilities(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_quit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createTray(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_confirm(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_input(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_chooseFile(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/WebView.h"
#include "ifs/Menu.h"
#include "ifs/Tray.h"
#include "ifs/TrayOptions.h"
#include "ifs/ChooseFileOptions.h"

namespace fibjs {
inline ClassInfo& gui_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "quit", s_static_quit, true, ClassData::ASYNC_SYNC },
        { "open", s_static_open, true, ClassData::ASYNC_SYNC },
        { "openFile", s_static_openFile, true, ClassData::ASYNC_SYNC },
        { "createMenu", s_static_createMenu, true, ClassData::ASYNC_SYNC },
        { "createTray", s_static_createTray, true, ClassData::ASYNC_SYNC },
        { "alert", s_static_alert, true, ClassData::ASYNC_SYNC },
        { "confirm", s_static_confirm, true, ClassData::ASYNC_SYNC },
        { "input", s_static_input, true, ClassData::ASYNC_SYNC },
        { "chooseFile", s_static_chooseFile, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "WebView", WebView_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "capabilities", s_static_get_capabilities, block_set, true }
    };

    static ClassData s_cd = {
        "gui", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void gui_base::s_static_get_capabilities(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_capabilities(vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_quit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = quit();

    METHOD_VOID();
}

inline void gui_base::s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "url");

    ARG_NAMED(exlib::string, 0, "url");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "opt");

    hr = open(v0, v1, vr);

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate), "opt");

    hr = open(v0, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "file");

    ARG_NAMED(exlib::string, 0, "file");
    OPT_ARG_NAMED(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate), "opt");

    hr = openFile(v0, v1, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_createMenu(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Menu_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(std::vector<v8::Local<v8::Object>>, 0, std::vector<v8::Local<v8::Object>>(), "items");

    hr = createMenu(v0, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_createTray(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Tray_base> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "opt");

    ARG_NAMED(obj_ptr<TrayOptions>, 0, "opt");

    hr = createTray((TrayOptions*)v0, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_alert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "message");

    ARG_NAMED(exlib::string, 0, "message");

    hr = alert(v0);

    METHOD_OVER_NAMED(2, 2, "message");

    ARG_NAMED(exlib::string, 0, "title");
    ARG_NAMED(exlib::string, 1, "message");

    hr = alert(v0, v1);

    METHOD_VOID();
}

inline void gui_base::s_static_confirm(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "message");

    ARG_NAMED(exlib::string, 0, "message");

    hr = confirm(v0, vr);

    METHOD_OVER_NAMED(2, 2, "message");

    ARG_NAMED(exlib::string, 0, "title");
    ARG_NAMED(exlib::string, 1, "message");

    hr = confirm(v0, v1, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_input(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "message");

    ARG_NAMED(exlib::string, 0, "message");
    OPT_ARG_NAMED(bool, 1, false, "password");

    hr = input(v0, v1, vr);

    METHOD_OVER_NAMED(3, 2, "message");

    ARG_NAMED(exlib::string, 0, "title");
    ARG_NAMED(exlib::string, 1, "message");
    OPT_ARG_NAMED(bool, 2, false, "password");

    hr = input(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void gui_base::s_static_chooseFile(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::vector<exlib::string> vr;

    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "options");

    ARG_NAMED(obj_ptr<ChooseFileOptions>, 0, "options");

    hr = chooseFile((ChooseFileOptions*)v0, vr);

    METHOD_RETURN();
}
}
