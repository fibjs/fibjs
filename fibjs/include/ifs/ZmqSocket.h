/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _ZmqSocket_base_H_
#define _ZmqSocket_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class zmq_base;
class Buffer_base;

class ZmqSocket_base : public object_base
{
    DECLARE_CLASS(ZmqSocket_base);

public:
    // ZmqSocket_base
    static result_t _new(int32_t type, obj_ptr<ZmqSocket_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t bind(exlib::string addr) = 0;
    virtual result_t connect(exlib::string addr) = 0;
    virtual result_t recv(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t send(Buffer_base* data) = 0;
    virtual result_t close() = 0;
    virtual result_t get_type(int32_t& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bind(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_connect(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_recv(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_send(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);

public:
    ASYNC_MEMBERVALUE1(ZmqSocket_base, recv, obj_ptr<Buffer_base>);
};

}

#include "zmq.h"
#include "Buffer.h"

namespace fibjs
{
    inline ClassInfo& ZmqSocket_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"bind", s_bind, false},
            {"connect", s_connect, false},
            {"recv", s_recv, false},
            {"send", s_send, false},
            {"close", s_close, false}
        };

        static ClassData::ClassProperty s_property[] = 
        {
            {"type", s_get_type, block_set, false}
        };

        static ClassData s_cd = 
        { 
            "ZmqSocket", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void ZmqSocket_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void ZmqSocket_base::__new(const T& args)
    {
        obj_ptr<ZmqSocket_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(1, 0);

        OPT_ARG(int32_t, 0, zmq_base::_PAIR);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void ZmqSocket_base::s_bind(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZmqSocket_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->bind(v0);

        METHOD_VOID();
    }

    inline void ZmqSocket_base::s_connect(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZmqSocket_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->connect(v0);

        METHOD_VOID();
    }

    inline void ZmqSocket_base::s_recv(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(ZmqSocket_base);
        METHOD_ENTER();

        ASYNC_METHOD_OVER(0, 0);

        if(!cb.IsEmpty()) {
            pInst->acb_recv(cb);
            hr = CALL_RETURN_NULL;
        } else
            hr = pInst->ac_recv(vr);

        METHOD_RETURN();
    }

    inline void ZmqSocket_base::s_send(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZmqSocket_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->send(v0);

        METHOD_VOID();
    }

    inline void ZmqSocket_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(ZmqSocket_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->close();

        METHOD_VOID();
    }

    inline void ZmqSocket_base::s_get_type(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        int32_t vr;

        METHOD_INSTANCE(ZmqSocket_base);
        PROPERTY_ENTER();

        hr = pInst->get_type(vr);

        METHOD_RETURN();
    }

}

#endif

