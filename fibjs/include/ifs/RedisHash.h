/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _RedisHash_base_H_
#define _RedisHash_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs {

class Buffer_base;

class RedisHash_base : public object_base {
    DECLARE_CLASS(RedisHash_base);

public:
    // RedisHash_base
    virtual result_t set(Buffer_base* field, Buffer_base* value) = 0;
    virtual result_t setNX(Buffer_base* field, Buffer_base* value) = 0;
    virtual result_t mset(v8::Local<v8::Object> kvs) = 0;
    virtual result_t mset(OptArgs kvs) = 0;
    virtual result_t get(Buffer_base* field, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t mget(v8::Local<v8::Array> fields, obj_ptr<NArray>& retVal) = 0;
    virtual result_t mget(OptArgs fields, obj_ptr<NArray>& retVal) = 0;
    virtual result_t incr(Buffer_base* field, int64_t num, int64_t& retVal) = 0;
    virtual result_t getAll(obj_ptr<NArray>& retVal) = 0;
    virtual result_t keys(obj_ptr<NArray>& retVal) = 0;
    virtual result_t len(int32_t& retVal) = 0;
    virtual result_t exists(Buffer_base* field, bool& retVal) = 0;
    virtual result_t del(v8::Local<v8::Array> fields, int32_t& retVal) = 0;
    virtual result_t del(OptArgs fields, int32_t& retVal) = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewString("not a constructor"));
    }

public:
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mget(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_incr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getAll(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_len(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
};
}

#include "Buffer.h"

namespace fibjs {
inline ClassInfo& RedisHash_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "set", s_set, false },
        { "setNX", s_setNX, false },
        { "mset", s_mset, false },
        { "get", s_get, false },
        { "mget", s_mget, false },
        { "incr", s_incr, false },
        { "getAll", s_getAll, false },
        { "keys", s_keys, false },
        { "len", s_len, false },
        { "exists", s_exists, false },
        { "del", s_del, false }
    };

    static ClassData s_cd = {
        "RedisHash", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, 0, NULL, NULL, NULL,
        &object_base::class_info()
    };

    static ClassInfo s_ci(s_cd);
    return s_ci;
}

inline void RedisHash_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RedisHash.set");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = pInst->set(v0, v1);

    METHOD_VOID();
}

inline void RedisHash_base::s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RedisHash.setNX");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(obj_ptr<Buffer_base>, 0);
    ARG(obj_ptr<Buffer_base>, 1);

    hr = pInst->setNX(v0, v1);

    METHOD_VOID();
}

inline void RedisHash_base::s_mset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_NAME("RedisHash.mset");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Object>, 0);

    hr = pInst->mset(v0);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->mset(v0);

    METHOD_VOID();
}

inline void RedisHash_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_NAME("RedisHash.get");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->get(v0, vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("RedisHash.mget");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->mget(v0, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->mget(v0, vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_incr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_NAME("RedisHash.incr");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(obj_ptr<Buffer_base>, 0);
    OPT_ARG(int64_t, 1, 1);

    hr = pInst->incr(v0, v1, vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_getAll(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("RedisHash.getAll");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getAll(vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_NAME("RedisHash.keys");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->keys(vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_len(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("RedisHash.len");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->len(vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_NAME("RedisHash.exists");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(obj_ptr<Buffer_base>, 0);

    hr = pInst->exists(v0, vr);

    METHOD_RETURN();
}

inline void RedisHash_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_NAME("RedisHash.del");
    METHOD_INSTANCE(RedisHash_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->del(v0, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->del(v0, vr);

    METHOD_RETURN();
}
}

#endif
