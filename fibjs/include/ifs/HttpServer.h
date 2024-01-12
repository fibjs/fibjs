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
#include "ifs/TcpServer.h"

namespace fibjs {

class TcpServer_base;
class Handler_base;

class HttpServer_base : public TcpServer_base {
    DECLARE_CLASS(HttpServer_base);

public:
    // HttpServer_base
    static result_t _new(int32_t port, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, int32_t port, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t enableCrossOrigin(exlib::string allowHeaders) = 0;
    virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
    virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
    virtual result_t get_maxHeaderSize(int32_t& retVal) = 0;
    virtual result_t set_maxHeaderSize(int32_t newVal) = 0;
    virtual result_t get_maxBodySize(int32_t& retVal) = 0;
    virtual result_t set_maxBodySize(int32_t newVal) = 0;
    virtual result_t get_enableEncoding(bool& retVal) = 0;
    virtual result_t set_enableEncoding(bool newVal) = 0;
    virtual result_t get_serverName(exlib::string& retVal) = 0;
    virtual result_t set_serverName(exlib::string newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_enableCrossOrigin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxHeaderSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxHeaderSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_serverName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_serverName(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
};
}

#include "ifs/Handler.h"

namespace fibjs {
inline ClassInfo& HttpServer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "enableCrossOrigin", s_enableCrossOrigin, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false },
        { "maxHeaderSize", s_get_maxHeaderSize, s_set_maxHeaderSize, false },
        { "maxBodySize", s_get_maxBodySize, s_set_maxBodySize, false },
        { "enableEncoding", s_get_enableEncoding, s_set_enableEncoding, false },
        { "serverName", s_get_serverName, s_set_serverName, false }
    };

    static ClassData s_cd = {
        "HttpServer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &TcpServer_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpServer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpServer_base::__new(const T& args)
{
    obj_ptr<HttpServer_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Handler_base>, 2);

    hr = _new(v0, v1, v2, vr, args.This());

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpServer_base::s_enableCrossOrigin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "Content-Type");

    hr = pInst->enableCrossOrigin(v0);

    METHOD_VOID();
}

inline void HttpServer_base::s_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxHeadersCount(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_maxHeaderSize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxHeaderSize(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_maxHeaderSize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxHeaderSize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxBodySize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_enableEncoding(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_enableEncoding(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_enableEncoding(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_enableEncoding(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_serverName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_serverName(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_serverName(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_serverName(v0);

    PROPERTY_SET_LEAVE();
}
}
