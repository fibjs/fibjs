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

namespace fibjs
{

class WebView_base;

class gui_base : public object_base
{
    DECLARE_CLASS(gui_base);

public:
    // gui_base
    static result_t load(exlib::string url, exlib::string title, obj_ptr<WebView_base>& retVal, AsyncEvent* ac);
    static result_t open(exlib::string url, exlib::string title, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_load(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_open(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(gui_base, load, exlib::string, exlib::string, obj_ptr<WebView_base>);
    ASYNC_STATIC2(gui_base, open, exlib::string, exlib::string);
};

}

#include "WebView.h"

namespace fibjs
{
    inline ClassInfo& gui_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"load", s_load, true},
            {"open", s_open, true}
        };

        static ClassData s_cd = 
        { 
            "gui", s__new, NULL, 
            2, s_method, 0, NULL, 0, NULL, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void gui_base::s_load(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<WebView_base> vr;

        ASYNC_METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(exlib::string, 1, "");

        if(!cb.IsEmpty()) {
            acb_load(v0, v1, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_load(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void gui_base::s_open(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        ASYNC_METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(exlib::string, 1, "");

        if(!cb.IsEmpty()) {
            acb_open(v0, v1, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_open(v0, v1);

        METHOD_VOID();
    }

}

#endif

