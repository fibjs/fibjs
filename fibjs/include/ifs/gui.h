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
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_IE7(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_IE8(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_IE9(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_IE10(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_IE11(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_EDGE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
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

    static ClassData::ClassProperty s_property[] = {
        { "IE7", s_get_IE7, block_set, true },
        { "IE8", s_get_IE8, block_set, true },
        { "IE9", s_get_IE9, block_set, true },
        { "IE10", s_get_IE10, block_set, true },
        { "IE11", s_get_IE11, block_set, true },
        { "EDGE", s_get_EDGE, block_set, true }
    };

    static ClassData s_cd = {
        "gui", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void gui_base::s_get_IE7(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _IE7;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void gui_base::s_get_IE8(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _IE8;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void gui_base::s_get_IE9(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _IE9;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void gui_base::s_get_IE10(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _IE10;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void gui_base::s_get_IE11(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _IE11;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void gui_base::s_get_EDGE(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr = _EDGE;
    PROPERTY_ENTER();
    METHOD_RETURN();
}

inline void gui_base::s_setVersion(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = setVersion(v0);

    METHOD_VOID();
}

inline void gui_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebView_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    hr = open(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
