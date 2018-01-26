/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpCookie_base_H_
#define _HttpCookie_base_H_

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
    static void s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_name(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_value(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_value(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_domain(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_domain(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_path(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_path(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_expires(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_expires(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_httpOnly(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_httpOnly(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_secure(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_secure(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

namespace fibjs {
inline ClassInfo& HttpCookie_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "parse", s_parse, false },
        { "match", s_match, false }
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
        &object_base::class_info()
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

    METHOD_NAME("new HttpCookie()");
    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate));

    hr = _new(v0, vr, args.This());

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpCookie_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpCookie.parse");
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

    METHOD_NAME("HttpCookie.match");
    METHOD_INSTANCE(HttpCookie_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->match(v0, vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_get_name(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpCookie.name");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_name(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_name(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.name");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_name(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_value(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpCookie.value");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_value(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_value(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.value");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_value(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_domain(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpCookie.domain");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_domain(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_domain(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.domain");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_domain(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_path(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpCookie.path");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_path(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_path(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.path");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_path(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_expires(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    date_t vr;

    METHOD_NAME("HttpCookie.expires");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_expires(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_expires(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.expires");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(date_t);

    hr = pInst->set_expires(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_httpOnly(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("HttpCookie.httpOnly");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_httpOnly(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_httpOnly(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.httpOnly");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_httpOnly(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpCookie_base::s_get_secure(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("HttpCookie.secure");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();

    hr = pInst->get_secure(vr);

    METHOD_RETURN();
}

inline void HttpCookie_base::s_set_secure(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpCookie.secure");
    METHOD_INSTANCE(HttpCookie_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_secure(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
