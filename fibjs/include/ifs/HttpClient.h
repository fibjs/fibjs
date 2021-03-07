/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpClient_base_H_
#define _HttpClient_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class X509Cert_base;
class PKey_base;
class HttpResponse_base;
class Stream_base;
class HttpRequest_base;
class SeekableStream_base;

class HttpClient_base : public object_base {
    DECLARE_CLASS(HttpClient_base);

public:
    // HttpClient_base
    static result_t _new(obj_ptr<HttpClient_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_cookies(obj_ptr<NArray>& retVal) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t get_enableCookie(bool& retVal) = 0;
    virtual result_t set_enableCookie(bool newVal) = 0;
    virtual result_t get_autoRedirect(bool& retVal) = 0;
    virtual result_t set_autoRedirect(bool newVal) = 0;
    virtual result_t get_enableEncoding(bool& retVal) = 0;
    virtual result_t set_enableEncoding(bool newVal) = 0;
    virtual result_t get_maxBodySize(int32_t& retVal) = 0;
    virtual result_t set_maxBodySize(int32_t newVal) = 0;
    virtual result_t get_userAgent(exlib::string& retVal) = 0;
    virtual result_t set_userAgent(exlib::string newVal) = 0;
    virtual result_t get_poolSize(int32_t& retVal) = 0;
    virtual result_t set_poolSize(int32_t newVal) = 0;
    virtual result_t get_poolTimeout(int32_t& retVal) = 0;
    virtual result_t set_poolTimeout(int32_t newVal) = 0;
    virtual result_t get_proxyAgent(exlib::string& retVal) = 0;
    virtual result_t set_proxyAgent(exlib::string newVal) = 0;
    virtual result_t get_sslVerification(int32_t& retVal) = 0;
    virtual result_t set_sslVerification(int32_t newVal) = 0;
    virtual result_t setClientCert(X509Cert_base* crt, PKey_base* key) = 0;
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t request(Stream_base* conn, HttpRequest_base* req, SeekableStream_base* response_body, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t request(exlib::string method, exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t get(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t post(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t del(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t put(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t patch(exlib::string url, v8::Local<v8::Object> opts, obj_ptr<HttpResponse_base>& retVal, AsyncEvent* ac) = 0;

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
    static void s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_userAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_userAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_poolSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_poolSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_poolTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_poolTimeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_proxyAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_proxyAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_sslVerification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_sslVerification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_request(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_put(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_patch(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(HttpClient_base, request, Stream_base*, HttpRequest_base*, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE4(HttpClient_base, request, Stream_base*, HttpRequest_base*, SeekableStream_base*, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE4(HttpClient_base, request, exlib::string, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, get, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, post, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, del, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, put, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
    ASYNC_MEMBERVALUE3(HttpClient_base, patch, exlib::string, v8::Local<v8::Object>, obj_ptr<HttpResponse_base>);
};
}

#include "ifs/X509Cert.h"
#include "ifs/PKey.h"
#include "ifs/HttpResponse.h"
#include "ifs/Stream.h"
#include "ifs/HttpRequest.h"
#include "ifs/SeekableStream.h"

namespace fibjs {
inline ClassInfo& HttpClient_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "setClientCert", s_setClientCert, false },
        { "request", s_request, false },
        { "requestSync", s_request, false },
        { "get", s_get, false },
        { "getSync", s_get, false },
        { "post", s_post, false },
        { "postSync", s_post, false },
        { "del", s_del, false },
        { "delSync", s_del, false },
        { "put", s_put, false },
        { "putSync", s_put, false },
        { "patch", s_patch, false },
        { "patchSync", s_patch, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "cookies", s_get_cookies, block_set, false },
        { "timeout", s_get_timeout, s_set_timeout, false },
        { "enableCookie", s_get_enableCookie, s_set_enableCookie, false },
        { "autoRedirect", s_get_autoRedirect, s_set_autoRedirect, false },
        { "enableEncoding", s_get_enableEncoding, s_set_enableEncoding, false },
        { "maxBodySize", s_get_maxBodySize, s_set_maxBodySize, false },
        { "userAgent", s_get_userAgent, s_set_userAgent, false },
        { "poolSize", s_get_poolSize, s_set_poolSize, false },
        { "poolTimeout", s_get_poolTimeout, s_set_poolTimeout, false },
        { "proxyAgent", s_get_proxyAgent, s_set_proxyAgent, false },
        { "sslVerification", s_get_sslVerification, s_set_sslVerification, false }
    };

    static ClassData s_cd = {
        "HttpClient", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info()
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

    METHOD_NAME("new HttpClient()");
    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpClient_base::s_get_cookies(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("HttpClient.cookies");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_cookies(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_get_timeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpClient.timeout");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_timeout(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_timeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.timeout");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_timeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_enableCookie(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("HttpClient.enableCookie");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_enableCookie(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_enableCookie(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.enableCookie");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_enableCookie(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_autoRedirect(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("HttpClient.autoRedirect");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_autoRedirect(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_autoRedirect(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.autoRedirect");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_autoRedirect(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("HttpClient.enableEncoding");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_enableEncoding(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.enableEncoding");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_enableEncoding(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpClient.maxBodySize");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.maxBodySize");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxBodySize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_userAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpClient.userAgent");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_userAgent(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_userAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.userAgent");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_userAgent(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_poolSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpClient.poolSize");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_poolSize(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_poolSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.poolSize");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_poolSize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_poolTimeout(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpClient.poolTimeout");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_poolTimeout(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_poolTimeout(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.poolTimeout");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_poolTimeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_proxyAgent(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpClient.proxyAgent");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_proxyAgent(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_proxyAgent(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.proxyAgent");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_proxyAgent(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_get_sslVerification(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpClient.sslVerification");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();

    hr = pInst->get_sslVerification(vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_set_sslVerification(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpClient.sslVerification");
    METHOD_INSTANCE(HttpClient_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_sslVerification(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpClient_base::s_setClientCert(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpClient.setClientCert");
    METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<X509Cert_base>, 0);
    ARG(obj_ptr<PKey_base>, 1);

    hr = pInst->setClientCert(v0, v1);

    METHOD_VOID();
}

inline void HttpClient_base::s_request(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("HttpClient.request");
    METHOD_INSTANCE(HttpClient_base);
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
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_request(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_request(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("HttpClient.get");
    METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_get(v0, v1, cb, args);
    else
        hr = pInst->ac_get(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("HttpClient.post");
    METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_post(v0, v1, cb, args);
    else
        hr = pInst->ac_post(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("HttpClient.del");
    METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_del(v0, v1, cb, args);
    else
        hr = pInst->ac_del(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_put(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("HttpClient.put");
    METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_put(v0, v1, cb, args);
    else
        hr = pInst->ac_put(v0, v1, vr);

    METHOD_RETURN();
}

inline void HttpClient_base::s_patch(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("HttpClient.patch");
    METHOD_INSTANCE(HttpClient_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    if (!cb.IsEmpty())
        hr = pInst->acb_patch(v0, v1, cb, args);
    else
        hr = pInst->ac_patch(v0, v1, vr);

    METHOD_RETURN();
}
}

#endif
