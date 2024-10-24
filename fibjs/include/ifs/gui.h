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
    static result_t createMenu(v8::Local<v8::Array> items, obj_ptr<Menu_base>& retVal);
    static result_t createTray(v8::Local<v8::Object> opt, obj_ptr<Tray_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_open(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_openFile(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createMenu(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_createTray(const v8::FunctionCallbackInfo<v8::Value>& args);
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
        { "createTray", s_static_createTray, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "WebView", WebView_base::class_info }
    };

    static ClassData s_cd = {
        "gui", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
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

    OPT_ARG(v8::Local<v8::Array>, 0, v8::Array::New(isolate->m_isolate));

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
}
