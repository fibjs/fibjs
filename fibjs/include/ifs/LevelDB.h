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

namespace fibjs {

class Buffer_base;

class LevelDB_base : public object_base {
    DECLARE_CLASS(LevelDB_base);

public:
    // LevelDB_base
    virtual result_t has(Buffer_base* key, bool& retVal, AsyncEvent* ac) = 0;
    virtual result_t get(Buffer_base* key, obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<NArray>& retVal) = 0;
    virtual result_t set(Buffer_base* key, Buffer_base* value, AsyncEvent* ac) = 0;
    virtual result_t mset(v8::Local<v8::Object> map) = 0;
    virtual result_t mremove(v8::Local<v8::Array> keys) = 0;
    virtual result_t remove(Buffer_base* key, AsyncEvent* ac) = 0;
    virtual result_t firstKey(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t lastKey(obj_ptr<Buffer_base>& retVal, AsyncEvent* ac) = 0;
    virtual result_t forEach(v8::Local<v8::Function> func) = 0;
    virtual result_t forEach(Buffer_base* from, v8::Local<v8::Function> func) = 0;
    virtual result_t forEach(Buffer_base* from, Buffer_base* to, v8::Local<v8::Function> func) = 0;
    virtual result_t forEach(v8::Local<v8::Object> opt, v8::Local<v8::Function> func) = 0;
    virtual result_t forEach(Buffer_base* from, v8::Local<v8::Object> opt, v8::Local<v8::Function> func) = 0;
    virtual result_t forEach(Buffer_base* from, Buffer_base* to, v8::Local<v8::Object> opt, v8::Local<v8::Function> func) = 0;
    virtual result_t begin(obj_ptr<LevelDB_base>& retVal) = 0;
    virtual result_t commit() = 0;
    virtual result_t close(AsyncEvent* ac) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_has(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mget(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mremove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_firstKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_lastKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_begin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_commit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);

public:
    ASYNC_MEMBERVALUE2(LevelDB_base, has, Buffer_base*, bool);
    ASYNC_MEMBERVALUE2(LevelDB_base, get, Buffer_base*, obj_ptr<Buffer_base>);
    ASYNC_MEMBER2(LevelDB_base, set, Buffer_base*, Buffer_base*);
    ASYNC_MEMBER1(LevelDB_base, remove, Buffer_base*);
    ASYNC_MEMBERVALUE1(LevelDB_base, firstKey, obj_ptr<Buffer_base>);
    ASYNC_MEMBERVALUE1(LevelDB_base, lastKey, obj_ptr<Buffer_base>);
    ASYNC_MEMBER0(LevelDB_base, close);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& LevelDB_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "has", s_has, false, ClassData::ASYNC_ASYNC },
        { "hasSync", s_has, false, ClassData::ASYNC_SYNC },
        { "get", s_get, false, ClassData::ASYNC_ASYNC },
        { "getSync", s_get, false, ClassData::ASYNC_SYNC },
        { "mget", s_mget, false, ClassData::ASYNC_SYNC },
        { "set", s_set, false, ClassData::ASYNC_ASYNC },
        { "setSync", s_set, false, ClassData::ASYNC_SYNC },
        { "mset", s_mset, false, ClassData::ASYNC_SYNC },
        { "mremove", s_mremove, false, ClassData::ASYNC_SYNC },
        { "remove", s_remove, false, ClassData::ASYNC_ASYNC },
        { "removeSync", s_remove, false, ClassData::ASYNC_SYNC },
        { "firstKey", s_firstKey, false, ClassData::ASYNC_ASYNC },
        { "firstKeySync", s_firstKey, false, ClassData::ASYNC_SYNC },
        { "lastKey", s_lastKey, false, ClassData::ASYNC_ASYNC },
        { "lastKeySync", s_lastKey, false, ClassData::ASYNC_SYNC },
        { "forEach", s_forEach, false, ClassData::ASYNC_SYNC },
        { "begin", s_begin, false, ClassData::ASYNC_SYNC },
        { "commit", s_commit, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_ASYNC },
        { "closeSync", s_close, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "LevelDB", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        true
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void LevelDB_base::s_has(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_has(v0, cb, args);
    else
        hr = pInst->ac_has(v0, vr);

    METHOD_RETURN();
}

inline void LevelDB_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_get(v0, cb, args);
    else
        hr = pInst->ac_get(v0, vr);

    METHOD_RETURN();
}

inline void LevelDB_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->mget(v0, vr);

    METHOD_RETURN();
}

inline void LevelDB_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    if (!cb.IsEmpty())
        hr = pInst->acb_set(v0, v1, cb, args);
    else
        hr = pInst->ac_set(v0, v1);

    METHOD_VOID();
}

inline void LevelDB_base::s_mset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->mset(v0);

    METHOD_VOID();
}

inline void LevelDB_base::s_mremove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->mremove(v0);

    METHOD_VOID();
}

inline void LevelDB_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_remove(v0, cb, args);
    else
        hr = pInst->ac_remove(v0);

    METHOD_VOID();
}

inline void LevelDB_base::s_firstKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_firstKey(cb, args);
    else
        hr = pInst->ac_firstKey(vr);

    METHOD_RETURN();
}

inline void LevelDB_base::s_lastKey(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_lastKey(cb, args);
    else
        hr = pInst->ac_lastKey(vr);

    METHOD_RETURN();
}

inline void LevelDB_base::s_forEach(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->forEach(v0);

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->forEach(v0, v1);

    METHOD_OVER(3, 3);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = pInst->forEach(v0, v1, v2);

    METHOD_OVER(2, 2);

    ARG(v8::Local<v8::Object>, 0);
    ARG(v8::Local<v8::Function>, 1);

    hr = pInst->forEach(v0, v1);

    METHOD_OVER(3, 3);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(v8::Local<v8::Object>, 1);
    ARG(v8::Local<v8::Function>, 2);

    hr = pInst->forEach(v0, v1, v2);

    METHOD_OVER(4, 4);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);
    ARG(v8::Local<v8::Object>, 2);
    ARG(v8::Local<v8::Function>, 3);

    hr = pInst->forEach(v0, v1, v2, v3);

    METHOD_VOID();
}

inline void LevelDB_base::s_begin(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<LevelDB_base> vr;

    METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->begin(vr);

    METHOD_RETURN();
}

inline void LevelDB_base::s_commit(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->commit();

    METHOD_VOID();
}

inline void LevelDB_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    ASYNC_METHOD_INSTANCE(LevelDB_base);
    METHOD_ENTER();

    ASYNC_METHOD_OVER(0, 0);

    if (!cb.IsEmpty())
        hr = pInst->acb_close(cb, args);
    else
        hr = pInst->ac_close();

    METHOD_VOID();
}
}
