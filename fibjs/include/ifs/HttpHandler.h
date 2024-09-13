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
#include "ifs/Handler.h"

namespace fibjs {

class Handler_base;

class HttpHandler_base : public Handler_base {
    DECLARE_CLASS(HttpHandler_base);

public:
    // HttpHandler_base
    static result_t _new(Handler_base* hdlr, obj_ptr<HttpHandler_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
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
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
    virtual result_t set_handler(Handler_base* newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_enableCrossOrigin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_serverName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_serverName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_handler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_handler(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& HttpHandler_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "enableCrossOrigin", s_enableCrossOrigin, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false },
        { "maxHeaderSize", s_get_maxHeaderSize, s_set_maxHeaderSize, false },
        { "maxBodySize", s_get_maxBodySize, s_set_maxBodySize, false },
        { "enableEncoding", s_get_enableEncoding, s_set_enableEncoding, false },
        { "serverName", s_get_serverName, s_set_serverName, false },
        { "handler", s_get_handler, s_set_handler, false }
    };

    static ClassData s_cd = {
        "HttpHandler", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Handler_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpHandler_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void HttpHandler_base::__new(const T& args)
{
    obj_ptr<HttpHandler_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void HttpHandler_base::s_enableCrossOrigin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "Content-Type");

    hr = pInst->enableCrossOrigin(v0);

    METHOD_VOID();
}

inline void HttpHandler_base::s_get_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void HttpHandler_base::s_set_maxHeadersCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxHeadersCount(v0);

    METHOD_VOID();
}

inline void HttpHandler_base::s_get_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxHeaderSize(vr);

    METHOD_RETURN();
}

inline void HttpHandler_base::s_set_maxHeaderSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxHeaderSize(v0);

    METHOD_VOID();
}

inline void HttpHandler_base::s_get_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void HttpHandler_base::s_set_maxBodySize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxBodySize(v0);

    METHOD_VOID();
}

inline void HttpHandler_base::s_get_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_enableEncoding(vr);

    METHOD_RETURN();
}

inline void HttpHandler_base::s_set_enableEncoding(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_enableEncoding(v0);

    METHOD_VOID();
}

inline void HttpHandler_base::s_get_serverName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_serverName(vr);

    METHOD_RETURN();
}

inline void HttpHandler_base::s_set_serverName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_serverName(v0);

    METHOD_VOID();
}

inline void HttpHandler_base::s_get_handler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_handler(vr);

    METHOD_RETURN();
}

inline void HttpHandler_base::s_set_handler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpHandler_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = pInst->set_handler(v0);

    METHOD_VOID();
}
}
