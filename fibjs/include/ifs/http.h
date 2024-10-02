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
class Stream_base;
class SeekableStream_base;

class http_base : public object_base {
    DECLARE_CLASS(http_base);

public:
    // http_base
    static result_t get_STATUS_CODES(v8::Local<v8::Object>& retVal);
    static result_t get_cookies(obj_ptr<NArray>& retVal);
    static result_t get_keepAlive(bool& retVal);
    static result_t set_keepAlive(bool newVal);
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
    static result_t get_maxHeaderSize(int32_t& retVal);
    static result_t set_maxHeaderSize(int32_t newVal);
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
    static result_t fileHandler(exlib::string root, bool autoIndex, obj_ptr<Handler_base>& retVal);
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
    static void s_static_get_STATUS_CODES(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_cookies(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_enableCookie(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_enableCookie(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_autoRedirect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_autoRedirect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_userAgent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_userAgent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_poolSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_poolSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_poolTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_poolTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_http_proxy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_http_proxy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_https_proxy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_https_proxy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
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
#include "ifs/Stream.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& http_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "fileHandler", s_static_fileHandler, true, ClassData::ASYNC_SYNC },
        { "request", s_static_request, true, ClassData::ASYNC_ASYNC },
        { "get", s_static_get, true, ClassData::ASYNC_ASYNC },
        { "post", s_static_post, true, ClassData::ASYNC_ASYNC },
        { "del", s_static_del, true, ClassData::ASYNC_ASYNC },
        { "put", s_static_put, true, ClassData::ASYNC_ASYNC },
        { "patch", s_static_patch, true, ClassData::ASYNC_ASYNC },
        { "head", s_static_head, true, ClassData::ASYNC_ASYNC }
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
        { "keepAlive", s_static_get_keepAlive, s_static_set_keepAlive, true },
        { "timeout", s_static_get_timeout, s_static_set_timeout, true },
        { "enableCookie", s_static_get_enableCookie, s_static_set_enableCookie, true },
        { "autoRedirect", s_static_get_autoRedirect, s_static_set_autoRedirect, true },
        { "enableEncoding", s_static_get_enableEncoding, s_static_set_enableEncoding, true },
        { "maxHeadersCount", s_static_get_maxHeadersCount, s_static_set_maxHeadersCount, true },
        { "maxHeaderSize", s_static_get_maxHeaderSize, s_static_set_maxHeaderSize, true },
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

inline void http_base::s_static_get_STATUS_CODES(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_STATUS_CODES(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get_cookies(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_cookies(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_keepAlive(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = set_keepAlive(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_timeout(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_timeout(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_enableCookie(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_enableCookie(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_enableCookie(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = set_enableCookie(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_autoRedirect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_autoRedirect(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_autoRedirect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = set_autoRedirect(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_enableEncoding(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = set_enableEncoding(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_maxHeadersCount(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_maxHeaderSize(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_maxHeaderSize(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_maxBodySize(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_userAgent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_userAgent(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_userAgent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_userAgent(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_poolSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_poolSize(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_poolSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_poolSize(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_poolTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_poolTimeout(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_poolTimeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_poolTimeout(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_http_proxy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_http_proxy(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_http_proxy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_http_proxy(v0);

    METHOD_VOID();
}

inline void http_base::s_static_get_https_proxy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_https_proxy(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_https_proxy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = set_https_proxy(v0);

    METHOD_VOID();
}

inline void http_base::s_static_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(bool, 1, false);

    hr = fileHandler(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_request(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, cb, args);
    else
        hr = ac_request(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);
    ARG(obj_ptr<SeekableStream_base>, 2);

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, v2, cb, args);
    else
        hr = ac_request(v0, v1, v2, vr);

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, v2, cb, args);
    else
        hr = ac_request(v0, v1, v2, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_request(v0, v1, cb, args);
    else
        hr = ac_request(v0, v1, vr);

    METHOD_OVER(1, 1);

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

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

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

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

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

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

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

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

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

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

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

    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_head(v0, v1, cb, args);
    else
        hr = ac_head(v0, v1, vr);

    METHOD_RETURN();
}
}
