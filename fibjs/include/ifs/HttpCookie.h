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

class HttpCookie_base : public object_base {
    DECLARE_CLASS(HttpCookie_base);

public:
    // HttpCookie_base
    static result_t _new(v8::Local<v8::Object> opts, obj_ptr<HttpCookie_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string name, exlib::string value, v8::Local<v8::Object> opts, obj_ptr<HttpCookie_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t parse(exlib::string header) = 0;
    virtual result_t match(exlib::string url, bool& retVal) = 0;
    virtual result_t get_name(exlib::string& retVal) = 0;
    virtual result_t set_name(exlib::string newVal) = 0;
    virtual result_t get_value(exlib::string& retVal) = 0;
    virtual result_t set_value(exlib::string newVal) = 0;
    virtual result_t get_domain(exlib::string& retVal) = 0;
    virtual result_t set_domain(exlib::string newVal) = 0;
    virtual result_t get_path(exlib::string& retVal) = 0;
    virtual result_t set_path(exlib::string newVal) = 0;
    virtual result_t get_expires(date_t& retVal) = 0;
    virtual result_t set_expires(date_t newVal) = 0;
    virtual result_t get_httpOnly(bool& retVal) = 0;
    virtual result_t set_httpOnly(bool newVal) = 0;
    virtual result_t get_secure(bool& retVal) = 0;
    virtual result_t set_secure(bool newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_match(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_name(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_value(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_value(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_domain(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_domain(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_path(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_path(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_expires(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_expires(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_httpOnly(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_httpOnly(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_secure(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_secure(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& HttpCookie_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "parse", s_parse, false, ClassData::ASYNC_SYNC },
        { "match", s_match, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "name", s_get_name, s_set_name, false },
        { "value", s_get_value, s_set_value, false },
        { "domain", s_get_domain, s_set_domain, false },
        { "path", s_get_path, s_set_path, false },
        { "expires", s_get_expires, s_set_expires, false },
        { "httpOnly", s_get_httpOnly, s_set_httpOnly, false },
        { "secure", s_get_secure, s_set_secure, false }
    };

    static ClassData s_cd = {
        "HttpCookie", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpCookie_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpCookie_base::__new(const T& args)
{
    obj_ptr<HttpCookie_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpCookie_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->parse(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_match(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->match(v0, vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_get_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_name(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_name(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_get_value(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_value(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_value(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_value(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_get_domain(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_domain(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_domain(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_domain(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_get_path(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_path(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_path(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_path(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_get_expires(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_expires(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_expires(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(date_t, 0);

    hr = pInst->set_expires(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_get_httpOnly(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_httpOnly(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_httpOnly(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_httpOnly(v0);

    METHOD_VOID();
}

inline void HttpCookie_base::s_get_secure(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_secure(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_secure(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_secure(v0);

    METHOD_VOID();
}
}
