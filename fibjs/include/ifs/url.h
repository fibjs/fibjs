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
    static result_t fileURLToPath(UrlObject_base* url, exlib::string& retVal);
    static result_t fileURLToPath(exlib::string url, exlib::string& retVal);
    static result_t pathToFileURL(exlib::string path, obj_ptr<UrlObject_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fileURLToPath(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_pathToFileURL(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/UrlObject.h"

namespace fibjs {
inline ClassInfo& url_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "format", s_static_format, true, ClassData::ASYNC_SYNC },
        { "parse", s_static_parse, true, ClassData::ASYNC_SYNC },
        { "resolve", s_static_resolve, true, ClassData::ASYNC_SYNC },
        { "fileURLToPath", s_static_fileURLToPath, true, ClassData::ASYNC_SYNC },
        { "pathToFileURL", s_static_pathToFileURL, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "URL", UrlObject_base::class_info }
    };

    static ClassData s_cd = {
        "url", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void url_base::s_static_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = format(v0, vr);

    METHOD_RETURN();
}

inline void url_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<UrlObject_base> vr;

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

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = resolve(v0, v1, vr);

    METHOD_RETURN();
}

inline void url_base::s_static_fileURLToPath(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<UrlObject_base>, 0);

    hr = fileURLToPath(v0, vr);

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = fileURLToPath(v0, vr);

    METHOD_RETURN();
}

inline void url_base::s_static_pathToFileURL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<UrlObject_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pathToFileURL(v0, vr);

    METHOD_RETURN();
}
}
