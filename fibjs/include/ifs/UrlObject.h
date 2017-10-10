/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _UrlObject_base_H_
#define _UrlObject_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class UrlObject_base : public object_base {
    DECLARE_CLASS(UrlObject_base);

public:
    // UrlObject_base
    static result_t _new(v8::Local<v8::Object> args, obj_ptr<UrlObject_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string url, bool parseQueryString, bool slashesDenoteHost, obj_ptr<UrlObject_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t parse(exlib::string url, bool parseQueryString, bool slashesDenoteHost) = 0;
    virtual result_t format(v8::Local<v8::Object> args) = 0;
    virtual result_t resolve(exlib::string url, obj_ptr<UrlObject_base>& retVal) = 0;
    virtual result_t normalize() = 0;
    virtual result_t get_href(exlib::string& retVal) = 0;
    virtual result_t set_href(exlib::string newVal) = 0;
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t set_protocol(exlib::string newVal) = 0;
    virtual result_t get_slashes(bool& retVal) = 0;
    virtual result_t set_slashes(bool newVal) = 0;
    virtual result_t get_auth(exlib::string& retVal) = 0;
    virtual result_t set_auth(exlib::string newVal) = 0;
    virtual result_t get_username(exlib::string& retVal) = 0;
    virtual result_t set_username(exlib::string newVal) = 0;
    virtual result_t get_password(exlib::string& retVal) = 0;
    virtual result_t set_password(exlib::string newVal) = 0;
    virtual result_t get_host(exlib::string& retVal) = 0;
    virtual result_t set_host(exlib::string newVal) = 0;
    virtual result_t get_hostname(exlib::string& retVal) = 0;
    virtual result_t set_hostname(exlib::string newVal) = 0;
    virtual result_t get_port(exlib::string& retVal) = 0;
    virtual result_t set_port(exlib::string newVal) = 0;
    virtual result_t get_path(exlib::string& retVal) = 0;
    virtual result_t set_path(exlib::string newVal) = 0;
    virtual result_t get_pathname(exlib::string& retVal) = 0;
    virtual result_t set_pathname(exlib::string newVal) = 0;
    virtual result_t get_search(exlib::string& retVal) = 0;
    virtual result_t set_search(exlib::string newVal) = 0;
    virtual result_t get_query(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t set_query(v8::Local<v8::Value> newVal) = 0;
    virtual result_t get_hash(exlib::string& retVal) = 0;
    virtual result_t set_hash(exlib::string newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_parse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_format(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_href(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_href(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_protocol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_slashes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_slashes(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_auth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_auth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_username(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_username(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_password(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_password(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_host(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_host(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_hostname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_port(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_port(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_path(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_path(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_pathname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_pathname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_search(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_search(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_query(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_query(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_hash(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_hash(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

namespace fibjs {
inline ClassInfo& UrlObject_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "parse", s_parse, false },
        { "format", s_format, false },
        { "resolve", s_resolve, false },
        { "normalize", s_normalize, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "href", s_get_href, s_set_href, false },
        { "protocol", s_get_protocol, s_set_protocol, false },
        { "slashes", s_get_slashes, s_set_slashes, false },
        { "auth", s_get_auth, s_set_auth, false },
        { "username", s_get_username, s_set_username, false },
        { "password", s_get_password, s_set_password, false },
        { "host", s_get_host, s_set_host, false },
        { "hostname", s_get_hostname, s_set_hostname, false },
        { "port", s_get_port, s_set_port, false },
        { "path", s_get_path, s_set_path, false },
        { "pathname", s_get_pathname, s_set_pathname, false },
        { "search", s_get_search, s_set_search, false },
        { "query", s_get_query, s_set_query, false },
        { "hash", s_get_hash, s_set_hash, false }
    };

    static ClassData s_cd = {
        "UrlObject", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void UrlObject_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void UrlObject_base::__new(const T& args)
{
    obj_ptr<UrlObject_base> vr;

    CONSTRUCT_ENTER("new UrlObject()");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(3, 0);

    OPT_ARG(exlib::string, 0, "");
    OPT_ARG(bool, 1, false);
    OPT_ARG(bool, 2, false);

    hr = _new(v0, v1, v2, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void UrlObject_base::s_parse(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    METHOD_ENTER("UrlObject.parse");

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);
    OPT_ARG(bool, 2, false);

    hr = pInst->parse(v0, v1, v2);

    METHOD_VOID();
}

inline void UrlObject_base::s_format(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    METHOD_ENTER("UrlObject.format");

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->format(v0);

    METHOD_VOID();
}

inline void UrlObject_base::s_resolve(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<UrlObject_base> vr;

    METHOD_INSTANCE(UrlObject_base);
    METHOD_ENTER("UrlObject.resolve");

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->resolve(v0, vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_normalize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    METHOD_ENTER("UrlObject.normalize");

    METHOD_OVER(0, 0);

    hr = pInst->normalize();

    METHOD_VOID();
}

inline void UrlObject_base::s_get_href(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.href");

    hr = pInst->get_href(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_href(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.href");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_href(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_protocol(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.protocol");

    hr = pInst->get_protocol(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_protocol(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.protocol");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_protocol(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_slashes(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.slashes");

    hr = pInst->get_slashes(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_slashes(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.slashes");
    PROPERTY_VAL(bool);

    hr = pInst->set_slashes(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_auth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.auth");

    hr = pInst->get_auth(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_auth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.auth");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_auth(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_username(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.username");

    hr = pInst->get_username(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_username(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.username");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_username(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_password(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.password");

    hr = pInst->get_password(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_password(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.password");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_password(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_host(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.host");

    hr = pInst->get_host(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_host(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.host");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_host(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_hostname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.hostname");

    hr = pInst->get_hostname(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_hostname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.hostname");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_hostname(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_port(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.port");

    hr = pInst->get_port(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_port(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.port");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_port(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_path(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.path");

    hr = pInst->get_path(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_path(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.path");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_path(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_pathname(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.pathname");

    hr = pInst->get_pathname(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_pathname(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.pathname");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_pathname(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_search(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.search");

    hr = pInst->get_search(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_search(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.search");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_search(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_query(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.query");

    hr = pInst->get_query(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_query(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.query");
    PROPERTY_VAL(v8::Local<v8::Value>);

    hr = pInst->set_query(v0);

    PROPERTY_SET_LEAVE();
}

inline void UrlObject_base::s_get_hash(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.hash");

    hr = pInst->get_hash(vr);

    METHOD_RETURN();
}

inline void UrlObject_base::s_set_hash(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(UrlObject_base);
    PROPERTY_ENTER("UrlObject.hash");
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_hash(v0);

    PROPERTY_SET_LEAVE();
}
}

#endif
