/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _gui_base_H_
#define _gui_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class WebView_base;

class gui_base : public object_base {
    DECLARE_CLASS(gui_base);

public:
    enum {
        _IE7 = 7000,
        _IE8 = 8000,
        _IE9 = 9000,
        _IE10 = 10000,
        _IE11 = 11000,
        _EDGE = 11001
    };

public:
    // gui_base
    static result_t setVersion(int32_t ver);
    static result_t open(exlib::string url, v8::Local<v8::Object> opt, obj_ptr<WebView_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_setVersion(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "WebView.h"

namespace fibjs {
inline ClassInfo& gui_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setVersion", s_setVersion, true },
        { "open", s_open, true }
    };

    static ClassData::ClassConst s_const[] = {
        { "IE7", _IE7 },
        { "IE8", _IE8 },
        { "IE9", _IE9 },
        { "IE10", _IE10 },
        { "IE11", _IE11 },
        { "EDGE", _EDGE }
    };

    static ClassData s_cd = {
        "gui", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, ARRAYSIZE(s_const), s_const, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void gui_base::s_setVersion(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("gui.setVersion");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = setVersion(v0);

    METHOD_VOID();
}

inline void gui_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_NAME("gui.open");
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    hr = open(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
