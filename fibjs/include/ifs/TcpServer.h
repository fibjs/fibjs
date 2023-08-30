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

class Handler_base;
class Socket_base;

class TcpServer_base : public object_base {
    DECLARE_CLASS(TcpServer_base);

public:
    // TcpServer_base
    static result_t _new(int32_t port, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, int32_t port, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t start() = 0;
    virtual result_t stop(AsyncEvent* ac) = 0;
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal) = 0;
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
    virtual result_t set_handler(Handler_base* newVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_socket(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_handler(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_handler(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);

public:
    ASYNC_MEMBER0(TcpServer_base, stop);
};
}

#include "ifs/Handler.h"
#include "ifs/Socket.h"

namespace fibjs {
inline ClassInfo& TcpServer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "start", s_start, false, false },
        { "stop", s_stop, false, true },
        { "stopSync", s_stop, false, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "socket", s_get_socket, block_set, false },
        { "handler", s_get_handler, s_set_handler, false }
    };

    static ClassData s_cd = {
        "TcpServer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TcpServer_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void TcpServer_base::__new(const T& args)
{
    obj_ptr<TcpServer_base> vr;

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

inline void TcpServer_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->start();

    METHOD_VOID();
}

inline void TcpServer_base::s_stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_stop(cb, args);
    else
        hr = pInst->ac_stop();

    METHOD_VOID();
}

inline void TcpServer_base::s_get_socket(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Socket_base> vr;

    METHOD_INSTANCE(TcpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_socket(vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_get_handler(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_INSTANCE(TcpServer_base);
    PROPERTY_ENTER();

    hr = pInst->get_handler(vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_handler(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(obj_ptr<Handler_base>);

    hr = pInst->set_handler(v0);

    PROPERTY_SET_LEAVE();
}
}
