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
#include "ifs/Message.h"

namespace fibjs {

class Message_base;
class Headers_base;
class Stream_base;

class HttpMessage_base : public Message_base {
    DECLARE_CLASS(HttpMessage_base);

public:
    // HttpMessage_base
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t set_protocol(exlib::string newVal) = 0;
    virtual result_t get_headers(obj_ptr<Headers_base>& retVal) = 0;
    virtual result_t get_keepAlive(bool& retVal) = 0;
    virtual result_t set_keepAlive(bool newVal) = 0;
    virtual result_t get_upgrade(bool& retVal) = 0;
    virtual result_t set_upgrade(bool newVal) = 0;
    virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
    virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
    virtual result_t get_maxHeaderSize(int32_t& retVal) = 0;
    virtual result_t set_maxHeaderSize(int32_t newVal) = 0;
    virtual result_t get_maxChunkSize(int32_t& retVal) = 0;
    virtual result_t set_maxChunkSize(int32_t newVal) = 0;
    virtual result_t get_maxBodySize(int32_t& retVal) = 0;
    virtual result_t set_maxBodySize(int32_t newVal) = 0;
    virtual result_t get_socket(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t hasHeader(exlib::string name, bool& retVal) = 0;
    virtual result_t firstHeader(exlib::string name, exlib::string& retVal) = 0;
    virtual result_t allHeader(exlib::string name, obj_ptr<NObject>& retVal) = 0;
    virtual result_t appendHeader(v8::Local<v8::Object> map) = 0;
    virtual result_t appendHeader(Headers_base* headers) = 0;
    virtual result_t appendHeader(exlib::string name, v8::Local<v8::Array> values) = 0;
    virtual result_t appendHeader(exlib::string name, exlib::string value) = 0;
    virtual result_t setHeader(v8::Local<v8::Object> map) = 0;
    virtual result_t setHeader(Headers_base* headers) = 0;
    virtual result_t setHeader(exlib::string name, v8::Local<v8::Array> values) = 0;
    virtual result_t setHeader(exlib::string name, exlib::string value) = 0;
    virtual result_t removeHeader(exlib::string name) = 0;
    virtual result_t getHeader(exlib::string name, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t getHeaders(obj_ptr<NObject>& retVal) = 0;
    virtual result_t get_headersSent(bool& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<HttpMessage_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

public:
    static void s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_protocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_headers(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_hasHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_firstHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_allHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_appendHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_removeHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getHeader(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getHeaders(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_headersSent(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Headers.h"
#include "ifs/Stream.h"

namespace fibjs {
inline ClassInfo& HttpMessage_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "hasHeader", s_hasHeader, false, ClassData::ASYNC_SYNC },
        { "firstHeader", s_firstHeader, false, ClassData::ASYNC_SYNC },
        { "allHeader", s_allHeader, false, ClassData::ASYNC_SYNC },
        { "appendHeader", s_appendHeader, false, ClassData::ASYNC_SYNC },
        { "setHeader", s_setHeader, false, ClassData::ASYNC_SYNC },
        { "removeHeader", s_removeHeader, false, ClassData::ASYNC_SYNC },
        { "getHeader", s_getHeader, false, ClassData::ASYNC_SYNC },
        { "getHeaders", s_getHeaders, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "protocol", s_get_protocol, s_set_protocol, false },
        { "headers", s_get_headers, block_set, false },
        { "keepAlive", s_get_keepAlive, s_set_keepAlive, false },
        { "upgrade", s_get_upgrade, s_set_upgrade, false },
        { "maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false },
        { "maxHeaderSize", s_get_maxHeaderSize, s_set_maxHeaderSize, false },
        { "maxChunkSize", s_get_maxChunkSize, s_set_maxChunkSize, false },
        { "maxBodySize", s_get_maxBodySize, s_set_maxBodySize, false },
        { "socket", s_get_socket, block_set, false },
        { "headersSent", s_get_headersSent, block_set, false }
    };

    static ClassData s_cd = {
        "HttpMessage", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Message_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpMessage_base::s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_protocol(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_protocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_protocol(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_headers(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Headers_base> vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_headers(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_get_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_keepAlive(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_keepAlive(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_upgrade(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_upgrade(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_upgrade(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxHeadersCount(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxHeaderSize(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxHeaderSize(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxChunkSize(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_maxChunkSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxChunkSize(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxBodySize(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_socket(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_hasHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->hasHeader(v0, vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_firstHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->firstHeader(v0, vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_allHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "");

    hr = pInst->allHeader(v0, vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_appendHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->appendHeader(v0);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Headers_base>, 0);

    hr = pInst->appendHeader(v0.get());

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->appendHeader(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->appendHeader(v0, v1);

    METHOD_VOID();
}

inline void HttpMessage_base::s_setHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->setHeader(v0);

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Headers_base>, 0);

    hr = pInst->setHeader(v0.get());

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(v8::Local<v8::Array>, 1);

    hr = pInst->setHeader(v0, v1);

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(exlib::string, 1);

    hr = pInst->setHeader(v0, v1);

    METHOD_VOID();
}

inline void HttpMessage_base::s_removeHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->removeHeader(v0);

    METHOD_VOID();
}

inline void HttpMessage_base::s_getHeader(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->getHeader(v0, vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_getHeaders(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getHeaders(vr);

    METHOD_RETURN();
}

inline void HttpMessage_base::s_get_headersSent(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpMessage_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_headersSent(vr);

    METHOD_RETURN();
}
}
