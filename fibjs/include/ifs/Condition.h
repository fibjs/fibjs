/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Condition_base_H_
#define _Condition_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Lock.h"

namespace fibjs
{

class Lock_base;

class Condition_base : public Lock_base
{
    DECLARE_CLASS(Condition_base);

public:
    // Condition_base
    static result_t _new(obj_ptr<Condition_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    static result_t _new(Lock_base* lock, obj_ptr<Condition_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t wait() = 0;
    virtual result_t notify() = 0;
    virtual result_t notifyAll() = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_notify(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_notifyAll(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& Condition_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"wait", s_wait, false},
            {"notify", s_notify, false},
            {"notifyAll", s_notifyAll, false}
        };

        static ClassData s_cd = 
        { 
            "Condition", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
            &Lock_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Condition_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Condition_base::__new(const T& args)
    {
        obj_ptr<Condition_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(0, 0);

        hr = _new(vr, args.This());

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Lock_base>, 0);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void Condition_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Condition_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->wait();

        METHOD_VOID();
    }

    inline void Condition_base::s_notify(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Condition_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->notify();

        METHOD_VOID();
    }

    inline void Condition_base::s_notifyAll(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Condition_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->notifyAll();

        METHOD_VOID();
    }

}

#endif

