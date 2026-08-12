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
class HttpCookie_base;

class HttpResponse_base : public HttpMessage_base {
    DECLARE_CLASS(HttpResponse_base);

public:
    // HttpResponse_base
    static result_t _new(obj_ptr<HttpResponse_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Value> body, v8::Local<v8::Object> options, obj_ptr<HttpResponse_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_statusCode(int32_t& retVal) = 0;
    virtual result_t set_statusCode(int32_t newVal) = 0;
    virtual result_t get_statusMessage(exlib::string& retVal) = 0;
    virtual result_t set_statusMessage(exlib::string newVal) = 0;
    virtual result_t get_statusText(exlib::string& retVal) = 0;
    virtual result_t set_statusText(exlib::string newVal) = 0;
    virtual result_t get_status(int32_t& retVal) = 0;
    virtual result_t set_status(int32_t newVal) = 0;
    virtual result_t get_ok(bool& retVal) = 0;
    virtual result_t writeHead(int32_t statusCode, exlib::string statusMessage, v8::Local<v8::Object> headers) = 0;
    virtual result_t writeHead(int32_t statusCode, v8::Local<v8::Object> headers) = 0;
    virtual result_t get_cookies(obj_ptr<NArray>& retVal) = 0;
    virtual result_t addCookie(HttpCookie_base* cookie) = 0;
    virtual result_t redirect(exlib::string url) = 0;
    virtual result_t redirect(int32_t statusCode, exlib::string url) = 0;
    virtual result_t get_url(exlib::string& retVal) = 0;
    virtual result_t get_redirected(bool& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t json(v8::Local<v8::Value> data, v8::Local<v8::Object> options, Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t json(Variant& retVal, AsyncEvent* ac) = 0;
    static result_t json(v8::Local<v8::Value> data, v8::Local<v8::Object> options, obj_ptr<HttpResponse_base>& retVal);
    static result_t redirect(exlib::string url, int32_t status, obj_ptr<HttpResponse_base>& retVal);
    static result_t error(obj_ptr<HttpResponse_base>& retVal);

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<HttpResponse_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_statusCode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_statusCode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_statusMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_statusMessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_statusText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_statusText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_status(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_status(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ok(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_writeHead(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_cookies(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addCookie(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_redirect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_redirected(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_redirect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(HttpResponse_base, json, v8::Local<v8::Value>, v8::Local<v8::Object>, Variant);
    ASYNC_MEMBERVALUE1(HttpResponse_base, json, Variant);
};
}

#include "ifs/HttpCookie.h"

namespace fibjs {
inline ClassInfo& HttpResponse_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "writeHead", s_writeHead, false, ClassData::ASYNC_SYNC },
        { "addCookie", s_addCookie, false, ClassData::ASYNC_SYNC },
        { "redirect", s_redirect, false, ClassData::ASYNC_SYNC },
        { "json", s_json, false, ClassData::ASYNC_ASYNC },
        { "json", s_static_json, true, ClassData::ASYNC_SYNC },
        { "redirect", s_static_redirect, true, ClassData::ASYNC_SYNC },
        { "error", s_static_error, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "statusCode", s_get_statusCode, s_set_statusCode, false },
        { "statusMessage", s_get_statusMessage, s_set_statusMessage, false },
        { "statusText", s_get_statusText, s_set_statusText, false },
        { "status", s_get_status, s_set_status, false },
        { "ok", s_get_ok, block_set, false },
        { "cookies", s_get_cookies, block_set, false },
        { "url", s_get_url, block_set, false },
        { "redirected", s_get_redirected, block_set, false },
        { "type", s_get_type, block_set, false }
    };

    static ClassData s_cd = {
        "HttpResponse", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &HttpMessage_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpResponse_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void HttpResponse_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t HttpResponse_base::load(v8::Local<v8::Value> v, obj_ptr<HttpResponse_base>& retVal)
{
    obj_ptr<HttpResponse_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    LOAD_RETURN();
}

inline void HttpResponse_base::s_get_statusCode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_statusCode(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_set_statusCode(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_statusCode(v0);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_statusMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_statusMessage(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_set_statusMessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_statusMessage(v0);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_statusText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_statusText(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_set_statusText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_statusText(v0);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_status(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_status(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_set_status(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_status(v0);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_ok(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ok(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_writeHead(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = pInst->writeHead(v0, v1, v2);

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = pInst->writeHead(v0, v1);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_cookies(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_cookies(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_addCookie(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<HttpCookie_base>, 0);

    hr = pInst->addCookie(v0.get());

    METHOD_VOID();
}

inline void HttpResponse_base::s_redirect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->redirect(v0);

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);

    hr = pInst->redirect(v0, v1);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_url(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_get_redirected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_redirected(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(HttpResponse_base);
    ASYNC_METHOD_ENTER("HttpResponse.json");

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_json(v0, v1, cb, args);
    else
        hr = pInst->ac_json(v0, v1, vr);

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_json(cb, args);
    else
        hr = pInst->ac_json(vr);

    ASYNC_METHOD_RETURN();
}

inline void HttpResponse_base::s_static_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = json(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_static_redirect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 302);

    hr = redirect(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = error(vr);

    METHOD_RETURN();
}
}
