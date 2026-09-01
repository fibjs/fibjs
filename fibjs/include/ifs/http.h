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
class Headers_base;
class HttpCookie_base;
class HttpServer_base;
class HttpClient_base;
class HttpsServer_base;
class Handler_base;
class SecureContext_base;
class HttpHandler_base;
class HttpRepeater_base;
class Stream_base;

class http_base : public object_base {
    DECLARE_CLASS(http_base);

public:
    // http_base
    static result_t createServer(Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal);
    static result_t createServer(SecureContext_base* context, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal);
    static result_t createServer(v8::Local<v8::Object> options, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal);
    static result_t get_STATUS_CODES(v8::Local<v8::Object>& retVal);
    static result_t get_METHODS(v8::Local<v8::Array>& retVal);
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
    static result_t get_enableH2(bool& retVal);
    static result_t set_enableH2(bool newVal);
    static result_t get_maxHeadersCount(int32_t& retVal);
    static result_t set_maxHeadersCount(int32_t newVal);
    static result_t get_maxHeaderSize(int32_t& retVal);
    static result_t set_maxHeaderSize(int32_t newVal);
    static result_t get_maxChunkSize(int32_t& retVal);
    static result_t set_maxChunkSize(int32_t newVal);
    static result_t get_maxBodySize(int32_t& retVal);
    static result_t set_maxBodySize(int32_t newVal);
    static result_t get_userAgent(exlib::string& retVal);
    static result_t set_userAgent(exlib::string newVal);
    static result_t get_poolTimeout(int32_t& retVal);
    static result_t set_poolTimeout(int32_t newVal);
    static result_t get_maxFreeSockets(int32_t& retVal);
    static result_t set_maxFreeSockets(int32_t newVal);
    static result_t fileHandler(exlib::string root, bool autoIndex, obj_ptr<Handler_base>& retVal);
    static result_t fileHandler(exlib::string root, v8::Local<v8::Object> options, obj_ptr<Handler_base>& retVal);
    static result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t request(exlib::string method, exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t requestSync(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t requestSync(v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t requestSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t getSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t get(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t get(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t postSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t post(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t post(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t delSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t del(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t del(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t putSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t put(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t put(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t patchSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t patch(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t patch(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t headSync(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t head(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpRequest_base>& retVal);
    static result_t head(exlib::string url, v8::Local<v8::Object> opts, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t head(exlib::string url, v8::Local<v8::Function> callback, obj_ptr<HttpRequest_base>& retVal);
    static result_t setGlobalProxyFromEnv(v8::Local<v8::Object> proxyEnv, v8::Local<v8::Function>& retVal);
    static result_t fetch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t fetch(HttpRequest_base* request, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<http_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_static_createServer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_STATUS_CODES(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_METHODS(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void s_static_get_enableH2(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_enableH2(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_userAgent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_userAgent(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_poolTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_poolTimeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_set_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_requestSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_getSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_postSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_post(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_delSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_putSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_put(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_patchSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_patch(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_headSync(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_head(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_setGlobalProxyFromEnv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_fetch(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE4(http_base, requestSync, exlib::string, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE2(http_base, requestSync, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, requestSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, getSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, postSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, delSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, putSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, patchSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, headSync, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, fetch, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE3(http_base, fetch, HttpRequest_base*, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
};
}

#include "ifs/HttpRequest.h"
#include "ifs/HttpResponse.h"
#include "ifs/Headers.h"
#include "ifs/HttpCookie.h"
#include "ifs/HttpServer.h"
#include "ifs/HttpClient.h"
#include "ifs/HttpsServer.h"
#include "ifs/Handler.h"
#include "ifs/SecureContext.h"
#include "ifs/HttpHandler.h"
#include "ifs/HttpRepeater.h"
#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& http_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "createServer", s_static_createServer, true, ClassData::ASYNC_SYNC },
        { "fileHandler", s_static_fileHandler, true, ClassData::ASYNC_SYNC },
        { "request", s_static_request, true, ClassData::ASYNC_SYNC },
        { "requestSync", s_static_requestSync, true, ClassData::ASYNC_ASYNC },
        { "getSync", s_static_getSync, true, ClassData::ASYNC_ASYNC },
        { "get", s_static_get, true, ClassData::ASYNC_SYNC },
        { "postSync", s_static_postSync, true, ClassData::ASYNC_ASYNC },
        { "post", s_static_post, true, ClassData::ASYNC_SYNC },
        { "delSync", s_static_delSync, true, ClassData::ASYNC_ASYNC },
        { "del", s_static_del, true, ClassData::ASYNC_SYNC },
        { "putSync", s_static_putSync, true, ClassData::ASYNC_ASYNC },
        { "put", s_static_put, true, ClassData::ASYNC_SYNC },
        { "patchSync", s_static_patchSync, true, ClassData::ASYNC_ASYNC },
        { "patch", s_static_patch, true, ClassData::ASYNC_SYNC },
        { "headSync", s_static_headSync, true, ClassData::ASYNC_ASYNC },
        { "head", s_static_head, true, ClassData::ASYNC_SYNC },
        { "setGlobalProxyFromEnv", s_static_setGlobalProxyFromEnv, true, ClassData::ASYNC_SYNC },
        { "fetch", s_static_fetch, true, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassObject s_object[] = {
        { "Request", HttpRequest_base::class_info },
        { "IncomingMessage", HttpRequest_base::class_info },
        { "Response", HttpResponse_base::class_info },
        { "ServerResponse", HttpResponse_base::class_info },
        { "Headers", Headers_base::class_info },
        { "Cookie", HttpCookie_base::class_info },
        { "Server", HttpServer_base::class_info },
        { "Client", HttpClient_base::class_info },
        { "Agent", HttpClient_base::class_info },
        { "HttpsServer", HttpsServer_base::class_info },
        { "Handler", HttpHandler_base::class_info },
        { "Repeater", HttpRepeater_base::class_info }
    };

    static ClassData::ClassProperty s_property[] = {
        { "STATUS_CODES", s_static_get_STATUS_CODES, block_set, true },
        { "METHODS", s_static_get_METHODS, block_set, true },
        { "cookies", s_static_get_cookies, block_set, true },
        { "keepAlive", s_static_get_keepAlive, s_static_set_keepAlive, true },
        { "timeout", s_static_get_timeout, s_static_set_timeout, true },
        { "enableCookie", s_static_get_enableCookie, s_static_set_enableCookie, true },
        { "autoRedirect", s_static_get_autoRedirect, s_static_set_autoRedirect, true },
        { "enableEncoding", s_static_get_enableEncoding, s_static_set_enableEncoding, true },
        { "enableH2", s_static_get_enableH2, s_static_set_enableH2, true },
        { "maxHeadersCount", s_static_get_maxHeadersCount, s_static_set_maxHeadersCount, true },
        { "maxHeaderSize", s_static_get_maxHeaderSize, s_static_set_maxHeaderSize, true },
        { "maxChunkSize", s_static_get_maxChunkSize, s_static_set_maxChunkSize, true },
        { "maxBodySize", s_static_get_maxBodySize, s_static_set_maxBodySize, true },
        { "userAgent", s_static_get_userAgent, s_static_set_userAgent, true },
        { "poolTimeout", s_static_get_poolTimeout, s_static_set_poolTimeout, true },
        { "maxFreeSockets", s_static_get_maxFreeSockets, s_static_set_maxFreeSockets, true }
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

inline void http_base::s_static_createServer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpServer_base> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = createServer(v0.get(), vr);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<SecureContext_base>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = createServer(v0.get(), v1.get(), vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = createServer(v0, v1.get(), vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get_STATUS_CODES(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_STATUS_CODES(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_get_METHODS(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Array> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_METHODS(vr);

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

inline void http_base::s_static_get_enableH2(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_enableH2(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_enableH2(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = set_enableH2(v0);

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

inline void http_base::s_static_get_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_maxChunkSize(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_maxChunkSize(v0);

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

inline void http_base::s_static_get_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = get_maxFreeSockets(vr);

    METHOD_RETURN();
}

inline void http_base::s_static_set_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = set_maxFreeSockets(v0);

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

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = fileHandler(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_request(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Stream_base>, 0);
    ARG(obj_ptr<HttpRequest_base>, 1);

    hr = request(v0.get(), v1.get(), vr);

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    hr = request(v0, v1, v2, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = request(v0, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = request(v0, v1, vr);

    METHOD_OVER(4, 4);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Object>, 2);
    ARG(v8::Local<v8::Function>, 3);

    hr = request(v0, v1, v2, v3, vr);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = request(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = request(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = request(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = request(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_requestSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.requestSync");

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_requestSync(v0, v1, v2, cb, args);
    else
        hr = ac_requestSync(v0, v1, v2, vr);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = acb_requestSync(v0, cb, args);
    else
        hr = ac_requestSync(v0, vr);

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_requestSync(v0, v1, cb, args);
    else
        hr = ac_requestSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_getSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.getSync");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_getSync(v0, v1, cb, args);
    else
        hr = ac_getSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = get(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = get(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = get(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_postSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.postSync");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_postSync(v0, v1, cb, args);
    else
        hr = ac_postSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_post(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = post(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = post(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = post(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_delSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.delSync");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_delSync(v0, v1, cb, args);
    else
        hr = ac_delSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = del(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = del(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = del(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_putSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.putSync");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_putSync(v0, v1, cb, args);
    else
        hr = ac_putSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_put(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = put(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = put(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = put(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_patchSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.patchSync");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_patchSync(v0, v1, cb, args);
    else
        hr = ac_patchSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_patch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = patch(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = patch(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = patch(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_headSync(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.headSync");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_headSync(v0, v1, cb, args);
    else
        hr = ac_headSync(v0, v1, vr);

    ASYNC_METHOD_RETURN();
}

inline void http_base::s_static_head(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpRequest_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = head(v0, v1, vr);

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = head(v0, v1, v2, vr);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = head(v0, v1, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_setGlobalProxyFromEnv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = setGlobalProxyFromEnv(v0, vr);

    METHOD_RETURN();
}

inline void http_base::s_static_fetch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    ASYNC_METHOD_ENTER("http.fetch");

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_fetch(v0, v1, cb, args);
    else
        hr = ac_fetch(v0, v1, vr);

    METHOD_OVER(2, 1);

    ARG(obj_ptr<HttpRequest_base>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    if (!cb.IsEmpty())
        hr = acb_fetch(v0.get(), v1, cb, args);
    else
        hr = ac_fetch(v0.get(), v1, vr);

    ASYNC_METHOD_RETURN();
}
}
