/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _http_base_H_
#define _http_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class HttpRequest_base;
class HttpResponse_base;
class HttpCookie_base;
class HttpServer_base;
class HttpClient_base;
class HttpsServer_base;
class HttpHandler_base;
class List_base;
class Handler_base;
class Stream_base;
class SeekableStream_base;
class Map_base;
class Buffer_base;

class http_base : public object_base
{
    DECLARE_CLASS(http_base);

public:
    // http_base
    static result_t get_cookies(obj_ptr<List_base>& retVal);
    static result_t get_timeout(int32_t& retVal);
    static result_t set_timeout(int32_t newVal);
    static result_t get_enableCookie(bool& retVal);
    static result_t set_enableCookie(bool newVal);
    static result_t get_autoRedirect(bool& retVal);
    static result_t set_autoRedirect(bool newVal);
    static result_t get_userAgent(exlib::string& retVal);
    static result_t set_userAgent(exlib::string newVal);
    static result_t fileHandler(exlib::string root, v8::Local<v8::Object> mimes, obj_ptr<Handler_base>& retVal);
    static result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
    static result_t request(exlib::string method, exlib::string url, SeekableStream_base* body, Map_base* headers, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac);
    static result_t request(exlib::string method, exlib::string url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
    static result_t request(exlib::string method, exlib::string url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
    static result_t get(exlib::string url, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
    static result_t post(exlib::string url, SeekableStream_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);
    static result_t post(exlib::string url, Buffer_base* body, v8::Local<v8::Object> headers, obj_ptr<HttpResponse_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_get_cookies(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_enableCookie(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_enableCookie(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_autoRedirect(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_autoRedirect(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_get_userAgent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void s_set_userAgent(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
    static void s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_STATICVALUE3(http_base, request, Stream_base*, HttpRequest_base*, obj_ptr<HttpResponse_base>);
    ASYNC_STATICVALUE5(http_base, request, exlib::string, exlib::string, SeekableStream_base*, Map_base*, obj_ptr<HttpResponse_base>);
};

}

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpCookie.h"
#include "HttpServer.h"
#include "HttpClient.h"
#include "HttpsServer.h"
#include "HttpHandler.h"
#include "List.h"
#include "Handler.h"
#include "Stream.h"
#include "SeekableStream.h"
#include "Map.h"
#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& http_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"fileHandler", s_fileHandler, true},
            {"request", s_request, true},
            {"get", s_get, true},
            {"post", s_post, true}
        };

        static ClassData::ClassObject s_object[] = 
        {
            {"Request", HttpRequest_base::class_info},
            {"Response", HttpResponse_base::class_info},
            {"Cookie", HttpCookie_base::class_info},
            {"Server", HttpServer_base::class_info},
            {"Client", HttpClient_base::class_info},
            {"HttpsServer", HttpsServer_base::class_info},
            {"Handler", HttpHandler_base::class_info}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"cookies", s_get_cookies, block_set, true},
            {"timeout", s_get_timeout, s_set_timeout, true},
            {"enableCookie", s_get_enableCookie, s_set_enableCookie, true},
            {"autoRedirect", s_get_autoRedirect, s_set_autoRedirect, true},
            {"userAgent", s_get_userAgent, s_set_userAgent, true}
        };

        static ClassData s_cd = 
        { 
            "http", s__new, NULL, 
            4, s_method, 7, s_object, 5, s_property, NULL, NULL,
            NULL
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void http_base::s_get_cookies(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<List_base> vr;

        PROPERTY_ENTER();

        hr = get_cookies(vr);

        METHOD_RETURN();
    }

    inline void http_base::s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        PROPERTY_ENTER();

        hr = get_timeout(vr);

        METHOD_RETURN();
    }

    inline void http_base::s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(int32_t);

        hr = set_timeout(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void http_base::s_get_enableCookie(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        PROPERTY_ENTER();

        hr = get_enableCookie(vr);

        METHOD_RETURN();
    }

    inline void http_base::s_set_enableCookie(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = set_enableCookie(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void http_base::s_get_autoRedirect(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        bool vr;

        PROPERTY_ENTER();

        hr = get_autoRedirect(vr);

        METHOD_RETURN();
    }

    inline void http_base::s_set_autoRedirect(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(bool);

        hr = set_autoRedirect(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void http_base::s_get_userAgent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        exlib::string vr;

        PROPERTY_ENTER();

        hr = get_userAgent(vr);

        METHOD_RETURN();
    }

    inline void http_base::s_set_userAgent(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args)
    {
        PROPERTY_ENTER();
        PROPERTY_VAL(exlib::string);

        hr = set_userAgent(v0);

        PROPERTY_SET_LEAVE();
    }

    inline void http_base::s_fileHandler(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Handler_base> vr;

        METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

        hr = fileHandler(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void http_base::s_request(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HttpResponse_base> vr;

        ASYNC_METHOD_ENTER(2, 2);

        ARG(obj_ptr<Stream_base>, 0);
        ARG(obj_ptr<HttpRequest_base>, 1);

        if(!cb.IsEmpty()) {
            acb_request(v0, v1, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_request(v0, v1, vr);

        METHOD_OVER(3, 2);

        ARG(exlib::string, 0);
        ARG(exlib::string, 1);
        OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

        hr = request(v0, v1, v2, vr);

        METHOD_OVER(4, 4);

        ARG(exlib::string, 0);
        ARG(exlib::string, 1);
        ARG(obj_ptr<SeekableStream_base>, 2);
        ARG(obj_ptr<Map_base>, 3);

        if(!cb.IsEmpty()) {
            acb_request(v0, v1, v2, v3, vr, cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = ac_request(v0, v1, v2, v3, vr);

        METHOD_OVER(4, 3);

        ARG(exlib::string, 0);
        ARG(exlib::string, 1);
        ARG(obj_ptr<SeekableStream_base>, 2);
        OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

        hr = request(v0, v1, v2, v3, vr);

        METHOD_OVER(4, 3);

        ARG(exlib::string, 0);
        ARG(exlib::string, 1);
        ARG(obj_ptr<Buffer_base>, 2);
        OPT_ARG(v8::Local<v8::Object>, 3, v8::Object::New(isolate));

        hr = request(v0, v1, v2, v3, vr);

        METHOD_RETURN();
    }

    inline void http_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HttpResponse_base> vr;

        METHOD_ENTER(2, 1);

        ARG(exlib::string, 0);
        OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

        hr = get(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void http_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<HttpResponse_base> vr;

        METHOD_ENTER(3, 2);

        ARG(exlib::string, 0);
        ARG(obj_ptr<SeekableStream_base>, 1);
        OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

        hr = post(v0, v1, v2, vr);

        METHOD_OVER(3, 2);

        ARG(exlib::string, 0);
        ARG(obj_ptr<Buffer_base>, 1);
        OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

        hr = post(v0, v1, v2, vr);

        METHOD_RETURN();
    }

}

#endif

