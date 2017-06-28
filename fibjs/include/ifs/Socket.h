/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Socket_base_H_
#define _Socket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Stream.h"

namespace fibjs {

class Stream_base;
class net_base;
class Buffer_base;
class DatagramPacket_base;

class Socket_base : public Stream_base {
    DECLARE_CLASS(Socket_base);

public:
    // Socket_base
    static result_t _new(int32_t family, int32_t type, obj_ptr<Socket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t get_family(int32_t& retVal) = 0;
    virtual result_t get_type(int32_t& retVal) = 0;
    virtual result_t get_remoteAddress(exlib::string& retVal) = 0;
    virtual result_t get_remotePort(int32_t& retVal) = 0;
    virtual result_t get_localAddress(exlib::string& retVal) = 0;
    virtual result_t get_localPort(int32_t& retVal) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t connect(exlib::string host, int32_t port, AsyncEvent* ac) = 0;
    virtual result_t bind(int32_t port, bool allowIPv4, AsyncEvent* ac) = 0;
    virtual result_t bind(exlib::string addr, int32_t port, bool allowIPv4, AsyncEvent* ac) = 0;
    virtual result_t listen(int32_t backlog, AsyncEvent* ac) = 0;
    virtual result_t accept(obj_ptr<Socket_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t recv(int32_t bytes, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t recvfrom(int32_t bytes, obj_ptr<DatagramPacket_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* data, AsyncEvent* ac) = 0;
    virtual result_t sendto(Buffer_base* data, exlib::string host, int32_t port, AsyncEvent* ac) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_family(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_remoteAddress(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_remotePort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_localAddress(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_localPort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_accept(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_recv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_recvfrom(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sendto(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER2(Socket_base, connect, exlib::string, int32_t);
    ASYNC_MEMBER2(Socket_base, bind, int32_t, bool);
    ASYNC_MEMBER3(Socket_base, bind, exlib::string, int32_t, bool);
    ASYNC_MEMBER1(Socket_base, listen, int32_t);
    ASYNC_MEMBERVALUE1(Socket_base, accept, obj_ptr<Socket_base>);
    ASYNC_MEMBERVALUE2(Socket_base, recv, int32_t, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE2(Socket_base, recvfrom, int32_t, obj_ptr<DatagramPacket_base>);
    ASYNC_MEMBER1(Socket_base, send, Buffer_base*);
    ASYNC_MEMBER3(Socket_base, sendto, Buffer_base*, exlib::string, int32_t);
};
}

#include "net.h"
#include "Buffer.h"
#include "DatagramPacket.h"

namespace fibjs {
inline ClassInfo& Socket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_connect, false },
        { "connectSync", s_connect, false },
        { "bind", s_bind, false },
        { "bindSync", s_bind, false },
        { "listen", s_listen, false },
        { "listenSync", s_listen, false },
        { "accept", s_accept, false },
        { "acceptSync", s_accept, false },
        { "recv", s_recv, false },
        { "recvSync", s_recv, false },
        { "recvfrom", s_recvfrom, false },
        { "recvfromSync", s_recvfrom, false },
        { "send", s_send, false },
        { "sendSync", s_send, false },
        { "sendto", s_sendto, false },
        { "sendtoSync", s_sendto, false }
    };

    static ClassData::ClassProperty s_property[] = {
        { "family", s_get_family, block_set, false },
        { "type", s_get_type, block_set, false },
        { "remoteAddress", s_get_remoteAddress, block_set, false },
        { "remotePort", s_get_remotePort, block_set, false },
        { "localAddress", s_get_localAddress, block_set, false },
        { "localPort", s_get_localPort, block_set, false },
        { "timeout", s_get_timeout, s_set_timeout, false }
    };

    static ClassData s_cd = {
        "Socket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
        &Stream_base::class_info()
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

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, net_base::_AF_INET);
    OPT_ARG(int32_t, 1, net_base::_SOCK_STREAM);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void Socket_base::s_get_family(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_family(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_type(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_remoteAddress(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_remoteAddress(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_remotePort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_remotePort(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_localAddress(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_localAddress(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_localPort(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_localPort(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_get_timeout(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();

    hr = pInst->get_timeout(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_set_timeout(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args)
{
    METHOD_INSTANCE(Socket_base);
    PROPERTY_ENTER();
    PROPERTY_VAL(int32_t);

    hr = pInst->set_timeout(v0);

    PROPERTY_SET_LEAVE();
}

inline void Socket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);

    if (!cb.IsEmpty()) {
        pInst->acb_connect(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_connect(v0, v1);

    METHOD_VOID();
}

inline void Socket_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(int32_t, 0);
    OPT_ARG(bool, 1, true);

    if (!cb.IsEmpty()) {
        pInst->acb_bind(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_bind(v0, v1);

    ASYNC_METHOD_OVER(3, 2);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    OPT_ARG(bool, 2, true);

    if (!cb.IsEmpty()) {
        pInst->acb_bind(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_bind(v0, v1, v2);

    METHOD_VOID();
}

inline void Socket_base::s_listen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, 120);

    if (!cb.IsEmpty()) {
        pInst->acb_listen(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_listen(v0);

    METHOD_VOID();
}

inline void Socket_base::s_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Socket_base> vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_accept(cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_accept(vr);

    METHOD_RETURN();
}

inline void Socket_base::s_recv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, -1);

    if (!cb.IsEmpty()) {
        pInst->acb_recv(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_recv(v0, vr);

    METHOD_RETURN();
}

inline void Socket_base::s_recvfrom(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DatagramPacket_base> vr;

    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 0);

    OPT_ARG(int32_t, 0, -1);

    if (!cb.IsEmpty()) {
        pInst->acb_recvfrom(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_recvfrom(v0, vr);

    METHOD_RETURN();
}

inline void Socket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_send(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_send(v0);

    METHOD_VOID();
}

inline void Socket_base::s_sendto(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Socket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 3);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(exlib::string, 1);
    ARG(int32_t, 2);

    if (!cb.IsEmpty()) {
        pInst->acb_sendto(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_sendto(v0, v1, v2);

    METHOD_VOID();
}
}

#endif
