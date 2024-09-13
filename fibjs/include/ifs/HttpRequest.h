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
#include "ifs/HttpMessage.h"

namespace fibjs {

class HttpMessage_base;
class HttpResponse_base;
class HttpCollection_base;

class HttpRequest_base : public HttpMessage_base {
    DECLARE_CLASS(HttpRequest_base);

public:
    // HttpRequest_base
    static result_t _new(obj_ptr<HttpRequest_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_response(obj_ptr<HttpResponse_base>& retVal) = 0;
    virtual result_t get_method(exlib::string& retVal) = 0;
    virtual result_t set_method(exlib::string newVal) = 0;
    virtual result_t get_address(exlib::string& retVal) = 0;
    virtual result_t set_address(exlib::string newVal) = 0;
    virtual result_t get_queryString(exlib::string& retVal) = 0;
    virtual result_t set_queryString(exlib::string newVal) = 0;
    virtual result_t get_cookies(obj_ptr<HttpCollection_base>& retVal) = 0;
    virtual result_t get_form(obj_ptr<HttpCollection_base>& retVal) = 0;
    virtual result_t get_query(obj_ptr<HttpCollection_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_response(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_method(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_method(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_address(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_address(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_queryString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_queryString(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_cookies(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_form(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_query(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/HttpResponse.h"
#include "ifs/HttpCollection.h"

namespace fibjs {
inline ClassInfo& HttpRequest_base::class_info()
{
    static ClassData::ClassProperty s_property[] = {
        { "response", s_get_response, block_set, false },
        { "method", s_get_method, s_set_method, false },
        { "address", s_get_address, s_set_address, false },
        { "queryString", s_get_queryString, s_set_queryString, false },
        { "cookies", s_get_cookies, block_set, false },
        { "form", s_get_form, block_set, false },
        { "query", s_get_query, block_set, false }
    };

    static ClassData s_cd = {
        "HttpRequest", false, s__new, NULL,
        0, NULL, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &HttpMessage_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpRequest_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpRequest_base::__new(const T& args)
{
    obj_ptr<HttpRequest_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpRequest_base::s_get_response(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_response(vr);

    METHOD_RETURN();
}

inline void HttpRequest_base::s_get_method(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_method(vr);

    METHOD_RETURN();
}

inline void HttpRequest_base::s_set_method(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_method(v0);

    METHOD_VOID();
}

inline void HttpRequest_base::s_get_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_address(vr);

    METHOD_RETURN();
}

inline void HttpRequest_base::s_set_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_address(v0);

    METHOD_VOID();
}

inline void HttpRequest_base::s_get_queryString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_queryString(vr);

    METHOD_RETURN();
}

inline void HttpRequest_base::s_set_queryString(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_queryString(v0);

    METHOD_VOID();
}

inline void HttpRequest_base::s_get_cookies(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpCollection_base> vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_cookies(vr);

    METHOD_RETURN();
}

inline void HttpRequest_base::s_get_form(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpCollection_base> vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_form(vr);

    METHOD_RETURN();
}

inline void HttpRequest_base::s_get_query(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpCollection_base> vr;

    METHOD_INSTANCE(HttpRequest_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_query(vr);

    METHOD_RETURN();
}
}
