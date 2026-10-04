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
class RedisHash_base;
class RedisList_base;
class RedisSet_base;
class RedisSortedSet_base;

class Redis_base : public object_base {
    DECLARE_CLASS(Redis_base);

public:
    using Union_set_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_set_value = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setNX_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setNX_value = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setXX_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setXX_value = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_append_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_append_value = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setRange_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setRange_value = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getRange_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_strlen_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_bitcount_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_get_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getset_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getset_value = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_decr_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_incr_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_setBit_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getBit_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_exists_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_type_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_expire_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_ttl_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_persist_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_rename_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_rename_newkey = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_renameNX_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_renameNX_newkey = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_sub_channel = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_unsub_channel = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_pub_channel = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_pub_message = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getHash_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getList_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getSet_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_getSortedSet_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_dump_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_restore_key = std::variant<obj_ptr<Buffer_base>, exlib::string>;
    using Union_restore_data = std::variant<obj_ptr<Buffer_base>, exlib::string>;

public:
    // Redis_base
    virtual result_t command(exlib::string cmd, OptArgs args, v8::Local<v8::Value>& retVal) = 0;
    virtual result_t set(Union_set_key key, Union_set_value value, int64_t ttl) = 0;
    virtual result_t setNX(Union_setNX_key key, Union_setNX_value value, int64_t ttl) = 0;
    virtual result_t setXX(Union_setXX_key key, Union_setXX_value value, int64_t ttl) = 0;
    virtual result_t mset(v8::Local<v8::Object> kvs) = 0;
    virtual result_t mset(OptArgs kvs) = 0;
    virtual result_t msetNX(v8::Local<v8::Object> kvs) = 0;
    virtual result_t msetNX(OptArgs kvs) = 0;
    virtual result_t append(Union_append_key key, Union_append_value value, int32_t& retVal) = 0;
    virtual result_t setRange(Union_setRange_key key, int32_t offset, Union_setRange_value value, int32_t& retVal) = 0;
    virtual result_t getRange(Union_getRange_key key, int32_t start, int32_t end, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t strlen(Union_strlen_key key, int32_t& retVal) = 0;
    virtual result_t bitcount(Union_bitcount_key key, int32_t start, int32_t end, int32_t& retVal) = 0;
    virtual result_t get(Union_get_key key, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t mget(v8::Local<v8::Array> keys, obj_ptr<NArray>& retVal) = 0;
    virtual result_t mget(OptArgs keys, obj_ptr<NArray>& retVal) = 0;
    virtual result_t getset(Union_getset_key key, Union_getset_value value, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t decr(Union_decr_key key, int64_t num, int64_t& retVal) = 0;
    virtual result_t incr(Union_incr_key key, int64_t num, int64_t& retVal) = 0;
    virtual result_t setBit(Union_setBit_key key, int32_t offset, int32_t value, int32_t& retVal) = 0;
    virtual result_t getBit(Union_getBit_key key, int32_t offset, int32_t& retVal) = 0;
    virtual result_t exists(Union_exists_key key, bool& retVal) = 0;
    virtual result_t type(Union_type_key key, exlib::string& retVal) = 0;
    virtual result_t keys(exlib::string pattern, obj_ptr<NArray>& retVal) = 0;
    virtual result_t del(v8::Local<v8::Array> keys, int32_t& retVal) = 0;
    virtual result_t del(OptArgs keys, int32_t& retVal) = 0;
    virtual result_t expire(Union_expire_key key, int64_t ttl, bool& retVal) = 0;
    virtual result_t ttl(Union_ttl_key key, int64_t& retVal) = 0;
    virtual result_t persist(Union_persist_key key, bool& retVal) = 0;
    virtual result_t rename(Union_rename_key key, Union_rename_newkey newkey) = 0;
    virtual result_t renameNX(Union_renameNX_key key, Union_renameNX_newkey newkey, bool& retVal) = 0;
    virtual result_t sub(Union_sub_channel channel, v8::Local<v8::Function> func) = 0;
    virtual result_t sub(v8::Local<v8::Object> map) = 0;
    virtual result_t unsub(Union_unsub_channel channel) = 0;
    virtual result_t unsub(Union_unsub_channel channel, v8::Local<v8::Function> func) = 0;
    virtual result_t unsub(v8::Local<v8::Array> channels) = 0;
    virtual result_t unsub(v8::Local<v8::Object> map) = 0;
    virtual result_t psub(exlib::string pattern, v8::Local<v8::Function> func) = 0;
    virtual result_t psub(v8::Local<v8::Object> map) = 0;
    virtual result_t unpsub(exlib::string pattern) = 0;
    virtual result_t unpsub(exlib::string pattern, v8::Local<v8::Function> func) = 0;
    virtual result_t unpsub(v8::Local<v8::Array> patterns) = 0;
    virtual result_t unpsub(v8::Local<v8::Object> map) = 0;
    virtual result_t pub(Union_pub_channel channel, Union_pub_message message, int32_t& retVal) = 0;
    virtual result_t getHash(Union_getHash_key key, obj_ptr<RedisHash_base>& retVal) = 0;
    virtual result_t getList(Union_getList_key key, obj_ptr<RedisList_base>& retVal) = 0;
    virtual result_t getSet(Union_getSet_key key, obj_ptr<RedisSet_base>& retVal) = 0;
    virtual result_t getSortedSet(Union_getSortedSet_key key, obj_ptr<RedisSortedSet_base>& retVal) = 0;
    virtual result_t dump(Union_dump_key key, obj_ptr<Buffer_base>& retVal) = 0;
    virtual result_t restore(Union_restore_key key, Union_restore_data data, int64_t ttl) = 0;
    virtual result_t close() = 0;

public:
    static void s__new(const v8::FunctionCallbackInfo<v8::Value>& args)
    {
        CONSTRUCT_INIT();

        ThrowTypeError("not a constructor");
    }

    static result_t load(v8::Local<v8::Value> v, obj_ptr<Redis_base>& retVal)
    { return CALL_E_TYPEMISMATCH; }

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
    static void s_get_onsuberror(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void s_set_onsuberror(const v8::FunctionCallbackInfo<v8::Value>& args);
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

#include "ifs/Buffer.h"
#include "ifs/RedisHash.h"
#include "ifs/RedisList.h"
#include "ifs/RedisSet.h"
#include "ifs/RedisSortedSet.h"

namespace fibjs {
inline ClassInfo& Redis_base::class_info()
{
    static ClassData::ClassMethod s_method[] = {
        { "command", s_command, false, ClassData::ASYNC_SYNC },
        { "set", s_set, false, ClassData::ASYNC_SYNC },
        { "setNX", s_setNX, false, ClassData::ASYNC_SYNC },
        { "setXX", s_setXX, false, ClassData::ASYNC_SYNC },
        { "mset", s_mset, false, ClassData::ASYNC_SYNC },
        { "msetNX", s_msetNX, false, ClassData::ASYNC_SYNC },
        { "append", s_append, false, ClassData::ASYNC_SYNC },
        { "setRange", s_setRange, false, ClassData::ASYNC_SYNC },
        { "getRange", s_getRange, false, ClassData::ASYNC_SYNC },
        { "strlen", s_strlen, false, ClassData::ASYNC_SYNC },
        { "bitcount", s_bitcount, false, ClassData::ASYNC_SYNC },
        { "get", s_get, false, ClassData::ASYNC_SYNC },
        { "mget", s_mget, false, ClassData::ASYNC_SYNC },
        { "getset", s_getset, false, ClassData::ASYNC_SYNC },
        { "decr", s_decr, false, ClassData::ASYNC_SYNC },
        { "incr", s_incr, false, ClassData::ASYNC_SYNC },
        { "setBit", s_setBit, false, ClassData::ASYNC_SYNC },
        { "getBit", s_getBit, false, ClassData::ASYNC_SYNC },
        { "exists", s_exists, false, ClassData::ASYNC_SYNC },
        { "type", s_type, false, ClassData::ASYNC_SYNC },
        { "keys", s_keys, false, ClassData::ASYNC_SYNC },
        { "del", s_del, false, ClassData::ASYNC_SYNC },
        { "expire", s_expire, false, ClassData::ASYNC_SYNC },
        { "ttl", s_ttl, false, ClassData::ASYNC_SYNC },
        { "persist", s_persist, false, ClassData::ASYNC_SYNC },
        { "rename", s_rename, false, ClassData::ASYNC_SYNC },
        { "renameNX", s_renameNX, false, ClassData::ASYNC_SYNC },
        { "sub", s_sub, false, ClassData::ASYNC_SYNC },
        { "unsub", s_unsub, false, ClassData::ASYNC_SYNC },
        { "psub", s_psub, false, ClassData::ASYNC_SYNC },
        { "unpsub", s_unpsub, false, ClassData::ASYNC_SYNC },
        { "pub", s_pub, false, ClassData::ASYNC_SYNC },
        { "getHash", s_getHash, false, ClassData::ASYNC_SYNC },
        { "getList", s_getList, false, ClassData::ASYNC_SYNC },
        { "getSet", s_getSet, false, ClassData::ASYNC_SYNC },
        { "getSortedSet", s_getSortedSet, false, ClassData::ASYNC_SYNC },
        { "dump", s_dump, false, ClassData::ASYNC_SYNC },
        { "restore", s_restore, false, ClassData::ASYNC_SYNC },
        { "close", s_close, false, ClassData::ASYNC_SYNC }
    };

    static ClassData::ClassProperty s_property[] = {
        { "onsuberror", s_get_onsuberror, s_set_onsuberror, false }
    };

    static ClassData s_cd = {
        "Redis", false, s__new, NULL,
        ARRAYSIZE(s_method), s_method, 0, NULL, ARRAYSIZE(s_property), s_property, 0, NULL, NULL, NULL,
        &object_base::class_info(),
        false
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
    ARG_LIST(1);

    hr = pInst->command(v0, v1, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_set(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(Union_set_key, 0);
    ARG(Union_set_value, 1);
    OPT_ARG(int64_t, 2, 0);

    hr = pInst->set(v0, v1, v2);

    METHOD_VOID();
}

inline void Redis_base::s_setNX(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(Union_setNX_key, 0);
    ARG(Union_setNX_value, 1);
    OPT_ARG(int64_t, 2, 0);

    hr = pInst->setNX(v0, v1, v2);

    METHOD_VOID();
}

inline void Redis_base::s_setXX(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(Union_setXX_key, 0);
    ARG(Union_setXX_value, 1);
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

    ARG_LIST(0);

    hr = pInst->mset(v0);

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

    ARG_LIST(0);

    hr = pInst->msetNX(v0);

    METHOD_VOID();
}

inline void Redis_base::s_append(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_append_key, 0);
    ARG(Union_append_value, 1);

    hr = pInst->append(v0, v1, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_setRange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(Union_setRange_key, 0);
    ARG(int32_t, 1);
    ARG(Union_setRange_value, 2);

    hr = pInst->setRange(v0, v1, v2, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_getRange(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 3);

    ARG(Union_getRange_key, 0);
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

    ARG(Union_strlen_key, 0);

    hr = pInst->strlen(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_bitcount(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 1);

    ARG(Union_bitcount_key, 0);
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

    ARG(Union_get_key, 0);

    hr = pInst->get(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_mget(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Array>, 0);

    hr = pInst->mget(v0, vr);

    METHOD_OVER(-1, 0);

    ARG_LIST(0);

    hr = pInst->mget(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_getset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_getset_key, 0);
    ARG(Union_getset_value, 1);

    hr = pInst->getset(v0, v1, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_decr(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int64_t vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 1);

    ARG(Union_decr_key, 0);
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

    ARG(Union_incr_key, 0);
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

    ARG(Union_setBit_key, 0);
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

    ARG(Union_getBit_key, 0);
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

    ARG(Union_exists_key, 0);

    hr = pInst->exists(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_type(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    exlib::string vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_type_key, 0);

    hr = pInst->type(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_keys(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<NArray> vr;

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

    ARG_LIST(0);

    hr = pInst->del(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_expire(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_expire_key, 0);
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

    ARG(Union_ttl_key, 0);

    hr = pInst->ttl(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_persist(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_persist_key, 0);

    hr = pInst->persist(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_rename(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_rename_key, 0);
    ARG(Union_rename_newkey, 1);

    hr = pInst->rename(v0, v1);

    METHOD_VOID();
}

inline void Redis_base::s_renameNX(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    bool vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_renameNX_key, 0);
    ARG(Union_renameNX_newkey, 1);

    hr = pInst->renameNX(v0, v1, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_sub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_sub_channel, 0);
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

    ARG(Union_unsub_channel, 0);

    hr = pInst->unsub(v0);

    METHOD_OVER(2, 2);

    ARG(Union_unsub_channel, 0);
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

inline void Redis_base::s_get_onsuberror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Function> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(0, 0);

    hr = pInst->getListener("suberror", vr);

    METHOD_RETURN();
}

inline void Redis_base::s_set_onsuberror(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(v8::Local<v8::Function>, 0);

    hr = pInst->setListener("suberror", v0);

    METHOD_VOID();
}

inline void Redis_base::s_pub(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    int32_t vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(2, 2);

    ARG(Union_pub_channel, 0);
    ARG(Union_pub_message, 1);

    hr = pInst->pub(v0, v1, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_getHash(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RedisHash_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_getHash_key, 0);

    hr = pInst->getHash(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_getList(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RedisList_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_getList_key, 0);

    hr = pInst->getList(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_getSet(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RedisSet_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_getSet_key, 0);

    hr = pInst->getSet(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_getSortedSet(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<RedisSortedSet_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_getSortedSet_key, 0);

    hr = pInst->getSortedSet(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_dump(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    obj_ptr<Buffer_base> vr;

    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(1, 1);

    ARG(Union_dump_key, 0);

    hr = pInst->dump(v0, vr);

    METHOD_RETURN();
}

inline void Redis_base::s_restore(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    METHOD_INSTANCE(Redis_base);
    METHOD_ENTER();

    METHOD_OVER(3, 2);

    ARG(Union_restore_key, 0);
    ARG(Union_restore_data, 1);
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
