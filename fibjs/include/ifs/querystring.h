/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _querystring_base_H_
#define _querystring_base_H_

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

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_escape(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unescape(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stringify(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "HttpCollection.h"

namespace fibjs {
inline ClassInfo& querystring_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "escape", s_escape, true },
        { "unescape", s_unescape, true },
        { "parse", s_parse, true },
        { "stringify", s_stringify, true }
    };

    static ClassData s_cd = {
        "querystring", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void querystring_base::s_escape(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("querystring.escape");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = escape(v0, vr);

    METHOD_RETURN();
}

inline void querystring_base::s_unescape(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("querystring.unescape");
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = unescape(v0, vr);

    METHOD_RETURN();
}

inline void querystring_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpCollection_base> vr;

    METHOD_NAME("querystring.parse");
    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "&");
    OPT_ARG(exlib::string, 2, "=");
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

    hr = parse(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}

inline void querystring_base::s_stringify(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("querystring.stringify");
    METHOD_ENTER();

    METHOD_OVER(4, 1);

    ARG(v8::Local<v8::Object>, 0);
    OPT_ARG(exlib::string, 1, "&");
    OPT_ARG(exlib::string, 2, "=");
    OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

    hr = stringify(v0, v1, v2, v3, vr);

    METHOD_RETURN();
}
}

#endif
