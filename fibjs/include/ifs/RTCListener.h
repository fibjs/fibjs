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
class RTCListenerOptions;

class RTCListener_base : public EventEmitter_base {
    DECLARE_CLASS(RTCListener_base);
    EVENT_SUPPORT();

public:
    class AddressType : public NType {
    public:
        virtual void to_value(Isolate* isolate, v8::Local<v8::Object>& retVal)
        {
            v8::Local<v8::Context> context = isolate->context();
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
    // RTCListener_base
    static result_t _new(RTCListenerOptions* options, obj_ptr<RTCListener_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t listen(int32_t port, exlib::string addr, obj_ptr<RTCListener_base>& retVal) = 0;
    virtual result_t address(obj_ptr<AddressType>& retVal) = 0;
    virtual result_t close(obj_ptr<RTCListener_base>& retVal) = 0;
    virtual result_t stop(obj_ptr<RTCListener_base>& retVal) = 0;
    virtual result_t get_certificate(v8::Local<v8::Object>& retVal) = 0;

public:
    static void __new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static result_t load(v8::Local<v8::Value> v, obj_ptr<RTCListener_base>& retVal);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_listen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_address(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_stop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_certificate(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/RTCListenerOptions.h"

namespace fibjs {
inline ClassInfo& RTCListener_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "listen", s_listen, false, ClassData::ASYNC_SYNC },
        { "address", s_address, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC },
        { "stop", s_stop, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "certificate", s_get_certificate, block_set, false },
        { "onconnection", s_get_onconnection, s_set_onconnection, false },
        { "onerror", s_get_onerror, s_set_onerror, false }
    };

    static ClassData s_cd = {
        "RTCListener", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &EventEmitter_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RTCListener_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    CONSTRUCT_INIT();
    __new(args);
}

inline void RTCListener_base::__new(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RTCListener_base> vr;

    CONSTRUCT_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(obj_ptr<RTCListenerOptions>, 0, new RTCListenerOptions(), "options");

    hr = _new((RTCListenerOptions*)v0, vr, args.This());

    CONSTRUCT_RETURN();
}

inline result_t RTCListener_base::load(v8::Local<v8::Value> v, obj_ptr<RTCListener_base>& retVal)
{
    obj_ptr<RTCListener_base> vr;

    LOAD_ENTER();

    METHOD_OVER(1, 0);

    OPT_ARG_NAMED(obj_ptr<RTCListenerOptions>, 0, new RTCListenerOptions(), "options");

    hr = _new((RTCListenerOptions*)v0, vr, args.This());

    LOAD_RETURN();
}

inline void RTCListener_base::s_listen(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RTCListener_base> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER_NAMED(2, 1, "port");

    ARG_NAMED(int32_t, 0, "port");
    OPT_ARG_NAMED(exlib::string, 1, "", "addr");

    hr = pInst->listen(v0, v1, vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_address(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<AddressType> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->address(vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RTCListener_base> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->close(vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_stop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RTCListener_base> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->stop(vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_get_certificate(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Object> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->get_certificate(vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_get_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("connection", vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_set_onconnection(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onconnection");

    hr = pInst->setListener("connection", v0);

    METHOD_VOID();
}

inline void RTCListener_base::s_get_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("error", vr);

    METHOD_RETURN();
}

inline void RTCListener_base::s_set_onerror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(RTCListener_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG_NAMED(v8::Local<v8::Function>, 0, "onerror");

    hr = pInst->setListener("error", v0);

    METHOD_VOID();
}
}
