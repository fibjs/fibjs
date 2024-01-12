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

class HttpRequest_base;
class HttpResponse_base;
class HttpCookie_base;
class HttpServer_base;
class HttpClient_base;
class HttpsServer_base;
class HttpHandler_base;
class HttpRepeater_base;
class Handler_base;
class X509Cert_base;
class PKey_base;
class Stream_base;
class SeekableStream_base;

class http_base : public object_base {
    DECLARE_CLASS(http_base);

public:
    // http_base
    static result_t get_STATUS_CODES(v8::Local<v8::Object>& retVal);
    static result_t get_cookies(obj_ptr<NArray>& retVal);
    static result_t get_timeout(int32_t& retVal);
    static result_t set_timeout(int32_t newVal);
    static result_t get_enableCookie(bool& retVal);
    static result_t set_enableCookie(bool newVal);
    static result_t get_autoRedirect(bool& retVal);
    static result_t set_autoRedirect(bool newVal);
    static result_t get_enableEncoding(bool& retVal);
    static result_t set_enableEncoding(bool newVal);
    static result_t get_maxHeadersCount(int32_t& retVal);
    static result_t set_maxHeadersCount(int32_t newVal);
    static result_t get_maxHeaderLength(int32_t& retVal);
    static result_t set_maxHeaderLength(int32_t newVal);
    static result_t get_maxBodySize(int32_t& retVal);
    static result_t set_maxBodySize(int32_t newVal);
    static result_t get_userAgent(exlib::string& retVal);
    static result_t set_userAgent(exlib::string newVal);
    static result_t get_poolSize(int32_t& retVal);
    static result_t set_poolSize(int32_t newVal);
    static result_t get_poolTimeout(int32_t& retVal);
    static result_t set_poolTimeout(int32_t newVal);
    static result_t get_http_proxy(exlib::string& retVal);
    static result_t set_http_proxy(exlib::string newVal);
    static result_t get_https_proxy(exlib::string& retVal);
    static result_t set_https_proxy(exlib::string newVal);
    static result_t fileHandler(exlib::string root, v8::Local<v8::Object> mimes, bool autoIndex, obj_ptr<Handler_base>& retVal);
    static result_t setClientCert(X509Cert_base* crt, PKey_base* key);
    static result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t request(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t request(v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t head(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_static_get_STATUS_CODES(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_cookies(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_get_timeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_timeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_enableCookie(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_enableCookie(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_autoRedirect(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_autoRedirect(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_maxHeaderLength(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_maxHeaderLength(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_userAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_userAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_poolSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_poolSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_poolTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_poolTimeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_http_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_http_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_get_https_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_static_set_https_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_static_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_post(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_put(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_patch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_head(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(http_base, request, Stream_base*, HttpRequest_base*, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE4(http_base, request, Stream_base*, HttpRequest_base*, SeekableStream_base*, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE4(http_base, request, exlib::string, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, request, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE2(http_base, request, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, get, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, post, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, del, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, put, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, patch, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, head, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
};
}

#include "ifs/HttpRequest.h"
#include "ifs/HttpResponse.h"
#include "ifs/HttpCookie.h"
#include "ifs/HttpServer.h"
#include "ifs/HttpClient.h"
#include "ifs/HttpsServer.h"
#include "ifs/HttpHandler.h"
#include "ifs/HttpRepeater.h"
#include "ifs/Handler.h"
#include "ifs/X509Cert.h"
#include "ifs/PKey.h"
#include "ifs/Stream.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& http_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "fileHandler", s_static_fileHandler, true, false },
        { "setClientCert", s_static_setClientCert, true, false },
        { "request", s_static_request, true, true },
        { "requestSync", s_static_request, true, false },
        { "get", s_static_get, true, true },
        { "getSync", s_static_get, true, false },
        { "post", s_static_post, true, true },
        { "postSync", s_static_post, true, false },
        { "del", s_static_del, true, true },
        { "delSync", s_static_del, true, false },
        { "put", s_static_put, true, true },
        { "putSync", s_static_put, true, false },
        { "patch", s_static_patch, true, true },
        { "patchSync", s_static_patch, true, false },
        { "head", s_static_head, true, true },
        { "headSync", s_static_head, true, false }
    };

    static ClassData::ClassObject s_object[] = {
        { "Request", HttpRequest_base::class_info },
        { "Response", HttpResponse_base::class_info },
        { "Cookie", HttpCookie_base::class_info },
        { "Server", HttpServer_base::class_info },
        { "Client", HttpClient_base::class_info },
        { "HttpsServer", HttpsServer_base::class_info },
        { "Handler", HttpHandler_base::class_info },
        { "Repeater", HttpRepeater_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "STATUS_CODES", s_static_get_STATUS_CODES, block_set, true },
        { "cookies", s_static_get_cookies, block_set, true },
        { "timeout", s_static_get_timeout, s_static_set_timeout, true },
        { "enableCookie", s_static_get_enableCookie, s_static_set_enableCookie, true },
        { "autoRedirect", s_static_get_autoRedirect, s_static_set_autoRedirect, true },
        { "enableEncoding", s_static_get_enableEncoding, s_static_set_enableEncoding, true },
        { "maxHeadersCount", s_static_get_maxHeadersCount, s_static_set_maxHeadersCount, true },
        { "maxHeaderLength", s_static_get_maxHeaderLength, s_static_set_maxHeaderLength, true },
        { "maxBodySize", s_static_get_maxBodySize, s_static_set_maxBodySize, true },
        { "userAgent", s_static_get_userAgent, s_static_set_userAgent, true },
        { "poolSize", s_static_get_poolSize, s_static_set_poolSize, true },
        { "poolTimeout", s_static_get_poolTimeout, s_static_set_poolTimeout, true },
        { "http_proxy", s_static_get_http_proxy, s_static_set_http_proxy, true },
        { "https_proxy", s_static_get_https_proxy, s_static_set_https_proxy, true }
    };

    static ClassData s_cd = {
        "http", true, s__new, NULL,
        ARRAYSIZE(s_method), s_method, ARRAYSIZE(s_object), s_object, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void http_base::s_static_get_STATUS_CODES(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    PROPERTY_ENTER();

    hr = get_STATUS_CODES(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get_cookies(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    PROPERTY_ENTER();

    hr = get_cookies(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get_timeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_timeout(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_timeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_timeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_enableCookie(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    PROPERTY_ENTER();

    hr = get_enableCookie(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_enableCookie(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = set_enableCookie(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_autoRedirect(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    PROPERTY_ENTER();

    hr = get_autoRedirect(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_autoRedirect(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = set_autoRedirect(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    PROPERTY_ENTER();

    hr = get_enableEncoding(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = set_enableEncoding(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_maxHeadersCount(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_maxHeaderLength(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_maxHeaderLength(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxHeaderLength(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_maxHeaderLength(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_maxBodySize(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_userAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_userAgent(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_userAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = set_userAgent(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_poolSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_poolSize(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_poolSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_poolSize(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_poolTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    PROPERTY_ENTER();

    hr = get_poolTimeout(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_poolTimeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = set_poolTimeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_http_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_http_proxy(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_http_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = set_http_proxy(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_get_https_proxy(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    PROPERTY_ENTER();

    hr = get_https_proxy(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_https_proxy(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = set_https_proxy(v0);

    PROPERTY_SET_LEAVE();
}

inline void http_base::s_static_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));
    OPT_ARG(bool, 2, false);

    hr = fileHandler(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);

    hr = setClientCert(v0, v1);

    METHOD_VOID();
}

inline void http_base::s_static_request(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, cb, args);
    else
        hr = ac_request(v0, v1, vr);

    ASYNC_METHOD_OVER(3, 3);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);
    ARG(obj_ptr<SeekableStream_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, v2, cb, args);
    else
        hr = ac_request(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, v2, cb, args);
    else
        hr = ac_request(v0, v1, v2, vr);

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, cb, args);
    else
        hr = ac_request(v0, v1, vr);

    ASYNC_METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = acb_request(v0, cb, args);
    else
        hr = ac_request(v0, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_get(v0, v1, cb, args);
    else
        hr = ac_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_post(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_post(v0, v1, cb, args);
    else
        hr = ac_post(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_del(v0, v1, cb, args);
    else
        hr = ac_del(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_put(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_put(v0, v1, cb, args);
    else
        hr = ac_put(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_patch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_patch(v0, v1, cb, args);
    else
        hr = ac_patch(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_head(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_head(v0, v1, cb, args);
    else
        hr = ac_head(v0, v1, vr);

    METHOD_RETURN();
}
}
