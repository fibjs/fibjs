/*
 * RedisHash.cpp
 *
 *  Created on: Aug 4, 2014
 *      Author: lion
 */

#include "object.h"
#include "RedisHash.h"

namespace fibjs {

result_t RedisHash::set(Buffer_base* field, Buffer_base* value)
{
    Variant v;
    return m_rdb->doCommand("HSET", m_key, field, value, v);
}

result_t RedisHash::setNX(Buffer_base* field, Buffer_base* value)
{
    Variant v;
    return m_rdb->doCommand("HSETNX", m_key, field, value, v);
}

result_t RedisHash::mset(v8::Local<v8::Object> kvs)
{
    Variant v;
    return m_rdb->doCommand("HMSET", m_key, kvs, v);
}
result_t RedisHash::mset(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    Variant v;
    Redis::_arg a(args);
    return m_rdb->doCommand("HMSET", m_key, a, v);
}

result_t RedisHash::get(Buffer_base* field, obj_ptr<Buffer_base>& retVal)
{
    return m_rdb->doCommand("HGET", m_key, field, retVal);
}

result_t RedisHash::mget(v8::Local<v8::Array> fields, obj_ptr<List_base>& retVal)
{
    return m_rdb->doCommand("HMGET", m_key, fields, retVal);
}

result_t RedisHash::mget(const v8::FunctionCallbackInfo<v8::Value>& args, obj_ptr<List_base>& retVal)
{
    Variant v;
    Redis::_arg a(args);
    return m_rdb->doCommand("HMGET", m_key, a, retVal);
}

result_t RedisHash::incr(Buffer_base* field, int64_t num, int64_t& retVal)
{
    return m_rdb->doCommand("HINCRBY", m_key, field, num, retVal);
}

result_t RedisHash::getAll(obj_ptr<List_base>& retVal)
{
    return m_rdb->doCommand("HGETALL", m_key, retVal);
}

result_t RedisHash::keys(obj_ptr<List_base>& retVal)
{
    return m_rdb->doCommand("HKEYS", m_key, retVal);
}

result_t RedisHash::len(int32_t& retVal)
{
    return m_rdb->doCommand("HLEN", m_key, retVal);
}

result_t RedisHash::del(v8::Local<v8::Array> fields, int32_t& retVal)
{
    return m_rdb->doCommand("HDEL", m_key, fields, retVal);
}

result_t RedisHash::del(const v8::FunctionCallbackInfo<v8::Value>& args, int32_t& retVal)
{
    Variant v;
    Redis::_arg a(args);
    return m_rdb->doCommand("HDEL", m_key, a, retVal);
}

result_t RedisHash::exists(Buffer_base* field, bool& retVal)
{
    return m_rdb->doCommand("HEXISTS", m_key, field, retVal);
}
}
