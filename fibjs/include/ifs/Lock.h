/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Lock_base_H_
#define _Lock_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Lock_base : public object_base
{
    DECLARE_CLASS(Lock_base);

public:
    // Lock_base
    static result_t _new(obj_ptr<Lock_base>& retVal, v8::Local<v8::Object> This = v8::Local<v8::Object>());
    virtual result_t acquire(bool blocking, bool& retVal) = 0;
    virtual result_t release() = 0;
    virtual result_t count(int32_t& retVal) = 0;

public:
    template<typename T>
    static void __new(const T &args);

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_acquire(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_release(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_count(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

namespace fibjs
{
    inline ClassInfo& Lock_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"acquire", s_acquire, false},
            {"release", s_release, false},
            {"count", s_count, false}
        };

        static ClassData s_cd = 
        { 
            "Lock", s__new, NULL, 
            ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Lock_base::s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();
        __new(args);
    }

    template<typename T>void Lock_base::__new(const T& args)
    {
        obj_ptr<Lock_base> vr;

        CONSTRUCT_ENTER();

        METHOD_OVER(0, 0);

        hr = _new(vr, args.This());

        CONSTRUCT_RETURN();
    }

    inline void Lock_base::s_acquire(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Lock_base);
        METHOD_ENTER();

        METHOD_OVER(1, 0);

        OPT_ARG(bool, 0, true);

        hr = pInst->acquire(v0, vr);

        METHOD_RETURN();
    }

    inline void Lock_base::s_release(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Lock_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->release();

        METHOD_VOID();
    }

    inline void Lock_base::s_count(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Lock_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->count(vr);

        METHOD_RETURN();
    }

}

#endif

