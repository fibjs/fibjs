/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RedisList_base_H_
#define _RedisList_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class List_base;

class RedisList_base : public object_base
{
    DECLARE_CLASS(RedisList_base);

public:
    // RedisList_base
    virtual result_t push(v8::Local<v8::Array> values, int32_t& retVal) = 0;
    virtual result_t push(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
    virtual result_t pop(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t rpush(v8::Local<v8::Array> values, int32_t& retVal) = 0;
    virtual result_t rpush(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
    virtual result_t rpop(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t set(int32_t index, Buffer_base* value) = 0;
    virtual result_t get(int32_t index, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t insertBefore(Buffer_base* pivot, Buffer_base* value, int32_t& retVal) = 0;
    virtual result_t insertAfter(Buffer_base* pivot, Buffer_base* value, int32_t& retVal) = 0;
    virtual result_t remove(int32_t count, Buffer_base* value, int32_t& retVal) = 0;
    virtual result_t trim(int32_t start, int32_t stop) = 0;
    virtual result_t len(int32_t& retVal) = 0;
    virtual result_t range(int32_t start, int32_t stop, obj_ptr<List_base>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_push(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rpush(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rpop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_insertAfter(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_trim(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_len(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_range(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "List.h"

namespace fibjs
{
    inline ClassInfo& RedisList_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"push", s_push, false},
            {"pop", s_pop, false},
            {"rpush", s_rpush, false},
            {"rpop", s_rpop, false},
            {"set", s_set, false},
            {"get", s_get, false},
            {"insertBefore", s_insertBefore, false},
            {"insertAfter", s_insertAfter, false},
            {"remove", s_remove, false},
            {"trim", s_trim, false},
            {"len", s_len, false},
            {"range", s_range, false}
        };

        static ClassData s_cd = 
        { 
            "RedisList", s__new, NULL, 
            12, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void RedisList_base::s_push(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->push(v0, vr);

        METHOD_OVER(-1, 0);

        hr = pInst->push(args, vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_pop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(0, 0);

        hr = pInst->pop(vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_rpush(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->rpush(v0, vr);

        METHOD_OVER(-1, 0);

        hr = pInst->rpush(args, vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_rpop(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(0, 0);

        hr = pInst->rpop(vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->set(v0, v1);

        METHOD_VOID();
    }

    inline void RedisList_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(1, 1);

        ARG(int32_t, 0);

        hr = pInst->get(v0, vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_insertBefore(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->insertBefore(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_insertAfter(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->insertAfter(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->remove(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_trim(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(int32_t, 1);

        hr = pInst->trim(v0, v1);

        METHOD_VOID();
    }

    inline void RedisList_base::s_len(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(0, 0);

        hr = pInst->len(vr);

        METHOD_RETURN();
    }

    inline void RedisList_base::s_range(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(RedisList_base);
        METHOD_ENTER(2, 2);

        ARG(int32_t, 0);
        ARG(int32_t, 1);

        hr = pInst->range(v0, v1, vr);

        METHOD_RETURN();
    }

}

#endif

