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

class UrlObject_base;

class url_base : public object_base {
    DECLARE_CLASS(url_base);

public:
    // url_base
    static result_t format(v8::Local<v8::Object> args, exlib::string& retVal);
    static result_t parse(exlib::string url, bool parseQueryString, bool slashesDenoteHost, obj_ptr<UrlObject_base>& retVal);
    static result_t resolve(exlib::string _from, exlib::string to, exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/UrlObject.h"

namespace fibjs {
inline ClassInfo& url_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "format", s_static_format, true },
        { "parse", s_static_parse, true },
        { "resolve", s_static_resolve, true }
    };

    static ClassData::ClassObject s_object[] = {
        { "URL", UrlObject_base::class_info }
    };

    static ClassData s_cd = {
        "url", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void url_base::s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("url.format");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = format(v0, vr);

    METHOD_RETURN();
}

inline void url_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<UrlObject_base> vr;

    METHOD_NAME("url.parse");
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);
    OPT_ARG(bool, 2, false);

    hr = parse(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void url_base::s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("url.resolve");
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = resolve(v0, v1, vr);

    METHOD_RETURN();
}
}
