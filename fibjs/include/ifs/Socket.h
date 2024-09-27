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
#include "ifs/Stream.h"

namespace fibjs {

class Stream_base;
class net_base;
class Buffer_base;

class Socket_base : public Stream_base {
    DECLARE_CLASS(Socket_base);

public:
    // Socket_base
    static result_t _new(int32_t family, obj_ptr<Socket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_family(int32_t& retVal) = 0;
    virtual result_t get_remoteAddress(exlib::string& retVal) = 0;
    virtual result_t get_remotePort(int32_t& retVal) = 0;
    virtual result_t get_localAddress(exlib::string& retVal) = 0;
    virtual result_t get_localPort(int32_t& retVal) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t connect(exlib::string host, int32_t port, int32_t timeout, AsyncEvent* ac) = 0;
    virtual result_t bind(int32_t port, bool allowIPv4) = 0;
    virtual result_t bind(exlib::string addr, int32_t port, bool allowIPv4) = 0;
    virtual result_t listen(int32_t backlog) = 0;
    virtual result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* data, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_family(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_remoteAddress(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_remotePort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_localAddress(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_localPort(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_accept(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_recv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER3(Socket_base, connect, exlib::string, int32_t, int32_t);
    ASYNC_MEMBERVALUE1(Socket_base, accept, obj_ptr<Socket_base>);
    ASYNC_MEMBERVALUE2(Socket_base, recv, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBER1(Socket_base, send, Buffer_base*);
};
}

#include "ifs/net.h"
#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& Socket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_connect, false, ClassData::ASYNC_ASYNC },
        { "bind", s_bind, false, ClassData::ASYNC_SYNC },
        { "listen", s_listen, false, ClassData::ASYNC_SYNC },
        { "accept", s_accept, false, ClassData::ASYNC_ASYNC },
        { "recv", s_recv, false, ClassData::ASYNC_ASYNC },
        { "send", s_send, false, ClassData::ASYNC_ASYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "family", s_get_family, block_set, false },
        { "remoteAddress", s_get_remoteAddress, block_set, false },
        { "remotePort", s_get_remotePort, block_set, false },
        { "localAddress", s_get_localAddress, block_set, false },
        { "localPort", s_get_localPort, block_set, false },
        { "timeout", s_get_timeout, s_set_timeout, false }
    };

    static ClassData s_cd = {
        "Socket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Stream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void Socket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void Socket_base::__new(const T& args)
{
    obj_ptr<Socket_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, net_base::C_AF_INET);

    hr = _new(v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Socket_base::s_get_family(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_family(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_remoteAddress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_remoteAddress(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_remotePort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_remotePort(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_localAddress(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_localAddress(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_localPort(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_localPort(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_timeout(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_timeout(v0);

    METHOD_VOID();
}

inline void Socket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Socket_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    OPT_ARG(int32_t, 2, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_connect(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_connect(v0, v1, v2);

    METHOD_VOID();
}

inline void Socket_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(bool, 1, true);

    hr = pInst->bind(v0, v1);

    METHOD_OVER(3, 1);

    ARG(exlib::string, 0);
    OPT_ARG(int32_t, 1, 0);
    OPT_ARG(bool, 2, true);

    hr = pInst->bind(v0, v1, v2);

    METHOD_VOID();
}

inline void Socket_base::s_listen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 120);

    hr = pInst->listen(v0);

    METHOD_VOID();
}

inline void Socket_base::s_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Socket_base> vr;

    ASYNC_METHOD_INSTANCE(Socket_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_accept(cb, args);
    else
        hr = pInst->ac_accept(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_recv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(Socket_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_recv(v0, cb, args);
    else
        hr = pInst->ac_recv(v0, vr);

    METHOD_RETURN();
}

inline void Socket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(Socket_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_send(v0, cb, args);
    else
        hr = pInst->ac_send(v0);

    METHOD_VOID();
}
}
