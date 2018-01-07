/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _DgramSocket_base_H_
#define _DgramSocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "EventEmitter.h"

namespace fibjs {

class EventEmitter_base;
class Buffer_base;

class DgramSocket_base : public EventEmitter_base {
    DECLARE_CLASS(DgramSocket_base);

public:
    // DgramSocket_base
    virtual result_t bind(int32_t port, exlib::string addr, AsyncEvent* ac) = 0;
    virtual result_t bind(v8::Local<v8::Object> opts, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* msg, int32_t port, exlib::string address, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* msg, int32_t offset, int32_t length, int32_t port, exlib::string address, AsyncEvent* ac) = 0;
    virtual result_t address(obj_ptr<NObject>& retVal) = 0;
    virtual result_t close() = 0;
    virtual result_t close(v8::Local<v8::Function> callback) = 0;
    virtual result_t getRecvBufferSize(int32_t& retVal) = 0;
    virtual result_t getSendBufferSize(int32_t& retVal) = 0;
    virtual result_t setRecvBufferSize(int32_t size) = 0;
    virtual result_t setSendBufferSize(int32_t size) = 0;
    virtual result_t ref(obj_ptr<DgramSocket_base>& retVal) = 0;
    virtual result_t unref(obj_ptr<DgramSocket_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

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
    static void s_setRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER2(DgramSocket_base, bind, int32_t, exlib::string);
    ASYNC_MEMBER1(DgramSocket_base, bind, v8::Local<v8::Object>);
    ASYNC_MEMBER3(DgramSocket_base, send, Buffer_base*, int32_t, exlib::string);
    ASYNC_MEMBER5(DgramSocket_base, send, Buffer_base*, int32_t, int32_t, int32_t, exlib::string);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& DgramSocket_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "bind", s_bind, false },
        { "bindSync", s_bind, false },
        { "send", s_send, false },
        { "sendSync", s_send, false },
        { "address", s_address, false },
        { "close", s_close, false },
        { "getRecvBufferSize", s_getRecvBufferSize, false },
        { "getSendBufferSize", s_getSendBufferSize, false },
        { "setRecvBufferSize", s_setRecvBufferSize, false },
        { "setSendBufferSize", s_setSendBufferSize, false },
        { "ref", s_ref, false },
        { "unref", s_unref, false }
    };

    static ClassData s_cd = {
        "DgramSocket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DgramSocket_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("DgramSocket.bind");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 0);

    OPT_ARG(int32_t, 0, 0);
    OPT_ARG(exlib::string, 1, "");

    if (!cb.IsEmpty()) {
        pInst->acb_bind(v0, v1, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_bind(v0, v1);

    ASYNC_METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    if (!cb.IsEmpty()) {
        pInst->acb_bind(v0, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_bind(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("DgramSocket.send");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(3, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);
    OPT_ARG(exlib::string, 2, "");

    if (!cb.IsEmpty()) {
        pInst->acb_send(v0, v1, v2, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_send(v0, v1, v2);

    ASYNC_METHOD_OVER(5, 4);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(int32_t, 1);
    ARG(int32_t, 2);
    ARG(int32_t, 3);
    OPT_ARG(exlib::string, 4, "");

    if (!cb.IsEmpty()) {
        pInst->acb_send(v0, v1, v2, v3, v4, cb);
        hr = CALL_RETURN_NULL;
    } else
        hr = pInst->ac_send(v0, v1, v2, v3, v4);

    METHOD_VOID();
}

inline void DgramSocket_base::s_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NObject> vr;

    METHOD_NAME("DgramSocket.address");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->address(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("DgramSocket.close");
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

    METHOD_NAME("DgramSocket.getRecvBufferSize");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getRecvBufferSize(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_getSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("DgramSocket.getSendBufferSize");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getSendBufferSize(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_setRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("DgramSocket.setRecvBufferSize");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->setRecvBufferSize(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("DgramSocket.setSendBufferSize");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->setSendBufferSize(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_ref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    METHOD_NAME("DgramSocket.ref");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->ref(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_unref(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    METHOD_NAME("DgramSocket.unref");
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->unref(vr);

    METHOD_RETURN();
}
}

#endif
