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
class DgramBindOptions;
class Buffer_base;

class DgramSocket_base : public EventEmitter_base {
    DECLARE_CLASS(DgramSocket_base);
    EVENT_SUPPORT();

public:
    using Union_send_msg = std::variant<obj_ptr<Buffer_base>, exlib::string>;

public:
    class AddressType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
            retVal->Set(context, isolate->NewString("family"), GetReturnValue(isolate, family)).Check();
            retVal->Set(context, isolate->NewString("address"), GetReturnValue(isolate, address)).Check();
            retVal->Set(context, isolate->NewString("port"), GetReturnValue(isolate, port)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, family));
            args.push_back(GetReturnValue(isolate, address));
            args.push_back(GetReturnValue(isolate, port));
        }

    public:
        exlib::string family;
        exlib::string address;
        int32_t port;
    };

public:
    // DgramSocket_base
    virtual result_t bind(int32_t port, exlib::string addr, obj_ptr<DgramSocket_base>& retVal, AsyncHandle ac) = 0;
    virtual result_t bind(DgramBindOptions* opts, obj_ptr<DgramSocket_base>& retVal, AsyncHandle ac) = 0;
    virtual result_t send(Union_send_msg msg, int32_t port, exlib::string address, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t send(Union_send_msg msg, int32_t offset, int32_t length, int32_t port, exlib::string address, int32_t& retVal, AsyncHandle ac) = 0;
    virtual result_t address(obj_ptr<AddressType>& retVal) = 0;
    virtual result_t close(obj_ptr<DgramSocket_base>& retVal) = 0;
    virtual result_t close(v8::Local<v8::Function> callback, obj_ptr<DgramSocket_base>& retVal) = 0;
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

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<DgramSocket_base>& retVal)
    { return result_t::type_mismatch(); }

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
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ref(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unref(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE3(DgramSocket_base, bind, int32_t, exlib::string, obj_ptr<DgramSocket_base>);
    ASYNC_MEMBERVALUE2(DgramSocket_base, bind, obj_ptr<DgramBindOptions>, obj_ptr<DgramSocket_base>);
    ASYNC_MEMBERVALUE4(DgramSocket_base, send, Union_send_msg, int32_t, exlib::string, int32_t);
    ASYNC_MEMBERVALUE6(DgramSocket_base, send, Union_send_msg, int32_t, int32_t, int32_t, exlib::string, int32_t);
};
}

#include "ifs/DgramBindOptions.h"
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

    static ClassData::ClassProperty s_property[] = {
        { "onclose", s_get_onclose, s_set_onclose, false },
        { "onerror", s_get_onerror, s_set_onerror, false },
        { "onlistening", s_get_onlistening, s_set_onlistening, false },
        { "onmessage", s_get_onmessage, s_set_onmessage, false }
    };

    static ClassData s_cd = {
        "DgramSocket", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void DgramSocket_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    ASYNC_METHOD_INSTANCE(DgramSocket_base);
    ASYNC_METHOD_ENTER("DgramSocket.bind");

    METHOD_OVER(2, 0);

    OPT_ARG_NAMED(int32_t, 0, 0, "port");
    OPT_ARG_NAMED(exlib::string, 1, "", "addr");

    if (!cb.IsEmpty())
        hr = pInst->acb_bind(v0, v1, cb, args);
    else
        hr = pInst->ac_bind(v0, v1, vr);

    METHOD_OVER_NAMED(1, 1, "opts");

    ARG_NAMED(obj_ptr<DgramBindOptions>, 0, "opts");

    if (!cb.IsEmpty())
        hr = pInst->acb_bind(v0, cb, args);
    else
        hr = pInst->ac_bind(v0, vr);

    ASYNC_METHOD_RETURN();
}

inline void DgramSocket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    ASYNC_METHOD_INSTANCE(DgramSocket_base);
    ASYNC_METHOD_ENTER("DgramSocket.send");

    METHOD_OVER_NAMED(3, 2, "port");

    ARG(Union_send_msg, 0);
    ARG_NAMED(int32_t, 1, "port");
    OPT_ARG_NAMED(exlib::string, 2, "", "address");

    if (!cb.IsEmpty())
        hr = pInst->acb_send(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_send(v0, v1, v2, vr);

    METHOD_OVER_NAMED(5, 4, "port");

    ARG(Union_send_msg, 0);
    ARG_NAMED(int32_t, 1, "offset");
    ARG_NAMED(int32_t, 2, "length");
    ARG_NAMED(int32_t, 3, "port");
    OPT_ARG_NAMED(exlib::string, 4, "", "address");

    if (!cb.IsEmpty())
        hr = pInst->acb_send(v0, v1, v2, v3, v4, cb, args);
    else
        hr = pInst->ac_send(v0, v1, v2, v3, v4, vr);

    ASYNC_METHOD_RETURN();
}

inline void DgramSocket_base::s_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AddressType> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->address(vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<DgramSocket_base> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close(vr);

    METHOD_OVER_NAMED(1, 1, "callback");

    ARG_NAMED(v8::Local<v8::Function>, 0, "callback");

    hr = pInst->close(v0, vr);

    METHOD_RETURN();
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

    METHOD_OVER_NAMED(2, 1, "multicastAddress");

    ARG_NAMED(exlib::string, 0, "multicastAddress");
    OPT_ARG_NAMED(exlib::string, 1, "", "multicastInterface");

    hr = pInst->addMembership(v0, v1);

    METHOD_VOID();
}

inline void DgramSocket_base::s_dropMembership(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "multicastAddress");

    ARG_NAMED(exlib::string, 0, "multicastAddress");
    OPT_ARG_NAMED(exlib::string, 1, "", "multicastInterface");

    hr = pInst->dropMembership(v0, v1);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setMulticastTTL(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "ttl");

    ARG_NAMED(int32_t, 0, "ttl");

    hr = pInst->setMulticastTTL(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setRecvBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "size");

    ARG_NAMED(int32_t, 0, "size");

    hr = pInst->setRecvBufferSize(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setSendBufferSize(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "size");

    ARG_NAMED(int32_t, 0, "size");

    hr = pInst->setSendBufferSize(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_setBroadcast(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(1, 1, "flag");

    ARG_NAMED(bool, 0, "flag");

    hr = pInst->setBroadcast(v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close", vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onclose");

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onerror");

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_get_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("listening", vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_set_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onlistening");

    hr = pInst->setListener("listening", v0);

    METHOD_VOID();
}

inline void DgramSocket_base::s_get_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("message", vr);

    METHOD_RETURN();
}

inline void DgramSocket_base::s_set_onmessage(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(DgramSocket_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onmessage");

    hr = pInst->setListener("message", v0);

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
