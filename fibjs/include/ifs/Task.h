/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Task_base_H_
#define _Task_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Task_base : public object_base
{
    DECLARE_CLASS(Task_base);

public:
    // Task_base
    static result_t _new(const char* id, obj_ptr<Task_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t _named_getter(const char* property, obj_ptr<Task_base>& retVal) = 0;
    virtual result_t _named_enumerator(v8::Local<v8::Array>& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args);
    static void i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args);
};

}

namespace fibjs
{
    inline ClassInfo& Task_base::class_info()
    {
        static ClassData::ClassNamed s_named = 
        {
            i_NamedGetter, i_NamedSetter, i_NamedDeleter, i_NamedEnumerator
        };

        static ClassData s_cd = 
        { 
            "Task", s__new, NULL, 
            0, NULL, 0, NULL, 0, NULL, NULL, &s_named,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Task_base::i_NamedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &args)
    {
        obj_ptr<Task_base> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Task_base);

        v8::String::Utf8Value k(property);
        if(class_info().has(*k))return;

        hr = pInst->_named_getter(*k, vr);
        if(hr == CALL_RETURN_NULL)return;

        METHOD_RETURN();
    }

    inline void Task_base::i_NamedEnumerator(const v8::PropertyCallbackInfo<v8::Array> &args)
    {
        v8::Local<v8::Array> vr;

        PROPERTY_ENTER();
        PROPERTY_INSTANCE(Task_base);

        hr = pInst->_named_enumerator(vr);

        METHOD_RETURN1();
    }

    inline void Task_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Task_base::__new(const T& args)
    {
        obj_ptr<Task_base> vr;

        CONSTRUCT_ENTER(1, 1);

        ARG(arg_string, 0);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

}

#endif

