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

class Headers_base;
class Stream_base;
class Blob_base;
class Buffer_base;

class WebResponse_base : public object_base {
    DECLARE_CLASS(WebResponse_base);

public:
    // WebResponse_base
    static result_t _new(v8::Local<v8::Value> body, v8::Local<v8::Object> options, obj_ptr<WebResponse_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, obj_ptr<WebResponse_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_status(int32_t& retVal) = 0;
    virtual result_t get_statusText(exlib::string& retVal) = 0;
    virtual result_t get_ok(bool& retVal) = 0;
    virtual result_t get_type(exlib::string& retVal) = 0;
    virtual result_t get_url(exlib::string& retVal) = 0;
    virtual result_t get_redirected(bool& retVal) = 0;
    virtual result_t get_bodyUsed(bool& retVal) = 0;
    virtual result_t get_headers(obj_ptr<Headers_base>& retVal) = 0;
    virtual result_t get_body(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t text(exlib::string& retVal, AsyncEvent* ac) = 0;
    virtual result_t json(Variant& retVal, AsyncEvent* ac) = 0;
    virtual result_t arrayBuffer(std::shared_ptr<v8::BackingStore>& retVal, AsyncEvent* ac) = 0;
    virtual result_t blob(obj_ptr<Blob_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t bytes(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t clone(obj_ptr<WebResponse_base>& retVal) = 0;
    static result_t json(v8::Local<v8::Value> data, v8::Local<v8::Object> options, obj_ptr<WebResponse_base>& retVal);
    static result_t redirect(exlib::string url, int32_t status, obj_ptr<WebResponse_base>& retVal);
    static result_t error(obj_ptr<WebResponse_base>& retVal);

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<WebResponse_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_status(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_statusText(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_ok(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_redirected(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_bodyUsed(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_headers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_body(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_text(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_arrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_blob(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bytes(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_clone(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_json(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_redirect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE1(WebResponse_base, text, exlib::string);
    ASYNC_MEMBERVALUE1(WebResponse_base, json, Variant);
    ASYNC_MEMBERVALUE1(WebResponse_base, arrayBuffer, std::shared_ptr<v8::BackingStore>);
    ASYNC_MEMBERVALUE1(WebResponse_base, blob, obj_ptr<Blob_base>);
    ASYNC_MEMBERVALUE1(WebResponse_base, bytes, obj_ptr<Buffer_base>);
};
}

#include "ifs/Headers.h"
#include "ifs/Stream.h"
#include "ifs/Blob.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& WebResponse_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "text", s_text, false, ClassData::ASYNC_PROMISE },
        { "json", s_json, false, ClassData::ASYNC_PROMISE },
        { "arrayBuffer", s_arrayBuffer, false, ClassData::ASYNC_PROMISE },
        { "blob", s_blob, false, ClassData::ASYNC_PROMISE },
        { "bytes", s_bytes, false, ClassData::ASYNC_PROMISE },
        { "clone", s_clone, false, ClassData::ASYNC_SYNC },
        { "json", s_static_json, true, ClassData::ASYNC_SYNC },
        { "redirect", s_static_redirect, true, ClassData::ASYNC_SYNC },
        { "error", s_static_error, true, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "status", s_get_status, block_set, false },
        { "statusText", s_get_statusText, block_set, false },
        { "ok", s_get_ok, block_set, false },
        { "type", s_get_type, block_set, false },
        { "url", s_get_url, block_set, false },
        { "redirected", s_get_redirected, block_set, false },
        { "bodyUsed", s_get_bodyUsed, block_set, false },
        { "headers", s_get_headers, block_set, false },
        { "body", s_get_body, block_set, false }
    };

    static ClassData s_cd = {
        "WebResponse", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void WebResponse_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void WebResponse_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebResponse_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t WebResponse_base::load(v8::Local<v8::Value> v, obj_ptr<WebResponse_base>& retVal)
{
    obj_ptr<WebResponse_base> vr;

    LOAD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void WebResponse_base::s_get_status(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_status(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_statusText(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_statusText(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_ok(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_ok(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_url(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_url(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_redirected(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_redirected(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_bodyUsed(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_bodyUsed(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_headers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Headers_base> vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_headers(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_get_body(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_body(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_text(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    ASYNC_METHOD_INSTANCE(WebResponse_base);
    ASYNC_METHOD_ENTER("WebResponse.text");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_text(cb, args);
    else
        hr = pInst->ac_text(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant vr;

    ASYNC_METHOD_INSTANCE(WebResponse_base);
    ASYNC_METHOD_ENTER("WebResponse.json");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_json(cb, args);
    else
        hr = pInst->ac_json(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_arrayBuffer(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    std::shared_ptr<v8::BackingStore> vr;

    ASYNC_METHOD_INSTANCE(WebResponse_base);
    ASYNC_METHOD_ENTER("WebResponse.arrayBuffer");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_arrayBuffer(cb, args);
    else
        hr = pInst->ac_arrayBuffer(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_blob(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Blob_base> vr;

    ASYNC_METHOD_INSTANCE(WebResponse_base);
    ASYNC_METHOD_ENTER("WebResponse.blob");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_blob(cb, args);
    else
        hr = pInst->ac_blob(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_bytes(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(WebResponse_base);
    ASYNC_METHOD_ENTER("WebResponse.bytes");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_bytes(cb, args);
    else
        hr = pInst->ac_bytes(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_clone(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebResponse_base> vr;

    METHOD_INSTANCE(WebResponse_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->clone(vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_static_json(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebResponse_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Value>, 0);
    OPT_ARG(v8::Local<v8::Object>, 1, v8::Object::New(isolate->m_isolate));

    hr = json(v0, v1, vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_static_redirect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebResponse_base> vr;

    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 302);

    hr = redirect(v0, v1, vr);

    METHOD_RETURN();
}

inline void WebResponse_base::s_static_error(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<WebResponse_base> vr;

    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = error(vr);

    METHOD_RETURN();
}
}
