/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpResponse_base_H_
#define _HttpResponse_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "HttpMessage.h"

namespace fibjs {

class HttpMessage_base;
class HttpCookie_base;
class Stream_base;

class HttpResponse_base : public HttpMessage_base {
    DECLARE_CLASS(HttpResponse_base);

public:
    // HttpResponse_base
    static result_t _new(obj_ptr<HttpResponse_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_statusCode(int32_t& retVal) = 0;
    virtual result_t set_statusCode(int32_t newVal) = 0;
    virtual result_t get_statusMessage(exlib::string& retVal) = 0;
    virtual result_t set_statusMessage(exlib::string newVal) = 0;
    virtual result_t writeHead(int32_t statusCode, exlib::string statusMessage, v8::Local<v8::Object> headers) = 0;
    virtual result_t writeHead(int32_t statusCode, v8::Local<v8::Object> headers) = 0;
    virtual result_t get_cookies(obj_ptr<NArray>& retVal) = 0;
    virtual result_t addCookie(HttpCookie_base* cookie) = 0;
    virtual result_t redirect(exlib::string url) = 0;
    virtual result_t sendHeader(Stream_base* stm, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_statusCode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_statusCode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_statusMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_statusMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_writeHead(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_cookies(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_addCookie(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_redirect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sendHeader(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER1(HttpResponse_base, sendHeader, Stream_base*);
};
}

#include "HttpCookie.h"
#include "Stream.h"

namespace fibjs {
inline ClassInfo& HttpResponse_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "writeHead", s_writeHead, false },
        { "addCookie", s_addCookie, false },
        { "redirect", s_redirect, false },
        { "sendHeader", s_sendHeader, false },
        { "sendHeaderSync", s_sendHeader, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "statusCode", s_get_statusCode, s_set_statusCode, false },
        { "statusMessage", s_get_statusMessage, s_set_statusMessage, false },
        { "cookies", s_get_cookies, block_set, false }
    };

    static ClassData s_cd = {
        "HttpResponse", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &HttpMessage_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpResponse_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpResponse_base::__new(const T& args)
{
    obj_ptr<HttpResponse_base> vr;

    METHOD_NAME("new HttpResponse()");
    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpResponse_base::s_get_statusCode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpResponse.statusCode");
    METHOD_INSTANCE(HttpResponse_base);
    PROPERTY_ENTER();

    hr = pInst->get_statusCode(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_set_statusCode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpResponse.statusCode");
    METHOD_INSTANCE(HttpResponse_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_statusCode(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpResponse_base::s_get_statusMessage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpResponse.statusMessage");
    METHOD_INSTANCE(HttpResponse_base);
    PROPERTY_ENTER();

    hr = pInst->get_statusMessage(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_set_statusMessage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpResponse.statusMessage");
    METHOD_INSTANCE(HttpResponse_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_statusMessage(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpResponse_base::s_writeHead(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpResponse.writeHead");
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(int32_t, 0);
    ARG(exlib::string, 1);
    OPT_ARG(v8::Local<v8::Object>, 2, v8::Object::New(isolate));

    hr = pInst->writeHead(v0, v1, v2);

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate));

    hr = pInst->writeHead(v0, v1);

    METHOD_VOID();
}

inline void HttpResponse_base::s_get_cookies(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("HttpResponse.cookies");
    METHOD_INSTANCE(HttpResponse_base);
    PROPERTY_ENTER();

    hr = pInst->get_cookies(vr);

    METHOD_RETURN();
}

inline void HttpResponse_base::s_addCookie(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpResponse.addCookie");
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<HttpCookie_base>, 0);

    hr = pInst->addCookie(v0);

    METHOD_VOID();
}

inline void HttpResponse_base::s_redirect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpResponse.redirect");
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->redirect(v0);

    METHOD_VOID();
}

inline void HttpResponse_base::s_sendHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpResponse.sendHeader");
    METHOD_INSTANCE(HttpResponse_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_sendHeader(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_sendHeader(v0);

    METHOD_VOID();
}
}

#endif
