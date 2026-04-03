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
class Handler_base;
class Socket_base;

class TcpServer_base : public EventEmitter_base {
    DECLARE_CLASS(TcpServer_base);
    EVENT_SUPPORT();

public:
    class AddressType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = retVal->GetCreationContextChecked();
            retVal->Set(context, isolate->NewString("address"), GetReturnValue(isolate, address)).Check();
            retVal->Set(context, isolate->NewString("family"), GetReturnValue(isolate, family)).Check();
            retVal->Set(context, isolate->NewString("port"), GetReturnValue(isolate, port)).Check();
        }

        virtual void to_args(Isolate* isolate, std::vector<v8::Local<v8::Value>>& args)
        {
            args.push_back(GetReturnValue(isolate, address));
            args.push_back(GetReturnValue(isolate, family));
            args.push_back(GetReturnValue(isolate, port));
        }

    public:
        exlib::string address;
        exlib::string family;
        int32_t port;
    };

public:
    // TcpServer_base
    static result_t _new(int32_t port, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, int32_t port, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(v8::Local<v8::Object> options, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(exlib::string addr, Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Handler_base* listener, obj_ptr<TcpServer_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t start() = 0;
    virtual result_t listen(int32_t port, exlib::string addr, int32_t backlog, AsyncEvent* ac) = 0;
    virtual result_t stop(AsyncEvent* ac) = 0;
    virtual result_t close(AsyncEvent* ac) = 0;
    virtual result_t address(obj_ptr<AddressType>& retVal) = 0;
    virtual result_t get_socket(obj_ptr<Socket_base>& retVal) = 0;
    virtual result_t get_timeout(int32_t& retVal) = 0;
    virtual result_t set_timeout(int32_t newVal) = 0;
    virtual result_t get_handler(obj_ptr<Handler_base>& retVal) = 0;
    virtual result_t set_handler(Handler_base* newVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<TcpServer_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_start(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_address(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_handler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_handler(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBER3(TcpServer_base, listen, int32_t, exlib::string, int32_t);
    ASYNC_MEMBER0(TcpServer_base, stop);
    ASYNC_MEMBER0(TcpServer_base, close);
};
}

#include "ifs/Handler.h"
#include "ifs/Socket.h"

namespace fibjs {
inline ClassInfo& TcpServer_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "start", s_start, false, ClassData::ASYNC_SYNC },
        { "listen", s_listen, false, ClassData::ASYNC_ASYNC },
        { "stop", s_stop, false, ClassData::ASYNC_ASYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "address", s_address, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "socket", s_get_socket, block_set, false },
        { "timeout", s_get_timeout, s_set_timeout, false },
        { "handler", s_get_handler, s_set_handler, false },
        { "onlistening", s_get_onlistening, s_set_onlistening, false },
        { "onconnection", s_get_onconnection, s_set_onconnection, false },
        { "onerror", s_get_onerror, s_set_onerror, false },
        { "onclose", s_get_onclose, s_set_onclose, false }
    };

    static ClassData s_cd = {
        "TcpServer", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
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

inline void TcpServer_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<TcpServer_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(2, 2);

    ARG(int32_t, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1.get(), vr, args.This());

    METHOD_OVER(3, 3);

    ARG(exlib::string, 0);
    ARG(int32_t, 1);
    ARG(obj_ptr<Handler_base>, 2);

    hr = _new(v0, v1, v2.get(), vr, args.This());

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1.get(), vr, args.This());

    METHOD_OVER(2, 2);

    ARG(exlib::string, 0);
    ARG(obj_ptr<Handler_base>, 1);

    hr = _new(v0, v1.get(), vr, args.This());

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t TcpServer_base::load(v8::Local<v8::Value> v, obj_ptr<TcpServer_base>& retVal)
{
    obj_ptr<TcpServer_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = _new(v0.get(), vr, args.This());

    LOAD_RETURN();
}

inline void TcpServer_base::s_start(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->start();

    METHOD_VOID();
}

inline void TcpServer_base::s_listen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TcpServer_base);
    ASYNC_METHOD_ENTER("TcpServer.listen");

    METHOD_OVER(3, 1);

    ARG(int32_t, 0);
    OPT_ARG(exlib::string, 1, "");
    OPT_ARG(int32_t, 2, -1);

    if (!cb.IsEmpty())
        hr = pInst->acb_listen(v0, v1, v2, cb, args);
    else
        hr = pInst->ac_listen(v0, v1, v2);

    ASYNC_METHOD_VOID();
}

inline void TcpServer_base::s_stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TcpServer_base);
    ASYNC_METHOD_ENTER("TcpServer.stop");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_stop(cb, args);
    else
        hr = pInst->ac_stop();

    ASYNC_METHOD_VOID();
}

inline void TcpServer_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(TcpServer_base);
    ASYNC_METHOD_ENTER("TcpServer.close");

    METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    ASYNC_METHOD_VOID();
}

inline void TcpServer_base::s_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AddressType> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->address(vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Socket_base> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_socket(vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_get_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_timeout(vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_timeout(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->set_timeout(v0);

    METHOD_VOID();
}

inline void TcpServer_base::s_get_handler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Handler_base> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_handler(vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_handler(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Handler_base>, 0);

    hr = pInst->set_handler(v0);

    METHOD_VOID();
}

inline void TcpServer_base::s_get_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("listening", vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_onlistening(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("listening", v0);

    METHOD_VOID();
}

inline void TcpServer_base::s_get_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("connection", vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("connection", v0);

    METHOD_VOID();
}

inline void TcpServer_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}

inline void TcpServer_base::s_get_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("close", vr);

    METHOD_RETURN();
}

inline void TcpServer_base::s_set_onclose(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(TcpServer_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("close", v0);

    METHOD_VOID();
}
}
