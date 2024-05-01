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

class HttpCollection_base;

class querystring_base : public object_base {
    DECLARE_CLASS(querystring_base);

public:
    // querystring_base
    static result_t escape(exlib::string str, exlib::string& retVal);
    static result_t unescape(exlib::string str, exlib::string& retVal);
    static result_t parse(exlib::string str, exlib::string sep, exlib::string eq, v8::Local<v8::Object> opt, obj_ptr<HttpCollection_base>& retVal);
    static result_t stringify(v8::Local<v8::Object> obj, exlib::string sep, exlib::string eq, v8::Local<v8::Object> opt, exlib::string& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_escape(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_unescape(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_stringify(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/HttpCollection.h"

namespace fibjs {
inline ClassInfo& querystring_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "escape", s_static_escape, true, ClassData::ASYNC_SYNC },
        { "unescape", s_static_unescape, true, ClassData::ASYNC_SYNC },
        { "parse", s_static_parse, true, ClassData::ASYNC_SYNC },
        { "stringify", s_static_stringify, true, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "querystring", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void querystring_base::s_static_escape(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = escape(v0, vr);

    METHOD_RETURN();
}

inline void querystring_base::s_static_unescape(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = unescape(v0, vr);

    METHOD_RETURN();
}

inline void querystring_base::s_static_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpCollection_base> vr;

    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "&");
    OPT_ARG(exlib::string, 2, "=");
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    hr = parse(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void querystring_base::s_static_stringify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(v8::Local<v8::Object>, 0);
    OPT_ARG(exlib::string, 1, "&");
    OPT_ARG(exlib::string, 2, "=");
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate->m_isolate));

    hr = stringify(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}
}
