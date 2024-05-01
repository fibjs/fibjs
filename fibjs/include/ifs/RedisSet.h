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

class RedisSet_base : public object_base {
    DECLARE_CLASS(RedisSet_base);

public:
    // RedisSet_base
    virtual result_t add(v8::Local<v8::Array> members, int32_t& retVal) = 0;
    virtual result_t add(OptArgs members, int32_t& retVal) = 0;
    virtual result_t remove(v8::Local<v8::Array> members, int32_t& retVal) = 0;
    virtual result_t remove(OptArgs members, int32_t& retVal) = 0;
    virtual result_t len(int32_t& retVal) = 0;
    virtual result_t exists(Buffer_base* member, bool& retVal) = 0;
    virtual result_t members(obj_ptr<NArray>& retVal) = 0;
    virtual result_t pop(obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t randMember(v8::Local<v8::Value>& retVal) = 0;
    virtual result_t randMember(int32_t count, v8::Local<v8::Value>& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_add(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_len(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_members(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pop(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_randMember(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "ifs/Buffer.h"

namespace fibjs {
inline ClassInfo& RedisSet_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "add", s_add, false, ClassData::ASYNC_SYNC },
        { "remove", s_remove, false, ClassData::ASYNC_SYNC },
        { "len", s_len, false, ClassData::ASYNC_SYNC },
        { "exists", s_exists, false, ClassData::ASYNC_SYNC },
        { "members", s_members, false, ClassData::ASYNC_SYNC },
        { "pop", s_pop, false, ClassData::ASYNC_SYNC },
        { "randMember", s_randMember, false, ClassData::ASYNC_SYNC }
    };

    static ClassData s_cd = {
        "RedisSet", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RedisSet_base::s_add(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->add(v0, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->add(v0, vr);

    METHOD_RETURN();
}

inline void RedisSet_base::s_remove(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->remove(v0, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->remove(v0, vr);

    METHOD_RETURN();
}

inline void RedisSet_base::s_len(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->len(vr);

    METHOD_RETURN();
}

inline void RedisSet_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->exists(v0, vr);

    METHOD_RETURN();
}

inline void RedisSet_base::s_members(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->members(vr);

    METHOD_RETURN();
}

inline void RedisSet_base::s_pop(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->pop(vr);

    METHOD_RETURN();
}

inline void RedisSet_base::s_randMember(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Value> vr;

    METHOD_INSTANCE(RedisSet_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->randMember(vr);

    METHOD_OVER(1, 1);

    ARG(int32_t, 0);

    hr = pInst->randMember(v0, vr);

    METHOD_RETURN();
}
}
