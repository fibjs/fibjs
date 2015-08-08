/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _LevelDB_base_H_
#define _LevelDB_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class List_base;

class LevelDB_base : public object_base
{
    DECLARE_CLASS(LevelDB_base);

public:
    // LevelDB_base
    virtual result_t has(Buffer_base* key, bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t get(Buffer_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<List_base>& retVal) = 0;
    virtual result_t set(Buffer_base* key, Buffer_base* value, AsyncEvent* ac) = 0;
    virtual result_t mset(v8::Local<v8::Object> map) = 0;
    virtual result_t remove(v8::Local<v8::Array> keys) = 0;
    virtual result_t remove(Buffer_base* key, AsyncEvent* ac) = 0;
    virtual result_t forEach(v8::Local<v8::Function> func) = 0;
    virtual result_t between(Buffer_base* from, Buffer_base* to, v8::Local<v8::Function> func) = 0;
    virtual result_t begin(obj_ptr<LevelDB_base>& retVal) = 0;
    virtual result_t commit() = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mget(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_between(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_commit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(LevelDB_base, has, Buffer_base*, bool);
    ASYNC_MEMBERVALUE2(LevelDB_base, get, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBER2(LevelDB_base, set, Buffer_base*, Buffer_base*);
    ASYNC_MEMBER1(LevelDB_base, remove, Buffer_base*);
    ASYNC_MEMBER0(LevelDB_base, close);
};

}

#include "Buffer.h"
#include "List.h"

namespace fibjs
{
    inline ClassInfo& LevelDB_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"has", s_has, false},
            {"get", s_get, false},
            {"mget", s_mget, false},
            {"set", s_set, false},
            {"mset", s_mset, false},
            {"remove", s_remove, false},
            {"forEach", s_forEach, false},
            {"between", s_between, false},
            {"begin", s_begin, false},
            {"commit", s_commit, false},
            {"close", s_close, false}
        };

        static ClassData s_cd = 
        { 
            "LevelDB", NULL, 
            11, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }


    inline void LevelDB_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->ac_has(v0, vr);

        METHOD_RETURN();
    }

    inline void LevelDB_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->ac_get(v0, vr);

        METHOD_RETURN();
    }

    inline void LevelDB_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->mget(v0, vr);

        METHOD_RETURN();
    }

    inline void LevelDB_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->ac_set(v0, v1);

        METHOD_VOID();
    }

    inline void LevelDB_base::s_mset(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->mset(v0);

        METHOD_VOID();
    }

    inline void LevelDB_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->remove(v0);

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->ac_remove(v0);

        METHOD_VOID();
    }

    inline void LevelDB_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->forEach(v0);

        METHOD_VOID();
    }

    inline void LevelDB_base::s_between(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(3, 3);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);
        ARG(v8::Local<v8::Function>, 2);

        hr = pInst->between(v0, v1, v2);

        METHOD_VOID();
    }

    inline void LevelDB_base::s_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<LevelDB_base> vr;

        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(0, 0);

        hr = pInst->begin(vr);

        METHOD_RETURN();
    }

    inline void LevelDB_base::s_commit(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(0, 0);

        hr = pInst->commit();

        METHOD_VOID();
    }

    inline void LevelDB_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(LevelDB_base);
        METHOD_ENTER(0, 0);

        hr = pInst->ac_close();

        METHOD_VOID();
    }

}

#endif

