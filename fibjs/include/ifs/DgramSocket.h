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
class Buffer_base;

class DgramSocket_base : public EventEmitter_base {
    DECLARE_CLASS(DgramSocket_base);

public:
    // DgramSocket_base
    virtual result_t bind(int32_t port, exlib::string addr, AsyncEvent* ac) = 0;
    virtual result_t bind(v8::Local<v8::Object> opts, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* msg, int32_t port, exlib::string address, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* msg, int32_t offset, int32_t length, int32_t port, exlib::string address, int32_t& retVal, AsyncEvent* ac) = 0;
    virtual result_t address(obj_ptr<NObject>& retVal) = 0;
    virtual result_t close() = 0;
    virtual result_t close(v8::Local<v8::Function> callback) = 0;
    virtual result_t getRecvBufferSize(int32_t& retVal) = 0;
    virtual result_t getSendBufferSize(int32_t& retVal) = 0;
    virtual result_t addMembership(exlib::string multicastAddress, exlib::string multicastInterface) = 0;
    virtual result_t dropMembership(exlib::string multicastAddress, exlib::string multicastInterface) = 0;
    virtual result_t setMulticastTTL(int32_t ttl) = 0;
    virtual result_t setRecvBufferSize(int32_t size) = 0;
    virtual result_t setSendBufferSize(int32_t size) = 0;
    virtual result_t setBroadcast(bool flag) = 0;
    virtual result_t ref(obj_ptr<DgramSocket_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<DgramSocket_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_address(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_addMembership(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dropMembership(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setMulticastTTL(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setBroadcast(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER2(DgramSocket_base, bind, int32_t, exlib::string);
    ASYNC_MEMBER1(DgramSocket_base, bind, v8::Local<v8::Object>);
    ASYNC_MEMBERVALUE4(DgramSocket_base, send, Buffer_base*, int32_t, exlib::string, int32_t);
    ASYNC_MEMBERVALUE6(DgramSocket_base, send, Buffer_base*, int32_t, int32_t, int32_t, exlib::string, int32_t);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& DgramSocket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "bind", s_bind, false, ClassData::ASYNC_ASYNC },
        { "send", s_send, false, ClassData::ASYNC_ASYNC },
        { "address", s_address, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "getRecvBufferSize", s_getRecvBufferSize, false, ClassData::ASYNC_SYNC },
        { "getSendBufferSize", s_getSendBufferSize, false, ClassData::ASYNC_SYNC },
        { "addMembership", s_addMembership, false, ClassData::ASYNC_SYNC },
        { "dropMembership", s_dropMembership, false, ClassData::ASYNC_SYNC },
        { "setMulticastTTL", s_setMulticastTTL, false, ClassData::ASYNC_SYNC },
        { "setRecvBufferSize", s_setRecvBufferSize, false, ClassData::ASYNC_SYNC },
        { "setSendBufferSize", s_setSendBufferSize, false, ClassData::ASYNC_SYNC },
        { "setBroadcast", s_setBroadcast, false, ClassData::ASYNC_SYNC },
        { "ref", s_ref, false, ClassData::ASYNC_SYNC },
        { "unref", s_unref, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "DgramSocket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DgramSocket_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(DgramSocket_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_bind(v0, v1, cb, args);
    else
        hr = pInst->ac_bind(v0, v1);

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_bind(v0, cb, args);
    else
        hr = pInst->ac_bind(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(DgramSocket_base);
    ASYNC_METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_send(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_send(v0, v1, v2, vr);

    METHOD_OVER(5, 4);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    OPT_ARG(exlib::string, 4, "");

    if (!cb.IsEmpty())
        hr = pInst->acb_send(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_send(v0, v1, v2, v3, v4, vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->address(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->close(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_getRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getRecvBufferSize(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_getSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getSendBufferSize(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_addMembership(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->addMembership(v0, v1);

    METHOD_VOID();
}

inline void DgramSocket_base::s_dropMembership(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(exlib::string, 0);
    OPT_ARG(exlib::string, 1, "");

    hr = pInst->dropMembership(v0, v1);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setMulticastTTL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->setMulticastTTL(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->setRecvBufferSize(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->setSendBufferSize(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setBroadcast(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(bool, 0);

    hr = pInst->setBroadcast(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}
}
