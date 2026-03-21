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
#include "ifs/EventEmitter.h"

namespace fibjs {

class EventEmitter_base;

class HttpAgent_base : public EventEmitter_base {
    DECLARE_CLASS(HttpAgent_base);
    EVENT_SUPPORT();

public:
    // HttpAgent_base
    static result_t _new(v8::Local<v8::Object> options, obj_ptr<HttpAgent_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t getName(v8::Local<v8::Object> options, exlib::string& retVal) = 0;
    virtual result_t destroy() = 0;
    virtual result_t get_keepAlive(bool& retVal) = 0;
    virtual result_t set_keepAlive(bool newVal) = 0;
    virtual result_t get_maxSockets(int32_t& retVal) = 0;
    virtual result_t set_maxSockets(int32_t newVal) = 0;
    virtual result_t get_maxTotalSockets(int32_t& retVal) = 0;
    virtual result_t set_maxTotalSockets(int32_t newVal) = 0;
    virtual result_t get_maxFreeSockets(int32_t& retVal) = 0;
    virtual result_t set_maxFreeSockets(int32_t newVal) = 0;
    virtual result_t get_defaultPort(int32_t& retVal) = 0;
    virtual result_t set_defaultPort(int32_t newVal) = 0;
    virtual result_t get_protocol(exlib::string& retVal) = 0;
    virtual result_t set_protocol(exlib::string newVal) = 0;
    virtual result_t get_freeSockets(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_sockets(v8::Local<v8::Object>& retVal) = 0;
    virtual result_t get_totalSocketCount(int32_t& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<HttpAgent_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxTotalSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxTotalSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_defaultPort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_defaultPort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_protocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_freeSockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_sockets(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_totalSocketCount(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

namespace fibjs {
inline ClassInfo& HttpAgent_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "getName", s_getName, false, ClassData::ASYNC_SYNC },
        { "destroy", s_destroy, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "keepAlive", s_get_keepAlive, s_set_keepAlive, false },
        { "maxSockets", s_get_maxSockets, s_set_maxSockets, false },
        { "maxTotalSockets", s_get_maxTotalSockets, s_set_maxTotalSockets, false },
        { "maxFreeSockets", s_get_maxFreeSockets, s_set_maxFreeSockets, false },
        { "defaultPort", s_get_defaultPort, s_set_defaultPort, false },
        { "protocol", s_get_protocol, s_set_protocol, false },
        { "freeSockets", s_get_freeSockets, block_set, false },
        { "sockets", s_get_sockets, block_set, false },
        { "totalSocketCount", s_get_totalSocketCount, block_set, false }
    };

    static ClassData s_cd = {
        "HttpAgent", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void HttpAgent_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void HttpAgent_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<HttpAgent_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t HttpAgent_base::load(v8::Local<v8::Value> v, obj_ptr<HttpAgent_base>& retVal)
{
    obj_ptr<HttpAgent_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = _new(v0, vr, args.This());

    LOAD_RETURN();
}

inline void HttpAgent_base::s_getName(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(v8::Local<v8::Object>, 0, v8::Object::New(isolate->m_isolate));

    hr = pInst->getName(v0, vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_destroy(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->destroy();

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_keepAlive(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_set_keepAlive(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->set_keepAlive(v0);

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_maxSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxSockets(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_set_maxSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxSockets(v0);

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_maxTotalSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxTotalSockets(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_set_maxTotalSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxTotalSockets(v0);

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_maxFreeSockets(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_set_maxFreeSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_maxFreeSockets(v0);

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_defaultPort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_defaultPort(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_set_defaultPort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_defaultPort(v0);

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_protocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_protocol(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_set_protocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(exlib::string, 0);

    hr = pInst->set_protocol(v0);

    METHOD_VOID();
}

inline void HttpAgent_base::s_get_freeSockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_freeSockets(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_get_sockets(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_sockets(vr);

    METHOD_RETURN();
}

inline void HttpAgent_base::s_get_totalSocketCount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(HttpAgent_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_totalSocketCount(vr);

    METHOD_RETURN();
}
}
