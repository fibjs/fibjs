/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _HttpServer_base_H_
#define _HttpServer_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "TcpServer.h"

namespace fibjs {

class TcpServer_base;
class Handler_base;
class Stats_base;

class HttpServer_base : public TcpServer_base {
    DECLARE_CLASS(HttpServer_base);

public:
    // HttpServer_base
    static result_t _new(int32_t port, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, int32_t port, Handler_base* hdlr, obj_ptr<HttpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t onerror(v8::Local<v8::Object> hdlrs) = 0;
    virtual result_t enableCrossOrigin(exlib::string allowHeaders) = 0;
    virtual result_t get_forceGZIP(bool& retVal) = 0;
    virtual result_t set_forceGZIP(bool newVal) = 0;
    virtual result_t get_maxHeadersCount(int32_t& retVal) = 0;
    virtual result_t set_maxHeadersCount(int32_t newVal) = 0;
    virtual result_t get_maxBodySize(int32_t& retVal) = 0;
    virtual result_t set_maxBodySize(int32_t newVal) = 0;
    virtual result_t get_serverName(exlib::string& retVal) = 0;
    virtual result_t set_serverName(exlib::string newVal) = 0;
    virtual result_t get_httpStats(obj_ptr<Stats_base>& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_enableCrossOrigin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_forceGZIP(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_forceGZIP(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_serverName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_serverName(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_get_httpStats(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
};
}

#include "Handler.h"
#include "Stats.h"

namespace fibjs {
inline ClassInfo& HttpServer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "onerror", s_onerror, false },
        { "enableCrossOrigin", s_enableCrossOrigin, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "forceGZIP", s_get_forceGZIP, s_set_forceGZIP, false },
        { "maxHeadersCount", s_get_maxHeadersCount, s_set_maxHeadersCount, false },
        { "maxBodySize", s_get_maxBodySize, s_set_maxBodySize, false },
        { "serverName", s_get_serverName, s_set_serverName, false },
        { "httpStats", s_get_httpStats, block_set, false }
    };

    static ClassData s_cd = {
        "HttpServer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &TcpServer_base::class_info()
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

    METHOD_NAME("new HttpServer()");
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

    CONSTRUCT_RETURN();
}

inline void HttpServer_base::s_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpServer.onerror");
    METHOD_INSTANCE(HttpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->onerror(v0);

    METHOD_VOID();
}

inline void HttpServer_base::s_enableCrossOrigin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("HttpServer.enableCrossOrigin");
    METHOD_INSTANCE(HttpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(exlib::string, 0, "Content-Type");

    hr = pInst->enableCrossOrigin(v0);

    METHOD_VOID();
}

inline void HttpServer_base::s_get_forceGZIP(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("HttpServer.forceGZIP");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_forceGZIP(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_forceGZIP(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpServer.forceGZIP");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(bool);

    hr = pInst->set_forceGZIP(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_maxHeadersCount(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpServer.maxHeadersCount");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxHeadersCount(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_maxHeadersCount(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpServer.maxHeadersCount");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxHeadersCount(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_maxBodySize(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("HttpServer.maxBodySize");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_maxBodySize(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_maxBodySize(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpServer.maxBodySize");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_maxBodySize(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_serverName(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_NAME("HttpServer.serverName");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_serverName(vr);

    METHOD_RETURN();
}

inline void HttpServer_base::s_set_serverName(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_NAME("HttpServer.serverName");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(exlib::string);

    hr = pInst->set_serverName(v0);

    PROPERTY_SET_LEAVE();
}

inline void HttpServer_base::s_get_httpStats(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stats_base> vr;

    METHOD_NAME("HttpServer.httpStats");
    METHOD_INSTANCE(HttpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_httpStats(vr);

    METHOD_RETURN();
}
}

#endif
