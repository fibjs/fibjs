/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RpcTask_base_H_
#define _RpcTask_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class RpcTask_base : public object_base
{
    DECLARE_CLASS(RpcTask_base);

public:
    // RpcTask_base
    static result_t _new(const char* id, obj_ptr<RpcTask_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t _function(const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t _named_getter(const char* property, obj_ptr<RpcTask_base>& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s__function(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args);
};

}

namespace fibjs
{
    inline ClassInfo& RpcTask_base::class_info()
    {
        static ClassData::ClassNamed s_named = 
        {
            i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
        };

        static ClassData s_cd = 
        { 
            "RpcTask", s__new, s__function, 
            0, NULL, 0, NULL, 0, NULL, NULL, &s_named,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void RpcTask_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<RpcTask_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(RpcTask_base);

        v8::String::Utf8Value k(property);
        if(class_info().has(*k))return;

        hr = pInst->_named_getter(*k, vr);
        if(hr == CALL_RETURN_NULL)return;

        METHOD_RETURN();
    }

    inline void RpcTask_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args)
    {
        v8::Local<v8::Array> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(RpcTask_base);

        hr = pInst->_named_enumerator(vr);

        METHOD_RETURN1();
    }

    inline void RpcTask_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void RpcTask_base::__new(const T& args)
    {
        obj_ptr<RpcTask_base> vr;

        CONSTRUCT_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void RpcTask_base::s__function(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_INSTANCE(RpcTask_base);
        METHOD_ENTER(-1, 0);

        hr = pInst->_function(args, vr);

        METHOD_RETURN();
    }

}

#endif

