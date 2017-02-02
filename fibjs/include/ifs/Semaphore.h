/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Semaphore_base_H_
#define _Semaphore_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"
#include "Lock.h"

namespace fibjs
{

class Lock_base;

class Semaphore_base : public Lock_base
{
    DECLARE_CLASS(Semaphore_base);

public:
    // Semaphore_base
    static result_t _new(int32_t value, obj_ptr<Semaphore_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t wait() = 0;
    virtual result_t post() = 0;
    virtual result_t trywait(bool& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_wait(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_post(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_trywait(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}


namespace fibjs
{
    inline ClassInfo& Semaphore_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"wait", s_wait, false},
            {"post", s_post, false},
            {"trywait", s_trywait, false}
        };

        static ClassData s_cd = 
        { 
            "Semaphore", s__new, NULL, 
            3, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &Lock_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void Semaphore_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Semaphore_base::__new(const T& args)
    {
        obj_ptr<Semaphore_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(1, 0);

        OPT_ARG(int32_t, 0, 1);

        hr = _new(v0, vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void Semaphore_base::s_wait(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Semaphore_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->wait();

        METHOD_VOID();
    }

    inline void Semaphore_base::s_post(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Semaphore_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->post();

        METHOD_VOID();
    }

    inline void Semaphore_base::s_trywait(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Semaphore_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->trywait(vr);

        METHOD_RETURN();
    }

}

#endif

