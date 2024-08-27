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

class SecureContext_base;
class HttpResponse_base;
class Stream_base;
class HttpRequest_base;
class SeekableStream_base;

class HttpClient_base : public object_base {
    DECLARE_CLASS(HttpClient_base);

public:
    // HttpClient_base
    static result_t _new(obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SecureContext_base* context, obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_cookies(obj_ptr<NArray>& retVal) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t get_enableCookie(bool& retVal) = 0;
    virtual result_t set_enableCookie(bool newVal) = 0;
    virtual result_t get_autoRedirect(bool& retVal) = 0;
    virtual result_t set_autoRedirect(bool newVal) = 0;
    virtual result_t get_enableEncoding(bool& retVal) = 0;
    virtual result_t set_enableEncoding(bool newVal) = 0;
    virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
    virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
    virtual result_t get_maxHeaderSize(int32_t& retVal) = 0;
    virtual result_t set_maxHeaderSize(int32_t newVal) = 0;
    virtual result_t get_maxBodySize(int32_t& retVal) = 0;
    virtual result_t set_maxBodySize(int32_t newVal) = 0;
    virtual result_t get_userAgent(exlib::string& retVal) = 0;
    virtual result_t set_userAgent(exlib::string newVal) = 0;
    virtual result_t get_poolSize(int32_t& retVal) = 0;
    virtual result_t set_poolSize(int32_t newVal) = 0;
    virtual result_t get_poolTimeout(int32_t& retVal) = 0;
    virtual result_t set_poolTimeout(int32_t newVal) = 0;
    virtual result_t get_http_proxy(exlib::string& retVal) = 0;
    virtual result_t set_http_proxy(exlib::string newVal) = 0;
    virtual result_t get_https_proxy(exlib::string& retVal) = 0;
    virtual result_t set_https_proxy(exlib::string newVal) = 0;
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t request(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t request(v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t head(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_cookies(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_timeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_timeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_enableCookie(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_enableCookie(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_autoRedirect(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_autoRedirect(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxHeaderSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxHeaderSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_userAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_userAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_poolSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_poolSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_poolTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_poolTimeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_http_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_http_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_https_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_https_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_put(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_patch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_head(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(HttpClient_base, request, Stream_base*, HttpRequest_base*, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE4(HttpClient_base, request, Stream_base*, HttpRequest_base*, SeekableStream_base*, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE4(HttpClient_base, request, exlib::string, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, request, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE2(HttpClient_base, request, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, get, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, post, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, del, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, put, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, patch, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, head, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
};
}

#include "ifs/SecureContext.h"
#include "ifs/HttpResponse.h"
#include "ifs/Stream.h"
#include "ifs/HttpRequest.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& HttpClient_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "request", s_request, false, ClassData::ASYNC_ASYNC },
        { "get", s_get, false, ClassData::ASYNC_ASYNC },
        { "post", s_post, false, ClassData::ASYNC_ASYNC },
        { "del", s_del, false, ClassData::ASYNC_ASYNC },
        { "put", s_put, false, ClassData::ASYNC_ASYNC },
        { "patch", s_patch, false, ClassData::ASYNC_ASYNC },
        { "head", s_head, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "cookies", s_get_cookies, block_set, false },
        { "timeout", s_get_timeout, s_set_timeout, false },
        { "enableCookie", s_get_enableCookie, s_set_enableCookie, false },
        { "autoRedirect", s_get_autoRedirect, s_set_autoRedirect, false },
        { "enableEncoding", s_get_enableEncoding, s_set_enableEncoding, false },
        { "maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false },
        { "maxHeaderSize", s_get_maxHeaderSize, s_set_maxHeaderSize, false },
        { "maxBodySize", s_get_maxBodySize, s_set_maxBodySize, false },
        { "userAgent", s_get_userAgent, s_set_userAgent, false },
        { "poolSize", s_get_poolSize, s_set_poolSize, false },
        { "poolTimeout", s_get_poolTimeout, s_set_poolTimeout, false },
        { "http_proxy", s_get_http_proxy, s_set_http_proxy, false },
        { "https_proxy", s_get_https_proxy, s_set_https_proxy, false }
    };

    static ClassData s_cd = {
        "HttpClient", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpClient_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpClient_base::__new(const T& args)
{
    obj_ptr<HttpClient_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    METHOD_OVER(1, 1);

    ARG(obj_ptr<SecureContext_base>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpClient_base::s_get_cookies(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_cookies(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_get_timeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_timeout(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_timeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_timeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_enableCookie(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_enableCookie(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_enableCookie(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_enableCookie(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_autoRedirect(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_autoRedirect(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_autoRedirect(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_autoRedirect(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_enableEncoding(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_enableEncoding(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxHeadersCount(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_maxHeaderSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxHeaderSize(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_maxHeaderSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxHeaderSize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxBodySize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_userAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_userAgent(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_userAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_userAgent(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_poolSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_poolSize(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_poolSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_poolSize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_poolTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_poolTimeout(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_poolTimeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_poolTimeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_http_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_http_proxy(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_http_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_http_proxy(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_https_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_https_proxy(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_https_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_https_proxy(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_request(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_request(v0, v1, cb, args);
    else
        hr = pInst->ac_request(v0, v1, vr);

    ASYNC_METHOD_OVER(3, 3);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);
    ARG(obj_ptr<SeekableStream_base>, 2);

    if (!cb.IsEmpty())
        hr = pInst->acb_request(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_request(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_request(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_request(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_request(v0, v1, cb, args);
    else
        hr = pInst->ac_request(v0, v1, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_request(v0, cb, args);
    else
        hr = pInst->ac_request(v0, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_get(v0, v1, cb, args);
    else
        hr = pInst->ac_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_post(v0, v1, cb, args);
    else
        hr = pInst->ac_post(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_del(v0, v1, cb, args);
    else
        hr = pInst->ac_del(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_put(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_put(v0, v1, cb, args);
    else
        hr = pInst->ac_put(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_patch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_patch(v0, v1, cb, args);
    else
        hr = pInst->ac_patch(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_head(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_head(v0, v1, cb, args);
    else
        hr = pInst->ac_head(v0, v1, vr);

    METHOD_RETURN();
}
}
