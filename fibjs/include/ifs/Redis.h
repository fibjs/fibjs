/***************************************************************************
 *                                                                         *
 *   This file was automatically generated using idlc.js                   *
 *   PLEASE DO NOT EDIT!!!!                                                *
 *                                                                         *
 ***************************************************************************/

#ifndef _Redis_base_H_
#define _Redis_base_H_

/**
 @author Leo Hoo <lion@9465.net>
 */

#include "../object.h"

namespace fibjs
{

class Buffer_base;
class List_base;
class RedisHash_base;
class RedisList_base;
class RedisSet_base;
class RedisSortedSet_base;

class Redis_base : public object_base
{
    DECLARE_CLASS(Redis_base);

public:
    // Redis_base
    virtual result_t command(exlib::string cmd, const v8::FunctionCallbackInfo<v8::Value>& args, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t set(Buffer_base* key, Buffer_base* value, int64_t ttl) = 0;
    virtual result_t setNX(Buffer_base* key, Buffer_base* value, int64_t ttl) = 0;
    virtual result_t setXX(Buffer_base* key, Buffer_base* value, int64_t ttl) = 0;
    virtual result_t mset(v8::Local<v8::Object> kvs) = 0;
    virtual result_t mset(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
    virtual result_t msetNX(v8::Local<v8::Object> kvs) = 0;
    virtual result_t msetNX(const v8::FunctionCallbackInfo<v8::Value>& args) = 0;
    virtual result_t append(Buffer_base* key, Buffer_base* value, int32_t& retVal) = 0;
    virtual result_t setRange(Buffer_base* key, int32_t offset, Buffer_base* value, int32_t& retVal) = 0;
    virtual result_t getRange(Buffer_base* key, int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t strlen(Buffer_base* key, int32_t& retVal) = 0;
    virtual result_t bitcount(Buffer_base* key, int32_t start, int32_t end, int32_t& retVal) = 0;
    virtual result_t get(Buffer_base* key, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<List_base>& retVal) = 0;
    virtual result_t mget(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<List_base>& retVal) = 0;
    virtual result_t getset(Buffer_base* key, Buffer_base* value, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t decr(Buffer_base* key, int64_t num, int64_t& retVal) = 0;
    virtual result_t incr(Buffer_base* key, int64_t num, int64_t& retVal) = 0;
    virtual result_t setBit(Buffer_base* key, int32_t offset, int32_t value, int32_t& retVal) = 0;
    virtual result_t getBit(Buffer_base* key, int32_t offset, int32_t& retVal) = 0;
    virtual result_t exists(Buffer_base* key, bool& retVal) = 0;
    virtual result_t type(Buffer_base* key, exlib::string& retVal) = 0;
    virtual result_t keys(exlib::string pattern, obj_ptr<List_base>& retVal) = 0;
    virtual result_t del(v8::Local<v8::Array> keys, int32_t& retVal) = 0;
    virtual result_t del(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal) = 0;
    virtual result_t expire(Buffer_base* key, int64_t ttl, bool& retVal) = 0;
    virtual result_t ttl(Buffer_base* key, int64_t& retVal) = 0;
    virtual result_t persist(Buffer_base* key, bool& retVal) = 0;
    virtual result_t rename(Buffer_base* key, Buffer_base* newkey) = 0;
    virtual result_t renameNX(Buffer_base* key, Buffer_base* newkey, bool& retVal) = 0;
    virtual result_t sub(Buffer_base* channel, v8::Local<v8::Function> func) = 0;
    virtual result_t sub(v8::Local<v8::Object> map) = 0;
    virtual result_t unsub(Buffer_base* channel) = 0;
    virtual result_t unsub(Buffer_base* channel, v8::Local<v8::Function> func) = 0;
    virtual result_t unsub(v8::Local<v8::Array> channels) = 0;
    virtual result_t unsub(v8::Local<v8::Object> map) = 0;
    virtual result_t psub(exlib::string pattern, v8::Local<v8::Function> func) = 0;
    virtual result_t psub(v8::Local<v8::Object> map) = 0;
    virtual result_t unpsub(exlib::string pattern) = 0;
    virtual result_t unpsub(exlib::string pattern, v8::Local<v8::Function> func) = 0;
    virtual result_t unpsub(v8::Local<v8::Array> patterns) = 0;
    virtual result_t unpsub(v8::Local<v8::Object> map) = 0;
    virtual result_t onsuberror(v8::Local<v8::Function> func) = 0;
    virtual result_t pub(Buffer_base* channel, Buffer_base* message, int32_t& retVal) = 0;
    virtual result_t getHash(Buffer_base* key, obj_ptr<RedisHash_base>& retVal) = 0;
    virtual result_t getList(Buffer_base* key, obj_ptr<RedisList_base>& retVal) = 0;
    virtual result_t getSet(Buffer_base* key, obj_ptr<RedisSet_base>& retVal) = 0;
    virtual result_t getSortedSet(Buffer_base* key, obj_ptr<RedisSortedSet_base>& retVal) = 0;
    virtual result_t dump(Buffer_base* key, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t restore(Buffer_base* key, Buffer_base* data, int64_t ttl) = 0;
    virtual result_t close() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        Isolate* isolate = Isolate::current();

        isolate->m_isolate->ThrowException(
            isolate->NewFromUtf8("not a constructor"));
    }

public:
    static void s_command(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setXX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_msetNX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_append(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setRange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getRange(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_strlen(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_bitcount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_get(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_mget(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getset(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_decr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_incr(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_setBit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getBit(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_exists(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_type(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_keys(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_del(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_expire(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_ttl(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_persist(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_rename(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_renameNX(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_sub(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unsub(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_psub(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_unpsub(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_onsuberror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_pub(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getHash(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getList(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSet(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_getSortedSet(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_dump(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_restore(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_close(const v8::FunctionCallbackInfo<v8::Value>& args);
};

}

#include "Buffer.h"
#include "List.h"
#include "RedisHash.h"
#include "RedisList.h"
#include "RedisSet.h"
#include "RedisSortedSet.h"

namespace fibjs
{
    inline ClassInfo& Redis_base::class_info()
    {
        static ClassData::ClassMethod s_method[] = 
        {
            {"command", s_command, false},
            {"set", s_set, false},
            {"setNX", s_setNX, false},
            {"setXX", s_setXX, false},
            {"mset", s_mset, false},
            {"msetNX", s_msetNX, false},
            {"append", s_append, false},
            {"setRange", s_setRange, false},
            {"getRange", s_getRange, false},
            {"strlen", s_strlen, false},
            {"bitcount", s_bitcount, false},
            {"get", s_get, false},
            {"mget", s_mget, false},
            {"getset", s_getset, false},
            {"decr", s_decr, false},
            {"incr", s_incr, false},
            {"setBit", s_setBit, false},
            {"getBit", s_getBit, false},
            {"exists", s_exists, false},
            {"type", s_type, false},
            {"keys", s_keys, false},
            {"del", s_del, false},
            {"expire", s_expire, false},
            {"ttl", s_ttl, false},
            {"persist", s_persist, false},
            {"rename", s_rename, false},
            {"renameNX", s_renameNX, false},
            {"sub", s_sub, false},
            {"unsub", s_unsub, false},
            {"psub", s_psub, false},
            {"unpsub", s_unpsub, false},
            {"onsuberror", s_onsuberror, false},
            {"pub", s_pub, false},
            {"getHash", s_getHash, false},
            {"getList", s_getList, false},
            {"getSet", s_getSet, false},
            {"getSortedSet", s_getSortedSet, false},
            {"dump", s_dump, false},
            {"restore", s_restore, false},
            {"close", s_close, false}
        };

        static ClassData s_cd = 
        { 
            "Redis", s__new, NULL, 
            40, s_method, 0, NULL, 0, NULL, NULL, NULL,
            &object_base::class_info()
        };

        static ClassInfo s_ci(s_cd);
        return s_ci;
    }

    inline void Redis_base::s_command(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        v8::Local<v8::Value> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(-1, 1);

        ARG(exlib::string, 0);

        hr = pInst->command(v0, args, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);
        OPT_ARG(int64_t, 2, 0);

        hr = pInst->set(v0, v1, v2);

        METHOD_VOID();
    }

    inline void Redis_base::s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);
        OPT_ARG(int64_t, 2, 0);

        hr = pInst->setNX(v0, v1, v2);

        METHOD_VOID();
    }

    inline void Redis_base::s_setXX(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);
        OPT_ARG(int64_t, 2, 0);

        hr = pInst->setXX(v0, v1, v2);

        METHOD_VOID();
    }

    inline void Redis_base::s_mset(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->mset(v0);

        METHOD_OVER(-1, 0);

        hr = pInst->mset(args);

        METHOD_VOID();
    }

    inline void Redis_base::s_msetNX(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->msetNX(v0);

        METHOD_OVER(-1, 0);

        hr = pInst->msetNX(args);

        METHOD_VOID();
    }

    inline void Redis_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->append(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_setRange(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 3);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(int32_t, 1);
        ARG(obj_ptr<Buffer_base>, 2);

        hr = pInst->setRange(v0, v1, v2, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getRange(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 3);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(int32_t, 1);
        ARG(int32_t, 2);

        hr = pInst->getRange(v0, v1, v2, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_strlen(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->strlen(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_bitcount(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 1);

        ARG(obj_ptr<Buffer_base>, 0);
        OPT_ARG(int32_t, 1, 0);
        OPT_ARG(int32_t, 2, -1);

        hr = pInst->bitcount(v0, v1, v2, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_get(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->get(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->mget(v0, vr);

        METHOD_OVER(-1, 0);

        hr = pInst->mget(args, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getset(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->getset(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_decr(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int64_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(obj_ptr<Buffer_base>, 0);
        OPT_ARG(int64_t, 1, 1);

        hr = pInst->decr(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_incr(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int64_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 1);

        ARG(obj_ptr<Buffer_base>, 0);
        OPT_ARG(int64_t, 1, 1);

        hr = pInst->incr(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_setBit(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 3);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(int32_t, 1);
        ARG(int32_t, 2);

        hr = pInst->setBit(v0, v1, v2, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getBit(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(int32_t, 1);

        hr = pInst->getBit(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_exists(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->exists(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_type(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        exlib::string vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->type(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<List_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->keys(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_del(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->del(v0, vr);

        METHOD_OVER(-1, 0);

        hr = pInst->del(args, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_expire(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(int64_t, 1);

        hr = pInst->expire(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_ttl(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int64_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->ttl(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_persist(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->persist(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->rename(v0, v1);

        METHOD_VOID();
    }

    inline void Redis_base::s_renameNX(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        bool vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->renameNX(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_sub(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = pInst->sub(v0, v1);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->sub(v0);

        METHOD_VOID();
    }

    inline void Redis_base::s_unsub(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->unsub(v0);

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = pInst->unsub(v0, v1);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->unsub(v0);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->unsub(v0);

        METHOD_VOID();
    }

    inline void Redis_base::s_psub(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = pInst->psub(v0, v1);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->psub(v0);

        METHOD_VOID();
    }

    inline void Redis_base::s_unpsub(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(exlib::string, 0);

        hr = pInst->unpsub(v0);

        METHOD_OVER(2, 2);

        ARG(exlib::string, 0);
        ARG(v8::Local<v8::Function>, 1);

        hr = pInst->unpsub(v0, v1);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Array>, 0);

        hr = pInst->unpsub(v0);

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Object>, 0);

        hr = pInst->unpsub(v0);

        METHOD_VOID();
    }

    inline void Redis_base::s_onsuberror(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(v8::Local<v8::Function>, 0);

        hr = pInst->onsuberror(v0);

        METHOD_VOID();
    }

    inline void Redis_base::s_pub(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        int32_t vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(2, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);

        hr = pInst->pub(v0, v1, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getHash(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<RedisHash_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->getHash(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getList(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<RedisList_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->getList(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getSet(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<RedisSet_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->getSet(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_getSortedSet(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<RedisSortedSet_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->getSortedSet(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_dump(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        obj_ptr<Buffer_base> vr;

        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(1, 1);

        ARG(obj_ptr<Buffer_base>, 0);

        hr = pInst->dump(v0, vr);

        METHOD_RETURN();
    }

    inline void Redis_base::s_restore(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(3, 2);

        ARG(obj_ptr<Buffer_base>, 0);
        ARG(obj_ptr<Buffer_base>, 1);
        OPT_ARG(int64_t, 2, 0);

        hr = pInst->restore(v0, v1, v2);

        METHOD_VOID();
    }

    inline void Redis_base::s_close(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        METHOD_INSTANCE(Redis_base);
        METHOD_ENTER();

        METHOD_OVER(0, 0);

        hr = pInst->close();

        METHOD_VOID();
    }

}

#endif

