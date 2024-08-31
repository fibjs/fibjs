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
class SecureContext_base;
class X509Certificate_base;

class TLSSocket_base : public Stream_base {
    DECLARE_CLASS(TLSSocket_base);

public:
    // TLSSocket_base
    static result_t _new(obj_ptr<TLSSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(SecureContext_base* context, obj_ptr<TLSSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, bool isServer, obj_ptr<TLSSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t connect(Stream_base* socket, exlib::string server_name, AsyncEvent* ac) = 0;
    virtual result_t accept(Stream_base* socket, AsyncEvent* ac) = 0;
    virtual result_t get_stream(obj_ptr<Stream_base>& retVal) = 0;
    virtual result_t getProtocol(exlib::string& retVal) = 0;
    virtual result_t getX509Certificate(obj_ptr<X509Certificate_base>& retVal) = 0;
    virtual result_t getPeerX509Certificate(obj_ptr<X509Certificate_base>& retVal) = 0;
    virtual result_t get_secureContext(obj_ptr<SecureContext_base>& retVal) = 0;
    virtual result_t get_remoteAddress(exlib::string& retVal) = 0;
    virtual result_t get_remotePort(int32_t& retVal) = 0;
    virtual result_t get_localAddress(exlib::string& retVal) = 0;
    virtual result_t get_localPort(int32_t& retVal) = 0;

public:
    template <typename T>
    static void __new(const T& args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_accept(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_stream(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_getProtocol(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getX509Certificate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getPeerX509Certificate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_remoteAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_remotePort(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_localAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void s_get_localPort(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER2(TLSSocket_base, connect, Stream_base*, exlib::string);
    ASYNC_MEMBER1(TLSSocket_base, accept, Stream_base*);
};
}

#include "ifs/SecureContext.h"
#include "ifs/X509Certificate.h"

namespace fibjs {
inline ClassInfo& TLSSocket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "connect", s_connect, false, ClassData::ASYNC_ASYNC },
        { "accept", s_accept, false, ClassData::ASYNC_ASYNC },
        { "getProtocol", s_getProtocol, false, ClassData::ASYNC_SYNC },
        { "getX509Certificate", s_getX509Certificate, false, ClassData::ASYNC_SYNC },
        { "getPeerX509Certificate", s_getPeerX509Certificate, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "stream", s_get_stream, block_set, false },
        { "secureContext", s_get_secureContext, block_set, false },
        { "remoteAddress", s_get_remoteAddress, block_set, false },
        { "remotePort", s_get_remotePort, block_set, false },
        { "localAddress", s_get_localAddress, block_set, false },
        { "localPort", s_get_localPort, block_set, false }
    };

    static ClassData s_cd = {
        "TLSSocket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &Stream_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void TLSSocket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

template <typename T>
void TLSSocket_base::__new(const T& args)
{
    obj_ptr<TLSSocket_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(0, 0);

    hr = _new(vr, args.This());

    METHOD_OVER(1, 1);

    ARG(obj_ptr<SecureContext_base>, 0);

    hr = _new(v0, vr, args.This());

    METHOD_OVER(2, 1);

    ARG(v8::Local<v8::Object>, 0);
    OPT_ARG(bool, 1, false);

    hr = _new(v0, v1, vr, args.This());

    CONSTRUCT_RETURN();
}

inline void TLSSocket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TLSSocket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 1);

    ARG(obj_ptr<Stream_base>, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_connect(v0, v1, cb, args);
    else
        hr = pInst->ac_connect(v0, v1);

    METHOD_VOID();
}

inline void TLSSocket_base::s_accept(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TLSSocket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Stream_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_accept(v0, cb, args);
    else
        hr = pInst->ac_accept(v0);

    METHOD_VOID();
}

inline void TLSSocket_base::s_get_stream(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<Stream_base> vr;

    METHOD_INSTANCE(TLSSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_stream(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_getProtocol(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(TLSSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getProtocol(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_getX509Certificate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Certificate_base> vr;

    METHOD_INSTANCE(TLSSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getX509Certificate(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_getPeerX509Certificate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<X509Certificate_base> vr;

    METHOD_INSTANCE(TLSSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getPeerX509Certificate(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_get_secureContext(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    obj_ptr<SecureContext_base> vr;

    METHOD_INSTANCE(TLSSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_secureContext(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_get_remoteAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(TLSSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_remoteAddress(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_get_remotePort(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(TLSSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_remotePort(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_get_localAddress(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(TLSSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_localAddress(vr);

    METHOD_RETURN();
}

inline void TLSSocket_base::s_get_localPort(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(TLSSocket_base);
    PROPERTY_ENTER();

    hr = pInst->get_localPort(vr);

    METHOD_RETURN();
}
}
