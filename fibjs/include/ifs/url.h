/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _url_base_H_
#define _url_base_H_

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

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "UrlObject.h"

namespace fibjs {
inline ClassInfo& url_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "format", s_format, true },
        { "parse", s_parse, true }
    };

    static ClassData s_cd = {
        "url", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void url_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("url.format");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = format(v0, vr);

    METHOD_RETURN();
}

inline void url_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
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
}

#endif
