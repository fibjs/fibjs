/*
 * RedisSet.cpp
 *
 *  Created on: Aug 5, 2014
 *      Author: lion
 */

#include "RedisSet.h"

namespace fibjs
{

result_t RedisSet::add(v8::Local<v8::Array> members, int32_t &retVal)
{
    return m_rdb->doCommand("SADD", m_key, members, retVal);
}

result_t RedisSet::add(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal)
{
    Redis::_arg a(args);
    return m_rdb->doCommand("SADD", m_key, a, retVal);
}

result_t RedisSet::remove(v8::Local<v8::Array> members, int32_t &retVal)
{
    return m_rdb->doCommand("SREM", m_key, members, retVal);
}

result_t RedisSet::remove(const v8::FunctionCallbackInfo<v8::Value> &args, int32_t &retVal)
{
    Redis::_arg a(args);
    return m_rdb->doCommand("SREM", m_key, a, retVal);
}

result_t RedisSet::len(int32_t &retVal)
{
    return m_rdb->doCommand("SCARD", m_key, retVal);
}

result_t RedisSet::exists(Buffer_base *member, bool &retVal)
{
    return m_rdb->doCommand("SISMEMBER", m_key, member, retVal);
}

result_t RedisSet::members(obj_ptr<List_base> &retVal)
{
    return m_rdb->doCommand("SMEMBERS", m_key, retVal);
}

result_t RedisSet::pop(obj_ptr<Buffer_base> &retVal)
{
    return m_rdb->doCommand("SPOP", m_key, retVal);
}

result_t RedisSet::randMember(v8::Local<v8::Value> &retVal)
{
    obj_ptr<Buffer_base> v;
    result_t hr;

    hr = m_rdb->doCommand("SRANDMEMBER", m_key, v);
    if (hr < 0)
        return hr;

    retVal = GetReturnValue(v);

    return 0;
}

result_t RedisSet::randMember(int32_t count, v8::Local<v8::Value> &retVal)
{
    obj_ptr<List_base> v;
    result_t hr;

    hr = m_rdb->doCommand("SRANDMEMBER", m_key, count, v);
    if (hr < 0)
        return hr;

    retVal = v->wrap();

    return 0;
}

}
